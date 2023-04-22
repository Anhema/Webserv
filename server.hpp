#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <iostream>
# include <cstring>
# include <arpa/inet.h>
# include <stdlib.h>
# include <stdio.h>


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
	}

private:
	string _ip_address;
	int _port;
	int	_socketFd;
	int _newSocket;
	long _incomingMessage;
	struct sockaddr_in m_socketAddress;
	unsigned int m_socketAddress_len;
	string m_serverMessage;

};

#endif
