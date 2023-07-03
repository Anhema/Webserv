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
        void    save(Data::Conf *data);
        void    print() const;

        std::vector<Data::Server>	getBrackets() const;
    private:
        std::vector<Data::Server>	m_serverBrackets;
        int                       	m_serverBracket_count;
	};
}
#endif

