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

string CGI::exec_cgi(string file_path, string body, string method)
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

	char* args[] = {&interpreter[0], &file_path[0], (char *)body.c_str(), NULL};
	//char** env = this->crete_env(file_path, request, request.content_length, "text");
	cout << "\n\n--" << body << "--\n\n";
	std::vector<string> env_list;
	env_list.push_back("SERVER_SOFTWARE=");
	env_list.push_back("SERVER_NAME=localhost");
	env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");

	env_list.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_list.push_back("SERVER_PORT=8080");
	env_list.push_back("REQUEST_METHOD=" + method);
	env_list.push_back("PATH_INFO=" + file_path);
	env_list.push_back("PATH_TRANSLATED=" + file_path);
	env_list.push_back("SCRIPT_NAME=" + file_path);
	env_list.push_back("SCRIPT_FILENAME=file:///Users/aherrero/Desktop/Cursus/Webserv/" + file_path);
	env_list.push_back("QUERY_STRING=" + body);
	env_list.push_back("SERVER_SIGNATURE=");
	env_list.push_back("HTTP_HOST=localhost");
	env_list.push_back("HTTP_USER_AGENT=aherrero");
	env_list.push_back("REMOTE_ADDR=127.0.0.1");
	env_list.push_back("SERVER_ADDR=127.0.0.1");
	env_list.push_back("CONTENT_LENGTH=" + std::to_string(body.length()));
	env_list.push_back("CONTENT_TYPE=application/x-www-form-urlencoded;charset=utf-8");

	char **env = NULL;
	env = new char*[env_list.size() + 1];;

	for (size_t i = 0; i < env_list.size(); i++)
	{
    	env[i] = new char[env_list[i].size() + 1];
    	::strncpy(env[i], env_list[i].c_str(), env_list[i].size() + 1);

		cout << "\n" << env[i] << "\n";
	}
	env[env_list.size()] = nullptr;

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
	delete []env;
	return (output);
}
