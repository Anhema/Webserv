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
# include "Message.hpp"


class Server {
public:
	Server();
	Server(string ip, int port);
	Server(const Server &obj);
	~Server();

	Server operator=(const Server &obj);



private:
	static const u_int16_t 	max_listen_queue = 128;
	static const u_int16_t	maxEvents = 1024;

private:
	const string		_ip;
	const int 			_port;
	const u_int32_t 	_socketAddress_len;
	fd 					_socket_fd;
	struct sockaddr_in	_socketAddress;
	Message				_message;

private:
	int 				_kq;
	struct kevent		_socketEvents;
	struct kevent		_serverEvents[maxEvents];



private:
	void	bindSocket();
	void	startSocket();
	void	startSocketAddress();
	void 	startListen();
	void 	startKqueue();
	void	startSocketEvents();

private:
	void 	monitorConnection(const fd connection);
	void 	serverLoop();
	void 	eventLoop(int new_events);
	void	getEvents(int &events);
	int		acceptClient();
	void	disconnectClient(const fd client);
	size_t 	sendResponse(const fd client);
	void	receiveHeader(const fd client, size_t buffer_size);
};

#endif
