#include "Server.hpp"
#include "Logger.hpp"

Server::Server():_ip("0.0.0.0"), _port(0000), _socketAddress_len(sizeof(_socketAddress)) {}

Server::~Server() {

	close(_socket_fd);
}

void Server::startSocketAddress() {

	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_port = htons(this->_port);
	this->_socketAddress.sin_addr.s_addr = inet_addr(_ip.c_str());

	stringstream ss;

	Logger::log("Socket address initialized succesfully", INFO);

}

void Server::startSocket() {

	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_socket_fd < 0)
		throw (std::runtime_error("initialing fd"));
	Logger::log("Socket created succesfully", INFO);

}

void Server::bindSocket() {

	if (bind(_socket_fd, (sockaddr *)&_socketAddress, _socketAddress_len) < 0)
		throw (std::runtime_error("can't bind socket"));
	Logger::log("Socket binded succesfully", INFO);
}

void Server::startListen() {

	stringstream ss;

	if (listen(_socket_fd, Server::max_listen_queue) < 0)
		throw (std::runtime_error("listen failed to start"));

	ss	<< "Server started listening on address: "
		<< inet_ntoa(_socketAddress.sin_addr)
		<< " port: "
		<< ntohs(_socketAddress.sin_port);

	Logger::log(ss.str(), INFO);
}

void Server::startKqueue() {

	_kq = kqueue();

	if (_kq == -1)
		throw (std::runtime_error("kqueue can't initiliaze"));
	Logger::log("Kqueue started succesfully", INFO);
}

void Server::startSocketEvents() {

	EV_SET(&_socketEvents, _socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(_kq, &_socketEvents, 1, NULL, 0, NULL);

}

void Server::getEvents(int &events) {
	events = kevent(_kq, NULL, 0, _serverEvents, Server::maxEvents, NULL);

	if (events == -1)
		throw (std::runtime_error("kevent faied to read new events"));

}

void Server::monitorConnection(const fd connection) {

	EV_SET(_serverEvents, connection, EVFILT_WRITE | EVFILT_READ, EV_ADD, 0, 0, 0);

	if (kevent(_kq, _serverEvents, 1, NULL, 0, NULL) == 1) {

		stringstream ss;

		ss << "kevent failed to monitor fd:" << connection;
		throw (std::runtime_error(ss.str()));
	}


}

void Server::receiveHeader(const fd client, size_t buffer_size) {

	unique_ptr<char []> buffer;

	buffer.reset(new char[buffer_size]);
	ssize_t read_size = recv(client, buffer.get(), buffer_size, 0);

	if (read_size == -1)
		Logger::log("read error", WARNING);
	else
		Logger::log("header received succesfully", INFO);
}

// Todo
 size_t Server::sendResponse(const fd client) {

	std::ostringstream 			header;
	string						html;
	size_t 						write_len;

	html	= "<!DOCTYPE html><html lang=\"en\"><body><img src=\"path\"/<h1> HOME </h1><p> Hello from your Server :) </p></body></html>";

	header <<  "HTTP/1.1 200 OK\nContent-Type: text/html\nServer: Hello\nContent-Length: " << html.size() << "\n\n"
			 << html;

	write_len = send(client, header.str().c_str(), header.str().size(), MSG_DONTWAIT);

	if (write_len != header.str().size())
		Logger::log("Write len is not equal to response size", WARNING);
	else
		Logger::log("Response sent succesfully", INFO);
	return (write_len);
}

void Server::acceptClient() {

	const fd new_connection = accept(_socket_fd, (sockaddr *)&_socketAddress, (socklen_t *)&_socketAddress_len);
	if (new_connection == -1)
		throw (std::runtime_error("Failed to accept incoming connection"));

	this->monitorConnection(new_connection);
	// Todo
	// Logger option for connections blue?
	stringstream ss;

	ss << "Accepted connection: " << new_connection;
	Logger::log(ss.str(), INFO);
}

void Server::disconnectClient(const fd client) {

	EV_SET(_serverEvents, client, EVFILT_READ | EVFILT_WRITE, EV_DELETE, 0, 0, 0);
	close(client);

}

void Server::eventLoop(int new_events) {

	for (int i = 0; i < new_events; i++)
	{
		const fd event_fd = _serverEvents[i].ident;

		if (event_fd == _socket_fd) this->acceptClient();
		if (_serverEvents[i].flags & EV_EOF) this->disconnectClient(event_fd);
		if (_serverEvents[i].filter & EVFILT_READ) this->receiveHeader(event_fd, _serverEvents[i].data);
		if (_serverEvents[i].filter &EVFILT_WRITE) this->sendResponse(event_fd);
	}
}

void Server::serverLoop() {
	int new_events;

	Logger::log("Server loop started", INFO);
	for (;;)
	{
		this->getEvents(new_events);
		this->eventLoop(new_events);
	}
}

Server::Server(std::string ip, int port): _ip(ip), _port(port), _socketAddress_len(sizeof(_socketAddress)) {

	this->startSocketAddress();
	this->startSocket();
	this->bindSocket();
	this->startListen();
	this->startKqueue();
	this->startSocketEvents();
	this->serverLoop();
}
