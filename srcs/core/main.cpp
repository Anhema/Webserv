#include "Logger.hpp"
#include "Server.hpp"

int main(void)
{
	try {
		Server server("0.0.0.0", 8081);
	}
	catch (std::exception &e) {
		Logger::log(e.what(), ERROR);
	}
}
