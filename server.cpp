#include "server.hpp"

void createServer(int port)
{
	try {
		Server server("0.0.0.0", port);
		server.initSocket();
		server.ServerListener();
	}
	catch (std::exception &e) {
		cout << "Error creating socket: " << e.what() << std::endl;
		if (port < 8090)
			createServer(port + 1);
		exit(EXIT_FAILURE);
	}

}

int main(void)
{
	createServer(8080);

}
