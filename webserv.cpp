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


	while (true)
	{
		this->accept_conection();

		std::ostringstream log;
        log
		<< "*****************CONNECTION ACCEPTED*****************\n\n"
        << "Server accept incoming connection\nADDRESS: " 
        << inet_ntoa(this->_socket_address.sin_addr) << ";\nPORT: " 
        << ntohs(this->_socket_address.sin_port)
		<< "\n\n****************************************************\n";

		this->debug_log(log.str());
		this->request();
		this->response();
	}
}

/*
	The accept() is used to process each connection thread in the queue created by listen().
	It does this by creating a new socket with a connection thread, which can be used to receive and send data between the client and the server.

	int accept(int sockfd,  struct sockaddr *restrict addr, socklen_t *restrict addrlen);

	sockfd = is the server’s socket
	addr = is a pointer to a socket address
	addrlen = is the length of the address object passed prior.

	accept() returns the new peer socket and writes the socket address information into the socket address pointer passed in as the second argument.
*/
void webserv::accept_conection()
{
	this->_new_socket = accept(this->_socket, (sockaddr *)&this->_socket_address, (socklen_t *)&this->_socket_address_len);
	if (this->_new_socket < 0)
	{
		std::ostringstream log;
        log
		<< "*****************CONNECTION REFUSED*****************"
        << "Server failed to accept incoming connecton\n ADDRESS: " 
        << inet_ntoa(this->_socket_address.sin_addr) << ";\n PORT: " 
        << ntohs(this->_socket_address.sin_port)
		<< "****************************************************";

		this->debug_log(log.str());
	}
}

/*
	A buffer is used to store the data transmitted in the request over the connection to the server.
	WITH http://localhost:8080 The REQUEST is printed in server logs NOT aviable with https://localhost:8080. Request is not printed
*/
void webserv::request()
{
	const int BUFFER_SIZE = 30720;

	char buffer[BUFFER_SIZE] = {0};
	if (read(this->_new_socket, buffer, BUFFER_SIZE) < 0)
	{
		throw (webserv::SocketException("Failed to read bytes from client socket connection"));
	}

	std::cout << buffer << "\n";
}

/*
	The response data is returned to the client via the socket connection using the write() system call.
	The functions take in the socket object, the message data as well as the size of the message data and write the data to the socket so the client receives a response on their end of the connection.
*/
void webserv::response()
{
	size_t send_bytes = 0;

	std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	std::ostringstream message;
	message << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
	<< htmlFile;
	
	this->_server_message = message.str();

	send_bytes = write(this->_new_socket, this->_server_message.c_str(), this->_server_message.size());
	if (send_bytes == this->_server_message.size())
		this->debug_log("-------- SERVER RESPONSE SENT TO CLIENT --------");
	else
		this->debug_log("-------- ERROR SENDING RESPONSE TO CLIENT --------");
}

void webserv::debug_log(std::string log)
{
	std::cout << log << "\n";
}
