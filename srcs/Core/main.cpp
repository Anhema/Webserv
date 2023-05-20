#include "Logger.hpp"
#include "Server.hpp"

void createServer(int port)
{
	try {
		Server server("0.0.0.0", port);
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
