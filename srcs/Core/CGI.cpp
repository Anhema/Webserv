#include "CGI.hpp"

CGI::CGI(){}
CGI::~CGI(){}

string CGI::exec_cgi(string file_path)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        std::cout << "ERROR\n";
        return ("");
    }

    std::string interpreter = "";
    if (Utils::get_extension(file_path) == "py")
        interpreter = "/usr/bin/python3";
    else if (Utils::get_extension(file_path) == "sh")
        interpreter = "/bin/bash";
    else
        return ("");

    char* args[] = {&interpreter[0], &file_path[0], NULL};
    char* env[] = {NULL};

    pid_t pid = fork();
    if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);
        close(pipefd[1]);
        execve(args[0], args, env);
        std::cerr << "Error al ejecutar el script CGI." << std::endl;
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
