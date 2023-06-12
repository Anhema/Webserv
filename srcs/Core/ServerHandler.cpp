#include "ServerHandler.hpp"

ServerHandler::ServerHandler(std::vector<t_server_config> configurations)
{
	for (std::vector<t_server_config>::iterator conf_it = configurations.begin(); conf_it != configurations.end(); conf_it++)
	{
		cout << "Llega\n";
		for (std::vector<int>::iterator port_it = conf_it->ports.begin(); port_it != conf_it->ports.end(); port_it++)
		{
			cout << "New server\n";
			cout << "Conf ip: " << (conf_it->ip) << endl;
			this->server_list.push_back(new Server(*conf_it, *port_it));
		}
	}
	this->startKqueue();
	this->monitorSockets();
}

ServerHandler::ServerHandler(int server_count): _server_count(server_count)
{
	this->monitorSockets();
}

ServerHandler::~ServerHandler()
{
	this->server_list.clear();
}


void ServerHandler::startKqueue()
{
	this->_kq = kqueue();

	if (_kq == -1)
		throw (std::runtime_error("couldn't start kqueue"));
}

void ServerHandler::monitorSockets()
{
	this->socketEvents.reset(new struct kevent[this->server_list.size()]);

	for (size_t i = 0; i < this->server_list.size(); i++)
	{
		EV_SET(socketEvents.get(), server_list.at(i)->getSocket(), EVFILT_READ, EV_ADD, 0, 0, 0);
		stringstream ss;

		kevent(this->_kq, socketEvents.get(), 1, NULL, 0, NULL);
		ss << "Monitoring socket fd: " << this->server_list.at(i)->getSocket();
		Logger::log(ss.str(), INFO);
	}
}

server_iterator ServerHandler::isSocketFd(const fd file_descripor)
{
	server_iterator it;
	for (it = this->server_list.begin(); it != this->server_list.end(); it++)
		if ((*it)->getSocket() == file_descripor)
			return it;
	return it;
}

void ServerHandler::updateEvents()
{
	this->_new_events = kevent(this->_kq, NULL, 0, events, 1, NULL);

	if (this->_new_events == -1)
		throw (std::runtime_error("error retrieving new events"));
}

void ServerHandler::eventLoop()
{
	for (int i = 0; i < this->_new_events; i++)
	{
		server_iterator	ocurrence;
		Server			*server = NULL;
		const fd		event_fd = events[i].ident;

		ocurrence = isSocketFd(event_fd);
		if (ocurrence != this->server_list.end())
			(*ocurrence)->acceptClient(this->active_fds, this->_kq);
		else if (events[i].flags & EV_EOF)
		{
			if (this->active_fds.count(event_fd))
				server = this->active_fds.find(event_fd)->second;
			else
				server = this->active_fds.begin()->second;
			server->disconnectClient(this->_kq, event_fd);
		}
		else if (events[i].filter == EVFILT_READ)
		{
			if (this->active_fds.count(event_fd))
				server = this->active_fds.find(event_fd)->second;
			else
				server = this->active_fds.begin()->second;

			server->message[event_fd].request(event_fd, events[i].data);
			if (server->message[event_fd].finishedReading)
				server->enableWrite(this->_kq, event_fd);
		}
		else if (events[i].filter == EVFILT_WRITE)
		{
            if (this->active_fds.count(event_fd))
				server = this->active_fds.find(event_fd)->second;
            else
				server = this->active_fds.begin()->second;

			server->message[event_fd].response(event_fd, events[i].data);
			server->disableWrite(this->_kq, event_fd);
			if (server->message[event_fd].getConnectionType() == "close")
			{
				cout << "Getter:" << server->message[event_fd].getConnectionType() << endl;
				server->disconnectClient(this->_kq, event_fd);
			}
		}
	}
}
void ServerHandler::mainLoop()
{
	for (;;)
	{
//		Logger::log("New loop", WARNING);
		this->updateEvents();
		this->eventLoop();
	}
}
