#include "ServerHandler.hpp"

ServerHandler::ServerHandler(int server_count): _server_count(server_count)
{
	this->startServers();
	this->startKqueue();
	this->monitorSockets();
}

ServerHandler::~ServerHandler()
{
	this->server_list.clear();
}

void ServerHandler::startServers()
{

    try
    {
        for (int i = 0; i < this->_server_count; i++)
            this->server_list.push_back(new Server(LOCALHOST, START_PORT + i, DEFAULT_NAME));
    }
    catch (std::exception &e)
    {
        this->_server_count += 3;
        this->startServers();
    }

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
	this->_new_events = kevent(this->_kq, NULL, 0, events, Server::maxEvents, NULL);

	if (this->_new_events == -1)
		throw (std::runtime_error("error retrieving new events"));
}

void ServerHandler::eventLoop()
{
	for (int i = 0; i < this->_new_events; i++)
	{
		server_iterator ocurrence;
		const fd event_fd = events[i].ident;

		if (events[i].flags & EV_ERROR)
		{
			Logger::log("EV_FLAG", ERROR);
			continue;
		}
		ocurrence = isSocketFd(event_fd);
		if (ocurrence != this->server_list.end())
		{
            cout << "kq " << this->_kq << " " << "array " << this->events << endl;
			(*ocurrence)->acceptClient(this->events, this->active_fds, this->_kq);
			continue;
		}
		if (events[i].flags & EV_EOF)
			close(event_fd);
		if (events[i].filter & EVFILT_READ)
		{
			cout << "Reading: " << events[i].ident << "\n";
			this->active_fds[event_fd]->message.request(event_fd, this->events[i].data);
//			server_list.at(0)->message.request(event_fd, events[i].data);
		}
		if (events[i].filter &EVFILT_WRITE)
		{
			cout << "Writing: " << events[i].ident << "\n";
			server_list.at(0)->message.response(event_fd);
		}
	}

}

void ServerHandler::mainLoop()
{
	for (;;)
	{
		this->updateEvents();
		this->eventLoop();
	}
}
