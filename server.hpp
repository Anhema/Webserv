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
# include <sstream>
# include <vector>
# include <sys/event.h>
# include <sys/time.h>
typedef int fd;

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
	class serverError: public std::runtime_error {
	public:
		serverError(const string &error): std::runtime_error(error) {};
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

	void listenLoop()
	{

		kernelQueueInit();
		cout << "TEST\n";
		kernelQueueLoop();
	}

	void kernelQueueInit()
	{
		_kq_id = kqueue();

		EV_SET(_serverSocketMonitor, _socketFd, EVFILT_READ | EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0 ,0);

	}

	void kernelQueueLoop()
	{

		int new_events;
		fd	new_connection;

		int socketEvents = kevent(_kq_id, _serverSocketMonitor, 1, NULL, 1, NULL);


		if (socketEvents == -1)
			throw (serverError("Kernel event error monitoring server\n"));

		for (;;)
		{


			new_events = kevent(_kq_id, NULL, 0, _serverSocketEvents, 1, NULL);

			if (new_events == -1)
				throw (serverError("Error reading new events\n"));


			for (int i = 0; i < new_events; i++)
			{
				fd event_fd = _serverSocketEvents[i].ident;

				cout << "Event fd: " << event_fd << endl;

				if (_serverSocketEvents[i].flags & EV_EOF)
				{
					cout << "*****CLIENT (" << event_fd << ") TRIES TO DISCONNECT\n";
					// the fd should be closed BUT its funnier this, thanks 42
				}
				if (event_fd == _socketFd)
				{
					cout << "Incoming connection on fd " << event_fd << "...\n";
					new_connection = accept(_socketFd, (struct sockaddr *)&m_socketAddress, (socklen_t *)&m_socketAddress_len);

					if (new_connection == -1)
						throw(serverError("Error accepting connection\n"));

					EV_SET(_serverSocketMonitor, new_connection, EVFILT_READ, EV_ADD, 0, 0, 0);

					if (kevent(_kq_id, _serverSocketMonitor, 1, NULL, 0, NULL) == -1)
						throw (serverError("Kevent error\n"));

				}
				if (_serverSocketEvents[i].filter & EVFILT_READ)
				{
					cout << "Socket: " << event_fd << " ready to be read\n";
					// Actual read of the socket :)

					// data attribute equals to available bytes in fd
					std::size_t read_size = _serverSocketEvents[i].data;

					std::unique_ptr<char[]> buffer;
					buffer.reset(new char[read_size]);

					std::size_t read_res = read(event_fd, buffer.get(), read_size);

					cout << "READ SIZE:" << read_size << endl;
					cout << "READ RES:" << read_res << endl;
					cout << "READ BUFFER" << buffer << endl;



				}
			}

		}

	}

	void response()
	{
		std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><img src=\"path\"/<h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
		string plainText = "Hello World";
		std::ostringstream response;
		response << "HTTP/1.1 200 OK\nContent-Type: text/html\nServer: Hello\nContent-Length: " << htmlFile.size() << "\n\n"
		   << htmlFile;
		std::size_t sendBytes = write(_newSocket, response.str().c_str(), response.str().size());


		if (sendBytes != response.str().size())
		{
			cout << "=========ERROR SENDING=========\n" << "\t\tMessage size: " << htmlFile.size() << "\n" << "Sent: " << sendBytes << "\n";
		}
		else
		{
			cout << "==========RESPONSE==========\n";
			cout << response.str() << "\n";

			cout << "==========RESPONSE SEND SUCCESFULLY===============\n\n";
		}
		close(_newSocket);

//		checkResponse();

	}

private:

	std::vector<fd> activeFds;
	string _ip_address;
	int _port;
	fd	_socketFd;
	fd  _newSocket;
	int _kq_id;
	struct kevent _serverSocketMonitor[4]; // change_event
	struct kevent _serverSocketEvents[4];  // event
	struct sockaddr_in m_socketAddress;
	unsigned int m_socketAddress_len;
	string m_serverMessage;

};

#endif
