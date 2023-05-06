#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct s_request
{
	std::string							method;
	std::string							target;
	std::string							version;
	std::map<std::string, std::string>	headers;
	std::vector<std::string>			body;
}				t_request;

typedef struct s_response
{
	std::string							htmlFile;
	std::string							extension;
	int									n;
}				t_response;

class webserv
{
	private:
		std::string 		_conf_file;
		std::string 		_ip_address;
		int					_port;
		int					_socket;
		int					_new_socket;
		int					_socket_address_len;
		struct sockaddr_in	_socket_address;
		std::string			_server_message;
		t_request			_request;
		t_response			_response;

	public:
		webserv();
		webserv(std::string ip_address, int port);
		~webserv();

		void			start_server();
		void			start_listening();
		void			accept_conection();
		void			request();
		void			response();
		std::string		get();

		/*
			In case of any error in the server, we can throw a
			error to prevent any unwanted exit an be able to
			restart the server
		*/
		class SocketException: public std::runtime_error
		{
			public:
				SocketException(std::string str): std::runtime_error(str) {};
		};
};

std::vector<std::string>	split(std::string str, std::string separator);
void						debug_log(std::string log);
std::string					read_file(std::string file_name);
std::string					get_extension(std::string file_name);

#endif
