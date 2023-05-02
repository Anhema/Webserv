#include "webserv.hpp"

webserv::webserv() : _ip_address("0.0.0.0"), _port(80), _socket_address_len(sizeof this->_socket_address)
{
    std::cout << "Server empty constructor called\n";
    this->_socket_address.sin_family = AF_INET;
    this->_socket_address.sin_port = htons(this->_port);
    this->_socket_address.sin_addr.s_addr = inet_addr(this->_ip_address.c_str());
    this->start_server();   
}

/*
    By default the server class will initialize the server with 0.0.0.0 IP and the PORT 80
    From the configuration file we will be able to get the specific port and the IP address
    
    struct sockaddr_in	_socket_address;

    struct sockaddr_in
    {
        short            sin_family;   // This is the socket family. AF_INET
        unsigned short   sin_port;     // The port to be used by our server. htons(8080)
        struct in_addr   sin_addr;     // This is the IP address we will be using for the server.
        char             sin_zero[8];  // This member variable has no “real” utility in the sockaddr_in structure. It is a buffer used to adjust the size of the sockaddr_in in cases where you need to cast it to a sockaddr type.
    };
    
    struct in_addr
    {
        unsigned long s_addr;
    };
*/
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

/*
    First we create the socker to get the "file descriptor". AF_INET are the IPv4 Internet protocols for a TCP/IP socket.
    SOCK_STREAM describes the type of communication structure the socket will allow for this protocol family.
    We use this to allow for reliable, full-duplex byte streams.

    SOCKET socket(int domain, int type, int protocol);

    domain = This argument specifies the communication domain. It represents the protocol family that the socket will belong to. (AF_INET)
    type = This argument describes the type of communication structure the socket will allow for this protocol family. (SOCK_STREAM)
    protocol = This specifies the particular protocol the socket will use from the given family of protocols that support the chosen type of communication. (0)

    After we must bind to tie a socket address to a given socket.

    When we had created the socker and bind it, we can start listening.
*/
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

/*
    Now we have to setup the listening for incoming connections on our socket address.
    listen() gets the incoming connections on a queue

    int listen(int sockfd, int backlog);

    sockfd = is the socket
    backlog = is the maximum number of connection threads we want to be able to hold at once. If a client tries to connect when the queue is full, they will get rejected by the server.
*/
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

void webserv::request()
{

}

void webserv::response()
{

}

void webserv::debug_log(std::string log)
{
    std::cout << log << "\n";
}
