#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <sstream>
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
		int					_socket_address_len;
		struct sockaddr_in	_socket_address;
		std::string			_server_message;

	public:
		webserv();
		webserv(std::string ip_address, int port);
		~webserv();

		void start_server();
		void start_listening();
		void debug_log(std::string log);
		void request();
		void response();

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

#endif
