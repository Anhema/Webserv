#ifndef SERVER_HANDLER_HPP
# define SERVER_HANDLER_HPP

# include "../Utilities/Utilities.hpp"
# include "../Logger/Logger.hpp"
# include "Server.hpp"
# include "Config.hpp"


# define SERVER_COUNT 3
# define LOCALHOST "0.0.0.0"
# define DEFAULT_NAME "Server_default"
# define START_PORT 8080


typedef std::vector<Server *>::iterator server_iterator;

class ServerHandler {

public:
	ServerHandler(std::vector<t_server_config>configurations);
	ServerHandler(int server_count);
	~ServerHandler();
	ServerHandler(ServerHandler const &rhs);

	ServerHandler &operator=(ServerHandler const &rhs);

    void									mainLoop(void);

private:
	void 									startServer(t_server_config configuration);
	void									startKqueue(void);
	void									monitorSockets(void);
	void 									eventLoop(void);
	void									updateEvents(void);
	server_iterator 						isSocketFd(const fd file_descriptor);

private:
	std::vector<Server *>					server_list;
	std::unique_ptr<struct kevent[]>		socketEvents;
	std::map<fd, Server *>			        active_fds;
	struct kevent							events[Server::maxEvents];

private:
    int                                     _new_events;
	int			 							_server_count;
	int										_kq;

};


#endif
