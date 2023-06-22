#include "Logger.hpp"
#include "ServerHandler.hpp"
#include "Config.hpp"


int main(int argc, char** argv)
{
	std::vector<t_server_config> configuration;

	if (argc != 2 )
	{
		std::cout << "Invalid arguments\n";
		return (0);
	}
	if (get_extension(argv[1]) != "conf")
	{
		std::cout << "Invalid extension\n";
		return (0);
	}
	if (!getConfiguration(argv[1], &configuration))
		return (0);

	t_server_config first_server;

	first_server.names.push_back("server1");
	first_server.names.push_back("server2");

	first_server.ports.push_back(8080);
	first_server.ports.push_back(8081);
	first_server.ports.push_back(8082);

	first_server.max_body_size = 1000000;
	first_server.ip = "0.0.0.0";

	t_server_config second_server;

	second_server.names.push_back("Second Server1");
	second_server.names.push_back("Second Server2");

	second_server.ports.push_back(8090);
	second_server.ports.push_back(8091);
	second_server.ports.push_back(8092);
	second_server.ip = "0.0.0.0";

	std::vector<t_server_config> all_server_brackets;

	all_server_brackets.push_back(first_server);
	all_server_brackets.push_back(second_server);

    try
    {
        ServerHandler server(all_server_brackets);
        server.mainLoop();
    }
    catch (std::exception &e)
    {
        Logger::log(e.what(), ERROR);
    }
}
