#ifndef SERVER_HPP
# define SERVER_HPP

# include "Utilities.hpp"
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <iostream>
# include <cstring>
# include <arpa/inet.h>
# include <stdlib.h>
# include <stdio.h>
# include <sstream>
# include <vector>
# include <sys/event.h>
# include <sys/time.h>
# include <fcntl.h>

class Server {
public:
	Server();
	Server(string ip, int port);
	Server(const Server &obj);
	~Server();

	Server operator=(const Server &obj);



private:
	const string		_ip;
	const int 			_port;
	const u_int32_t 	_socketAddress_len;
	fd 					_socket_fd;
	struct sockaddr_in	_socketAddress;

	static const u_int32_t 	max_listen_queue = 128;

private:
	void	bindSocket();
	void	startSocket();
	void	startSocketAddress();
	void 	startListen();
	void 	startKqueue();

};

#endif
