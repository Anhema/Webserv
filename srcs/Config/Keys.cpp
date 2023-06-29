# include "Keys.hpp"
# include <arpa/inet.h>

/*
 * SERVER NAMES DIRECTIVE
 */

Parser::Keys::ServerName::ServerName(): Directive("server_name", UNDEFINED)
{
	cout << "Directive Server name constructed\n";
}

Parser::Keys::ServerName::~ServerName()
{
	cout << "Directive Server name destructed\n";

}

void Parser::Keys::ServerName::m_format_checker(const std::vector <std::string> &tokens)
{

	if (this->m_max_tokens != UNDEFINED && static_cast<short>(tokens.size()) > this->m_max_tokens)
	{
		this->m_errno = TOO_MANY_ARGUMENTS;
	}
}

void Parser::Keys::ServerName::m_validate_token(const std::string &token)
{
	for (std::string::const_iterator it = token.begin(); it != token.end(); it++)
	{
		if (!isascii(*it))
		{
			this->m_err_token = token;
			this->m_errno = INVALID_VALUE;
			return;
		}
	}
}

void Parser::Keys::ServerName::m_save(const std::vector<std::string> &tokens, struct s_server_config &config)
{
	config.names = tokens;
}

/*
 * IP DIRECTIVE
 */

Parser::Keys::Ip::Ip(): Directive("ip", 1)
{
	cout << "Directive ip constructed\n";
}

Parser::Keys::Ip::~Ip()
{
	cout << "Directive ip destructed\n";

}

void Parser::Keys::Ip::m_format_checker(const std::vector <std::string> &tokens)
{

	if (this->m_max_tokens != UNDEFINED && static_cast<short>(tokens.size()) > this->m_max_tokens)
	{
		this->m_errno = TOO_MANY_ARGUMENTS;
	}
}

void Parser::Keys::Ip::m_validate_token(const std::string &token)
{
	if (inet_addr(token.c_str()) == (in_addr_t)-1)
	{
		this->m_errno = INVALID_VALUE;
		this->m_err_token = token;
	}
}

void Parser::Keys::Ip::m_save(const std::vector<std::string> &tokens, struct s_server_config &config)
{
	config.ip = tokens.at(1);
}

/*
 * PORTS DIRECTIVE
 */


Parser::Keys::Ports::Ports(): Directive("ports", UNDEFINED)
{
	cout << "Directive ports constructed\n";
}

Parser::Keys::Ports::~Ports()
{
	cout << "Directive ports destructed\n";

}

void Parser::Keys::Ports::m_format_checker(const std::vector <std::string> &tokens)
{

	if (this->m_max_tokens != UNDEFINED && static_cast<short>(tokens.size()) > this->m_max_tokens)
	{
		this->m_errno = TOO_MANY_ARGUMENTS;
	}
}

void Parser::Keys::Ports::m_validate_token(const std::string &token)
{
	if (!Utils::isport(token))
	{
		this->m_errno = INVALID_VALUE;
		this->m_err_token = token;
	}
}

void Parser::Keys::Ports::m_save(const std::vector<std::string> &tokens, struct s_server_config &config)
{
	for (std::vector<string>::const_iterator it = tokens.begin(); it != tokens.end(); it++)
		config.ports.push_back(std::atoi(it->c_str()));
}
/*
 * ROOT DIRECTIVE
 */

Parser::Keys::Root::Root(): Directive("root", 1)
{
	cout << "Directive root constructed\n";
}

Parser::Keys::Root::~Root()
{
	cout << "Directive Root destructed\n";

}

void Parser::Keys::Root::m_format_checker(const std::vector <std::string> &tokens)
{

	if (this->m_max_tokens != UNDEFINED && static_cast<short>(tokens.size()) > this->m_max_tokens)
	{
		this->m_errno = TOO_MANY_ARGUMENTS;
	}
}

void Parser::Keys::Root::m_validate_token(const std::string &token)
{
	if (!Utils::can_open_dir(token.c_str()))
	{
		this->m_errno = INVALID_VALUE;
		this->m_err_token = token;
	}
}

void Parser::Keys::Root::m_save(const std::vector<std::string> &tokens, struct s_server_config &config)
{
	config.root = tokens.at(1);
}

/*
 * MAX BODY DIRECTIVE
 */

Parser::Keys::MaxBody::MaxBody(): Directive("max_body", 1)
{
	cout << "Directive root constructed\n";
}

Parser::Keys::MaxBody::~MaxBody()
{
	cout << "Directive Root destructed\n";

}

void Parser::Keys::MaxBody::m_format_checker(const std::vector <std::string> &tokens)
{

	if (this->m_max_tokens != UNDEFINED && static_cast<short>(tokens.size()) > this->m_max_tokens)
	{
		this->m_errno = TOO_MANY_ARGUMENTS;
	}
}

void Parser::Keys::MaxBody::m_validate_token(const std::string &token)
{
	char *end;
	const size_t input = std::strtol(token.c_str(), &end, 10);

	if (strlen(end) != 1)
	{
		this->m_errno = INVALID_TOKEN;
		this->m_err_token = token;
		return;
	}

	const char unit = *end;


	switch (unit)
	{
		case 'M':
			this->m_max_bytes = input * 1024 * 1000;
			break;
		case 'K':
			this->m_max_bytes =  input * 1024;
			break;
		default:
			this->m_errno = INVALID_TOKEN;
			this->m_err_token = token;
	}

}

void Parser::Keys::MaxBody::m_save(const std::vector<std::string> &tokens, struct s_server_config &config)
{
	(void)tokens;
	config.max_body_size = this->m_max_bytes;
}
