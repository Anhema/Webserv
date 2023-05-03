#include "server.hpp"

int main(void)
{
	try {
		Server server("0.0.0.0", 8081);
		server.initSocket();
		server.ServerListener();
	}
	catch (std::exception &e) {
		cout << "Error creating socket: " << e.what() << std::endl;
		return (EXIT_FAILURE);
	}


}
