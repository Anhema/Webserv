#include "CGI.hpp"

CGI::CGI(){}
CGI::~CGI(){}

char** CGI::crete_env(string file_path, t_request request, size_t contentLength, string contentType)
{
	string path_info = "PATH_INFO=\"" + file_path + "\"";
	string host = "REMOTE_HOST=\"" + request.headers["Host"] + "\"";
	string content_type = "CONTENT_TYPE=\"" + contentType + "\"";
	string content_length = "CONTENT_LENGTH=\"" + std::to_string(contentLength) + "\"";

	char *env[] =
	{
		(char *)path_info.c_str(),
		(char *)host.c_str(),
		(char *)content_type.c_str(),
		(char *)content_length.c_str(),
		NULL
	};

	char** ret = env;
	return (ret);
}

string CGI::exec_cgi(string file_path, string body)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		std::cout << "ERROR\n";
		return ("");
	}

	std::string interpreter = "";
	if (Utils::get_extension(file_path) == "php")
		interpreter = "/usr/bin/php";
	else if (Utils::get_extension(file_path) == "sh")
		interpreter = "/bin/bash";
	else
		return ("");

	char* args[] = {&interpreter[0], &file_path[0], NULL};
	//char** env = this->crete_env(file_path, request, request.content_length, "text");

	string path_info = "PATH_INFO=" + file_path;
	string content_length = "CONTENT_LENGTH=" + std::to_string(body.size());
	string content = "QUERY_STRING=" + body;
	string num1 = "num1=1";
	string num2 = "num2=2";

	char *env[] =
	{
		(char *)path_info.c_str(),
		(char *)content_length.c_str(),
		(char *)content.c_str(),
		(char *)num1.c_str(),
		(char *)num2.c_str(),
		NULL
	};

	pid_t pid = fork();
	if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		dup2(pipefd[1], 2);
		close(pipefd[1]);
		execve(args[0], args, env);
		std::cerr << "Error";
		exit (1);
	}
	close(pipefd[1]);
	char buffer[1024];
	ssize_t bytesRead;
	string output = "";
	while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) != 0)
	{
		buffer[bytesRead] = '\0';
		output.append(buffer);
	}
	close(pipefd[0]);
	return (output);
}
