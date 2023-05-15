#include "Server.hpp"
#include "Logger.hpp"

Server::Server():_ip("0.0.0.0"), _port(0000), _socketAddress_len(sizeof(_socketAddress)) {}

Server::~Server() {}

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
		<< ntohs(_socketAddress.sin_port)
		<< endl;

	Logger::log(ss.str(), INFO);
}

void Server::startKqueue() {


}

Server::Server(std::string ip, int port): _ip(ip), _port(port), _socketAddress_len(sizeof(_socketAddress)) {

	this->startSocketAddress();
	this->startSocket();
	this->bindSocket();
	this->startListen();

}
