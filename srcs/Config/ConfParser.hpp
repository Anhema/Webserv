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
		void	validate_bracekt(Data::Server &bracket);
		void	validate_bracekt(Data::Location &bracket);

        std::vector<Data::Server>	getBrackets() const;
    private:
        std::vector<Data::Server>	m_serverBrackets;
        int                       	m_serverBracket_count;
		int                       	m_locationBracket_count;
	};
}
#endif

