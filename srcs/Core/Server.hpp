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
# include "Config.hpp"

class Server {
public:
	Server();
	Server(t_server_config options, int port);
	Server(string ip, int port, string name);
	Server(const Server &obj);
	~Server();

	Server	operator=(const Server &obj);
    fd		getSocket(void) const;


public:
	static const u_int16_t 	max_listen_queue = 128;
	static const u_int16_t	maxEvents = 1024;
    Message                 message[Server::maxEvents];

private:
	const	string			_ip;
	const	int 			_port;
	const	u_int32_t 		_socketAddress_len;
	const	t_server_config m_config;
	struct	sockaddr_in		_socketAddress;
	struct	kevent			_read_event;
	struct	kevent			_write_event;
	fd 						_socket_fd;

public:
    fd		 	   acceptClient(std::map<fd, Server *> &socket_map, int kq) const;

public:
	void 			disconnectClient(int kq, const fd client);
	void			enableEvent(int kq, const fd event_fd, struct kevent *k_struct, short event) const;
	void			disableEvent(int kq, const fd event_fd, struct kevent *k_struct, short event) const;
	void			enableWrite(int kq, const fd event_fd) const;
	void			disableWrite(int kq, const fd event_fd) const;

private:
	void	    	bindSocket();
	void    		startSocket();
	void	    	startSocketAddress();
	void 	    	startListen();

};

#endif
