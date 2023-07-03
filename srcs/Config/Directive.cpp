#include "Directive.hpp"
#include "Config.hpp"
Parser::Directive::Directive(const std::string &keyword, const short max_tokens): m_keyword(keyword), m_max_tokens(max_tokens), m_errno(NO_ERROR)
{
	//cout << "Directive Constructed\n";
}

Parser::Directive::~Directive()
{
	//cout << "Directive Destructed\n";
}


void Parser::Directive::m_iterate_through(const std::vector <std::string> &tokens)
{
	for (std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); it++)
	{
		this->m_validate_token(*it);
		if (this->m_errno)
			return;
	}
}

void Parser::Directive::handler(const std::vector <std::string> &tokens, Data::Conf *dst)
{
	this->m_format_checker(tokens);
	this->m_iterate_through(tokens);
	this->m_save(tokens, dst);
}

Parser::Errors Parser::Directive::Error() const
{
	return this->m_errno;
}

void Parser::Directive::Throw(const std::string &line)
{
	switch (this->m_errno)
	{
		case NO_ERROR:
			break;
		case TOO_MANY_ARGUMENTS:
			break;
		case INVALID_VALUE:
			throw (Parser::InvalidValue(line, this->m_err_token, 10));
		case INVALID_TOKEN:
			break;
	}
}
