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
        exit(EXIT_FAILURE);
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

		ocurrence = isSocketFd(event_fd);
		if (ocurrence != this->server_list.end())
		{
            cout << "kq " << this->_kq << " " << "array " << this->events << endl;
			(*ocurrence)->acceptClient(this->events, this->active_fds, this->_kq);
		}
		else if (events[i].flags & EV_EOF)
		{
			stringstream ss;

			ss << "EV EOF: fd " << event_fd;
			Logger::log(ss.str(), INFO);
			close(event_fd);
		}
		else if (events[i].filter == EVFILT_READ)
		{
			Server *reader;

			cout << "Reading: " << events[i].ident << "\n";
			if (this->active_fds.count(event_fd))
			{
				reader = this->active_fds.find(event_fd)->second;
			}
			else
			{
				reader = this->active_fds.begin()->second;
			}
			reader->message.request(event_fd, events[i].data);
			reader->enableWrite(this->_kq, event_fd);
		}
		else if (events[i].filter == EVFILT_WRITE)
		{
			Server *responder;

			cout << "Writing: " << events[i].ident << "\n";
            if (this->active_fds.count(event_fd))
			{
				responder = this->active_fds.find(event_fd)->second;
			}
            else
			{
				responder = this->active_fds.begin()->second;
			}
			responder->message.response(event_fd);
			responder->disableWrite(this->_kq, event_fd);
		}
	}
}

void ServerHandler::mainLoop()
{
	for (;;)
	{
		Logger::log("New loop", WARNING);
		this->updateEvents();
		this->eventLoop();
	}
}
