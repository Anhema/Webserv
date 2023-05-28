#include "Server.hpp"
#include "../Logger/Logger.hpp"

Server::Server(std::string ip, int port, string name):  _ip(ip),
                                                        _port(port),
                                                        _socketAddress_len(sizeof(_socketAddress)),
                                                        _server_name(name)
{

    this->startSocketAddress();
    this->startSocket();
    this->bindSocket();
    this->startListen();
}

Server::Server():   _ip("0.0.0.0"),
                    _port(0000),
                    _socketAddress_len(sizeof(_socketAddress)),
                    _server_name("default") {}

Server::Server(const Server &obj):  _ip(obj._ip),
                                    _port(obj._port),
                                    _socketAddress_len(sizeof(_socketAddress)),
                                    _server_name(obj._server_name) {}

Server::~Server() {

    cout << "Closing socket: " << _socket_fd << "\n";
	close(_socket_fd);
}

fd Server::getSocket() const { return _socket_fd; }

string Server::getName() { return _server_name; }

void Server::startSocketAddress() {

	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_port = htons(this->_port);
	this->_socketAddress.sin_addr.s_addr = inet_addr(_ip.c_str());

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

	ss	<< "Socked with fd: "
        << _socket_fd
        <<" started listening on address: "
		<< inet_ntoa(_socketAddress.sin_addr)
		<< " port: "
		<< ntohs(_socketAddress.sin_port);
	Logger::log(ss.str(), INFO);
}

fd Server::acceptClient(struct kevent *event_array, std::map<fd, Server *>active_fd, int kq) const
{
    const fd new_client = accept(this->_socket_fd, (sockaddr *)&_socketAddress, (socklen_t *)&_socketAddress);

    if (new_client == -1)
        throw (std::runtime_error("Failed to accept incoming connection"));

    EV_SET(event_array, new_client, EVFILT_WRITE, EV_ADD, 0, 0, 0);
    EV_SET(event_array, new_client, EVFILT_READ, EV_ADD, 0, 0, 0);

    if (kevent(kq, event_array, 1, NULL, 0, NULL) == -1)
	{
        stringstream ss;

        ss << "kevent failed to monitor fd: " << new_client;
        throw (std::runtime_error(ss.str()));
    }

    stringstream ss;

    ss << "Accepted connection: " << new_client;
    Logger::log(ss.str(), INFO);
	active_fd[new_client] = (Server *)this;
    return (new_client);
}
