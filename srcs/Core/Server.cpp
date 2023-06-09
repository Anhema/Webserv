#include "Server.hpp"
#include "../Logger/Logger.hpp"


Server::Server(t_server_config options, int port):
_ip(options.ip),
_port(port),
_socketAddress_len(sizeof(_socketAddress)),
m_config(options)
{
	cout << "Enters\n";
    this->startSocketAddress();
    this->startSocket();
    this->bindSocket();
    this->startListen();
}

//Server::Server(std::string ip, int port, string name):  _ip(ip),
//                                                        _port(port),
//                                                        _socketAddress_len(sizeof(_socketAddress))
//{
//
//    this->startSocketAddress();
//    this->startSocket();
//    this->bindSocket();
//    this->startListen();
//}
//
//Server::Server():   _ip("0.0.0.0"),
//                    _port(0000),
//                    _socketAddress_len(sizeof(_socketAddress)),
//
//Server::Server(const Server &obj):  _ip(obj._ip),
//                                    _port(obj._port),
//                                    _socketAddress_len(sizeof(_socketAddress))
//									{}

Server::~Server() {

    cout << "Closing socket: " << _socket_fd << "\n";
	close(_socket_fd);
}

fd Server::getSocket() const { return _socket_fd; }

void Server::startSocketAddress()
{
	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_port = htons(this->_port);
	this->_socketAddress.sin_addr.s_addr = inet_addr(_ip.c_str());

	Logger::log("Socket address initialized succesfully", INFO);
}

void Server::startSocket()
{
	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_socket_fd < 0)
		throw (std::runtime_error("initialing fd"));

	const char reuse = 1;

	if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
		throw (std::runtime_error("SO_REUSEADDR"));

	fcntl(this->_socket_fd, F_SETFL, O_NONBLOCK);

	Logger::log("Socket created succesfully", INFO);
}

void Server::bindSocket()
{
	if (bind(_socket_fd, (sockaddr *)&_socketAddress, _socketAddress_len) < 0)
		throw (std::runtime_error("can't bind socket"));
	Logger::log("Socket binded succesfully", INFO);
}

void Server::enableEvent(int kq, const fd event_fd, struct kevent *k_struct, short event) const
{
	EV_SET(k_struct, event_fd, event, EV_ADD | EV_ENABLE, 0, 0, 0);

	if (kevent(kq, k_struct, 1, NULL, 0, NULL) < 0)
		throw (std::runtime_error("kqueue failed to add event"));
}

void Server::disableEvent(int kq, const fd event_fd, struct kevent *k_struct, short event) const
{
	EV_SET(k_struct, event_fd, event, EV_ADD | EV_DISABLE, 0, 0, 0);

	if (kevent(kq, k_struct, 1, NULL, 0, NULL) < 0)
		throw (std::runtime_error("kqueue failed to disable event"));
}

void Server::enableWrite(int kq, const fd event_fd) const
{
	EV_SET((struct kevent *)&this->_write_event, event_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, 0);

	if (kevent(kq, &this->_write_event, 1, NULL, 0, NULL) < 0)
		throw (std::runtime_error("kqueue failed to enable write"));
}

void Server::disableWrite(int kq, const fd event_fd) const
{
	EV_SET((struct kevent *)&this->_write_event, event_fd, EVFILT_WRITE, EV_DISABLE, 0, 0, 0);

	if (kevent(kq, &this->_write_event, 1, NULL, 0, NULL) < 0)
		throw (std::runtime_error("kqueue failed to disable write"));
}

void Server::disconnectClient(int kq, const fd client)
{
	EV_SET((struct kevent *)&this->_write_event, client, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
	if (kevent(kq, &this->_write_event, 1, NULL, 0, NULL) < 0)
		throw (std::runtime_error("kqueue failed to delete write"));

	EV_SET((struct kevent *)&this->_read_event, client, EVFILT_READ, EV_DELETE, 0, 0, 0);
	if (kevent(kq, &this->_read_event, 1, NULL, 0, NULL) < 0)
		throw (std::runtime_error("kqueue failed to delete read"));

	stringstream ss;

	ss << "EV EOF: fd " << client;
	Logger::log(ss.str(), INFO);

	close(client);
}

void Server::startListen()
{
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

fd Server::acceptClient(std::map<fd, Server *> &active_fd, int kq) const
{

    const fd	new_client = accept(this->_socket_fd, (sockaddr *)&_socketAddress, (socklen_t *)&_socketAddress);

    if (new_client == -1)
        throw (std::runtime_error("Failed to accept incoming connection"));

	fcntl(new_client, F_SETFL, O_NONBLOCK);
	this->enableEvent(kq, new_client, (struct kevent *)&this->_read_event, EVFILT_READ);
	this->disableEvent(kq, new_client, (struct kevent *)&this->_write_event, EVFILT_WRITE);

    stringstream ss;

    ss << "Accepted connection: " << new_client;
    Logger::log(ss.str(), INFO);
	active_fd[new_client] = (Server *)this;
    return (new_client);
}
