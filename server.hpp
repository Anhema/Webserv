#ifndef SERVER_HPP
# define SERVER_HPP

# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <iostream>
# include <cstring>
# include <arpa/inet.h>
# include <stdlib.h>
# include <stdio.h>
#include <sstream>


# define BUFFER_SIZE 300

using std::cout;
using std::cerr;
using std::endl;
using std::string;




class Server {
public:
	Server() {
		cout << "Server constructed\n";
	};

	Server(string ip_address, int port): _ip_address(ip_address), _port(port) {
		cout << "Server ip: " << _ip_address << " listening on port: " << port << endl;
		m_socketAddress_len = sizeof(m_socketAddress);
	};

	~Server() {
		cout << "Server destructed\n";
	};

	class SocketExceptionCantBind: public std::runtime_error {
	public:
		SocketExceptionCantBind(): std::runtime_error("socket cant bind to address") {};
	};

	class ListenExceptionCantInit: public std::runtime_error {
	public:
		ListenExceptionCantInit(): std::runtime_error("Can't start listening") {};
	};

	class SocketException: public std::runtime_error {
	public:
		SocketException(): std::runtime_error("socket malfunction") {};
	};

	void initSocket() {
		_socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (_socketFd < 0)
			throw (Server::SocketException());

		cout << "Socket fd initialized with fd:" << _socketFd << endl;

		m_socketAddress.sin_family = AF_INET;
		m_socketAddress.sin_port = htons(_port);
		m_socketAddress.sin_addr.s_addr = inet_addr(_ip_address.c_str());

		if (bind(_socketFd, (sockaddr*)&m_socketAddress, m_socketAddress_len) < 0)
			throw (Server::SocketExceptionCantBind());
		else
			cout << "bind succesfull\n";

	};
	void ServerListener() {
		if (listen(_socketFd, 20) < 0)
			throw (Server::ListenExceptionCantInit());
		else {
			cout << "\nListening on ADDRESS: "
				 << inet_ntoa(m_socketAddress.sin_addr)
				 << " PORT: " << ntohs(m_socketAddress.sin_port)
				 << " \n\n";
		}
		listenLoop();

	}

	void accepConnection() {
		_newSocket = accept(_socketFd, (sockaddr*)&m_socketAddress, (socklen_t*)&m_socketAddress_len);
		cout << "Accepting...";
		if (_newSocket == -1)
			throw (Server::SocketException());


	}

	void listenLoop() {
		while (true) {
			char buffer[BUFFER_SIZE];
			accepConnection();
			cout << "Accepted\n";
			if (read(_newSocket, buffer, BUFFER_SIZE) < 0)
				exit(EXIT_FAILURE);
			cout << "======REQUEST-HEADER==========\n\n" <<  buffer << "\n==========END======\n\n";
			response();


		}
	}

	void checkResponse()
	{
		char buffer[BUFFER_SIZE];

		cout << "===== NEW SOCKET CHECK =====\n";
		read(_newSocket, buffer, BUFFER_SIZE);


		string newSocketContent(buffer);
		cout << newSocketContent;
		cout << "===========END=========\n";
	}

	void response()
	{
		std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
		string plainText = "Hello World";
		std::ostringstream response;

		response << "HTTP/1.1 200 OK\nContent-Type: text/plain\nServer: Hello\nContent-Length: " << plainText.size() << "\n\n"
		   << plainText;
		std::size_t sendBytes = write(_newSocket, response.str().c_str(), htmlFile.size());



		if (sendBytes != htmlFile.size())
		{
			cout << "=========ERROR SENDING=========\n" << "\t\tMessage size: " << htmlFile.size() << "\n" << "Sent: " << sendBytes << "\n";
		}
		else
		{
			cout << "==========RESPONSE==========\n";
			cout << response.str() << "\n";

			cout << "==========RESPONSE SEND SUCCESFULLY===============\n\n";
		}

		checkResponse();

	}

private:
	string _ip_address;
	int _port;
	int	_socketFd;
	int _newSocket;
//	long _incomingMessage;
	struct sockaddr_in m_socketAddress;
	unsigned int m_socketAddress_len;
	string m_serverMessage;

};

#endif
