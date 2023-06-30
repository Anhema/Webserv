#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include "Parser.hpp"

namespace WebServ
{
	class ConfParser: public Parser::Reader
	{
	public:
		ConfParser(std::string const &file);
		~ConfParser();

		void	init();
		void	start();


	};
}
#endif

