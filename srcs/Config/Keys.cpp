# include "Keys.hpp"
# include <arpa/inet.h>

/*
 * SERVER NAMES DIRECTIVE
 */

Parser::Keys::ServerName::ServerName(): Directive("server_name", PARSER_UNDEFINED_MAX_TOKENS)
{
	//cout << "Directive Server name constructed\n";
}

Parser::Keys::ServerName::~ServerName()
{
	//cout << "Directive Server name destructed\n";

}

void Parser::Keys::ServerName::m_format_checker(const std::vector <std::string> &tokens)
{

	if (this->m_max_tokens != PARSER_UNDEFINED_MAX_TOKENS && static_cast<short>(tokens.size()) > this->m_max_tokens)
	{
		this->m_errno = TOO_MANY_ARGUMENTS;
	}
}

void Parser::Keys::ServerName::m_validate_token(const std::string &token)
{
	this->asciiCheck(token);
}

void Parser::Keys::ServerName::m_save(const std::vector<std::string> &tokens, Data::Conf *config)
{
    Data::Server *dst;

    dst = dynamic_cast<Data::Server *>(config);
    if (dst)
        dst->names = tokens;
    else
        throw (std::runtime_error("casting in ports directive"));
}

/*
 * IP DIRECTIVE
 */

Parser::Keys::Ip::Ip(): Directive("ip", 1)
{
	//cout << "Directive ip constructed\n";
}

Parser::Keys::Ip::~Ip()
{
	//cout << "Directive ip destructed\n";

}

void Parser::Keys::Ip::m_format_checker(const std::vector <std::string> &tokens)
{
	this->std_max_tokens_check(tokens);
}

void Parser::Keys::Ip::m_validate_token(const std::string &token)
{
	if (inet_addr(token.c_str()) == (in_addr_t)-1)
	{
		this->m_errno = INVALID_VALUE;
		this->m_err_token = token;
	}
}

void Parser::Keys::Ip::m_save(const std::vector<std::string> &tokens, Data::Conf *config)
{
    Data::Server *dst;

    dst = dynamic_cast<Data::Server *>(config);
    if (dst)
        dst->ip = tokens.at(0);
    else
        throw (std::runtime_error("casting in ports directive"));
}

/*
 * PORTS DIRECTIVE
 */


Parser::Keys::Ports::Ports(): Directive("ports", PARSER_UNDEFINED_MAX_TOKENS)
{
	//cout << "Directive ports constructed\n";
}

Parser::Keys::Ports::~Ports()
{
	//cout << "Directive ports destructed\n";

}

void Parser::Keys::Ports::m_format_checker(const std::vector <std::string> &tokens)
{
	this->std_max_tokens_check(tokens);
}

void Parser::Keys::Ports::m_validate_token(const std::string &token)
{
	if (!Utils::isport(token))
	{
		this->m_errno = INVALID_VALUE;
		this->m_err_token = token;
	}
}

void Parser::Keys::Ports::m_save(const std::vector<std::string> &tokens, Data::Conf *config)
{
    Data::Server *dst;

    dst = dynamic_cast<Data::Server *>(config);
    if (dst)
        for (std::vector<string>::const_iterator it = tokens.begin(); it != tokens.end(); it++)
            dst->ports.push_back(std::atoi(it->c_str()));
    else
        throw (std::runtime_error("casting in ports directive"));
}
/*
 * ROOT DIRECTIVE
 */

Parser::Keys::Root::Root(): Directive("root", 1)
{
	//cout << "Directive root constructed\n";
}

Parser::Keys::Root::~Root()
{
	//cout << "Directive Root destructed\n";

}

void Parser::Keys::Root::m_format_checker(const std::vector <std::string> &tokens)
{
	this->std_max_tokens_check(tokens);
}

void Parser::Keys::Root::m_validate_token(const std::string &token)
{
	if (!Utils::can_open_dir(token.c_str()))
	{
		this->m_errno = INVALID_VALUE;
		this->m_err_token = token;
	}
}

void Parser::Keys::Root::m_save(const std::vector<std::string> &tokens, Data::Conf *config)
{
    {
        Data::Server *dst;

        dst = dynamic_cast<Data::Server *>(config);
        if (dst)
        {
            dst->root = tokens.at(0);
            return;
        }
    }
    {
        Data::Location *dst;

        dst = dynamic_cast<Data::Location *>(config);
        if (dst)
        {
            dst->root = tokens.at(0);
            return;
        }
    }
    throw (std::runtime_error("casting root directive"));
}

/*
 * MAX BODY DIRECTIVE
 */

Parser::Keys::MaxBody::MaxBody(): Directive("max_body", 1), m_max_bytes(0)
{
	//cout << "Directive root constructed\n";
}

Parser::Keys::MaxBody::~MaxBody()
{
	//cout << "Directive Root destructed\n";

}

void Parser::Keys::MaxBody::m_format_checker(const std::vector <std::string> &tokens)
{
	this->std_max_tokens_check(tokens);
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

void Parser::Keys::MaxBody::m_save(const std::vector<std::string> &tokens, Data::Conf *config)
{
	(void)tokens;
    Data::Server *dst;

    dst = dynamic_cast<Data::Server *>(config);
    if (dst)
        dst->max_body_size = this->m_max_bytes;
    else
        std::runtime_error("casting in max_body directive");
}

Parser::Keys::LocationPath::LocationPath(): Parser::Directive("path", 1) {

}

Parser::Keys::LocationPath::~LocationPath() {

}

void Parser::Keys::LocationPath::m_format_checker(const std::vector<std::string> &tokens)
{
	this->std_max_tokens_check(tokens);
}

void Parser::Keys::LocationPath::m_validate_token(const string &token) {
	this->asciiCheck(token);
}

void Parser::Keys::LocationPath::m_save(const std::vector<std::string> &tokens, Data::Conf *config)
{
	if (Data::Location *dst = dynamic_cast<Data::Location *>(config))
		dst->route = tokens.at(0);
	else
		std::runtime_error("casting in LocationPath directive");
}

Parser::Keys::Index::Index(): Parser::Directive("index", 1) {

}

Parser::Keys::Index::~Index() {

}

void Parser::Keys::Index::m_validate_token(const string &token) {
	this->asciiCheck(token);
	if (Utils::get_extension(token) != "html")
	{
		this->m_errno = INVALID_VALUE;
		this->m_err_token = token;
		return;
	}
}

void Parser::Keys::Index::m_format_checker(const std::vector<std::string> &tokens) {
	this->std_max_tokens_check(tokens);
}

void Parser::Keys::Index::m_save(const std::vector<std::string> &tokens, Data::Conf *config) {

	if (Data::Location *dst = dynamic_cast<Data::Location *>(config))
		dst->index = tokens.at(0);
	else
		std::runtime_error("casting in index directive");
}

const std::vector<std::string> &Parser::Keys::AcceptMethod::get_valid_methods() {
	static std::vector<std::string> valid_methods;

	if (valid_methods.empty())
	{
		valid_methods.push_back(GET_METHOD);
		valid_methods.push_back(POST_METHOD);
		valid_methods.push_back(DELETE_METHOD);
	}
	return valid_methods;
}

bool Parser::Keys::AcceptMethod::is_in_methods(string const &method) {
	return (std::find(this->get_valid_methods().begin(), this->get_valid_methods().end(), method) != this->get_valid_methods().end());
}

Parser::Keys::AcceptMethod::AcceptMethod(): Parser::Directive("method", 1) {

}

Parser::Keys::AcceptMethod::~AcceptMethod() {

}

void Parser::Keys::AcceptMethod::m_format_checker(const std::vector<std::string> &tokens) {
	this->std_max_tokens_check(tokens);
}

void Parser::Keys::AcceptMethod::m_validate_token(const string &token) {
	if (!is_in_methods(token))
	{
		this->m_errno = INVALID_VALUE;
		this->m_err_token = token;
		return;
	}
}

void Parser::Keys::AcceptMethod::m_save(const std::vector<std::string> &tokens, Data::Conf *config)
{
	if (Data::Accept *dst = dynamic_cast<Data::Accept *>(config))
		dst->methods.push_back(tokens.at(0));
	else
		std::runtime_error("casting in index directive");
}
