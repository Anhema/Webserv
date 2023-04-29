#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class webserv
{
	private:
		std::string 		_conf_file;
		std::string 		_ip_address;
		int					_port;
		int         		_socket;
		struct sockaddr_in	_socker_address;

	public:
		webserv();
		webserv(std::string conf_file);
		~webserv();

		void start_server();
		void debug_log(std::string log);

		class SocketException: public std::runtime_error
		{
			public:
				SocketException(): std::runtime_error("Failed creating socket") {};
		};
};

#endif