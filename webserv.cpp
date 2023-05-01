#include "webserv.hpp"

webserv::webserv() : _ip_address("0.0.0.0"), _port(80), _socket_address_len(sizeof this->_socket_address)
{
    std::cout << "Server empty constructor called\n";
    this->_socket_address.sin_family = AF_INET;
    this->_socket_address.sin_port = htons(this->_port);
    this->_socket_address.sin_addr.s_addr = inet_addr(this->_ip_address.c_str());
    this->start_server();   
}

webserv::webserv(std::string ip_address, int port) : _ip_address(ip_address), _port(port), _socket_address_len(sizeof this->_socket_address)
{
    this->_socket_address.sin_family = AF_INET;
    this->_socket_address.sin_port = htons(this->_port);
    this->_socket_address.sin_addr.s_addr = inet_addr(this->_ip_address.c_str());
    this->start_server();
}

webserv::~webserv()
{
    std::cout << "webserv stopped\n";
    close(this->_socket);
}

void webserv::start_server()
{
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
			throw (webserv::SocketException("Failed creating socket"));
    if (bind(this->_socket, (sockaddr *)&this->_socket_address, this->_socket_address_len) < 0)
    {
    	throw (webserv::SocketException("Cannot connect socket to address"));
        return ;
    }
    this->start_listening();
}

void webserv::start_listening()
{
    if (listen(this->_socket, 20) < 0)
    {
        throw (webserv::SocketException("Socket listen failed"));
        return;     
    }
    std::ostringstream log;
    log << "\n*** WEBSERV ***\nListening on ADDRESS: " 
        << this->_ip_address
        << "\nPORT: " << ntohs(this->_socket_address.sin_port) 
        << "\n\n";

    this->debug_log(log.str());
}

void webserv::debug_log(std::string log)
{
    std::cout << log << "\n";
}
