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
	if (Utils::get_extension(argv[1]) != "conf")
	{
		std::cout << "Invalid extension\n";
		return (0);
	}

	Configuration config;
	try
	{
		config.getConfiguration(argv[1]);
		return (0);
	}
	catch (Configuration::Exception &e)
	{
		e.print();
		return (0);
	}
	catch (Configuration::SyntaxError &e)
	{
		cout  << e.what() << endl;
		return (0);
	}
	catch (std::exception &e)
	{
		Logger::log(e.what(), ERROR);
		return (0);
	}


//	for (std::vector<t_server_config>::iterator it = configuration.begin(); it != configuration.end(); it++)
//	{
//		cout << "===============NEW-BRACKET===============\n";
//		cout << "(ip) -> " << it->ip << endl;
//	}

    try
    {
        ServerHandler server(configuration);
        server.mainLoop();
    }
    catch (std::exception &e)
    {
        Logger::log(e.what(), ERROR);
    }
}
