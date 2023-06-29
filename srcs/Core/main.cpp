#include "Logger.hpp"
#include "ServerHandler.hpp"
#include "Config.hpp"

int main(int argc, char** argv)
{
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
	catch (Parser::Exception &e)
	{
		e.print();
		return (0);
	}
	catch (Parser::SyntaxError &e)
	{
		cout  << e.what() << endl;
		return (0);
	}
	catch (std::exception &e)
	{
		Logger::log(e.what(), ERROR);
		return (0);
	}
}
