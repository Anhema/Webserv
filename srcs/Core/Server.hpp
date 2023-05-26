#ifndef SERVER_HPP
# define SERVER_HPP

# include "../Utilities/Utilities.hpp"
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
	Server(string ip, int port, string name);
	Server(const Server &obj);
	~Server();

	Server operator=(const Server &obj);
    fd  getSocket(void) const;
    string getName(void);



public:
	static const u_int16_t 	max_listen_queue = 128;
	static const u_int16_t	maxEvents = 1024;
    Message                 message;

private:
	const string		_ip;
	const int 			_port;
	const u_int32_t 	_socketAddress_len;
    const string        _server_name;
	fd 					_socket_fd;
	struct sockaddr_in	_socketAddress;

public:
    fd		    acceptClient(struct kevent *event_array, std::map<fd, Server *>, int kq) const;

private:
	void	    bindSocket();
	void    	startSocket();
	void	    startSocketAddress();
	void 	    startListen();
	void    	startKqueue();
	void	    startSocketEvents();

private:
	void 	    monitorConnection(const fd connection);
	void    	serverLoop();
	void 	    eventLoop(int new_events);
	void	    getEvents(int &events);
	void	    disconnectClient(const fd client);
};

#endif
