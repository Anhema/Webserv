#include "Logger.hpp"
#include "ServerHandler.hpp"
#include "Config.hpp"
#include "ConfParser.hpp"

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

	WebServ::ConfParser parser(argv[1]);

	Parser::Reader::Rules parsing_rules;

	parsing_rules.comment = '#';
	parsing_rules.bracket_closer = '}';
	parsing_rules.bracket_opener = '{';
	parsing_rules.useSemicolon = false;
	parsing_rules.key_end = "";
	parsing_rules.extension = ".conf";

	try
	{
		parser.init();
		parser.start();
		//parser.setRules(parsing_rules);
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
