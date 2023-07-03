#include "Config.hpp"
#include "stdlib.h"
Parser::KeysNotClosed::KeysNotClosed(): std::invalid_argument("Keys are not closed correctly") {}

Parser::SyntaxError::SyntaxError(): std::invalid_argument("Syntax Error") {}

Parser::SyntaxError::SyntaxError(const string &line): std::invalid_argument(("Error at line: " + line).c_str()) {}

Parser::InvalidValue::InvalidValue(const std::string &line, const std::string &token, int n): Exception(line, token, n) {}

Parser::BadCheck::BadCheck(const std::string &bad_token): token(bad_token) {}

Parser::TooManyArguments::TooManyArguments(const std::string &line, const int expected, const int have): Exception(line, line, expected)
{
	this->m_expected = expected;
	this->m_have = have;
}

Parser::InvalidFile::InvalidFile(const std::string &line, const std::string &file, int n): Exception(line, file, n) {}

void Parser::InvalidFile::print()
{
	cout	<< BOLDRED << "[ERROR]" << NC << " "
			<< "can't open file " << RED "-> " << NC << this->m_token << endl;

}

void Parser::TooManyArguments::print()
{
	cout	<< BOLDRED << "[ERROR]" << NC << " "
			<< "in line " << RED "-> " << NC << "(" << &this->m_line.at(this->m_line.find_first_not_of(' ')) << ") "
			<< "Too many arguments, expected " << this->m_expected << " have " << this->m_have << endl;

}

void Parser::InvalidToken::print()
{
	(void) this->n;
	cout	<< BOLDRED << "[ERROR]" << NC << " "
			<< "in line " << RED "-> " << NC << "(" << &this->m_line.at(this->m_line.find_first_not_of(' ')) << ") "
			<< "Invalid token: \"" << BOLDRED << this->m_token << NC << "\"\n";

}

Parser::Exception::Exception(const std::string &line, const std::string &token, int n):
m_line(line), m_token(token), n(n), message("parsing error") {}

Parser::InvalidToken::InvalidToken(const std::string &line, const std::string &token, int n): Exception(line, token, n) {}

void Parser::InvalidValue::print()
{
	cout	<< BOLDRED << "[ERROR]" << NC << " "
			<< "in line " << RED "-> " << NC << "(" << &this->m_line.at(this->m_line.find_first_not_of(' ')) << ") "
			<< "Invalid value: \"" << BOLDRED << this->m_token << NC << "\"\n";
}




const std::vector<string> &Configuration::Keywords() {
	static std::vector<string>	keywords;

	if (keywords.empty())
	{
		keywords.push_back("ip");
		keywords.push_back("ports");
		keywords.push_back("server_name");
		keywords.push_back("root");
		keywords.push_back("max_body");
		keywords.push_back("index");
		keywords.push_back("error");
		keywords.push_back("location");
	}
	return keywords;
}

const std::vector<string> &Configuration::Accepted_errors() {
	static std::vector<string>	keywords;

	if (keywords.empty())
	{
		keywords.push_back("404");
		keywords.push_back("502");
	}
	return keywords;
}

const std::vector<string> &Configuration::Location_keywords() {
	static std::vector<string>	keywords;

	if (keywords.empty())
	{
		keywords.push_back("listen");
		keywords.push_back("root");
		keywords.push_back("index");
	}
	return keywords;
}

size_t get_end_key(std::string str, size_t start)
{
	int		n = 1;
	size_t	i = start;

	while (i < str.size())
	{
		if (str.at(i) == '{')
			n++;
		if (str.at(i) == '}')
			n--;
		if (n == 0)
			return (i);
		i++;
	}
	return (0);
}

bool something_between_positions(std::string str, size_t start, size_t end)
{
	size_t	i = start;

	while (i < end)
	{
		if (str.at(i) != '\n' && str.at(i) != ' ' && str.at(i) != '\t' && str.at(i) != '\r' && str.at(i) != '\f' && str.at(i) != '\v')
			return true;
		i++;
	}
	return false;
}

bool remaining_file(std::string str)
{
	size_t	i = 0;

	while (i < str.size())
	{
		if (str.at(i) != '\n' && str.at(i) != ' ' && str.at(i) != '\t' && str.at(i) != '\f' && str.at(i) != '\v')
			return true;
		i++;
	}
	return false;
}


std::vector<string> Configuration::tokenize(const std::string &raw)
{
	std::stringstream	line(raw);
	std::vector<string>	tokens;
	string 				tmp;

	while (line >> tmp)
		tokens.push_back(tmp);
	return tokens;
}

std::vector<string> Configuration::parse_line(const std::string &raw, const std::vector<string> &keywords)
{
	cout << "line -> " << raw << endl;
	std::vector<string> tokens = tokenize(raw);
	const string key = tokens.at(0);
    (void)keywords;

//	if (std::find(keywords.begin(), keywords.end(), key) == keywords.end())
//		throw (Parser::InvalidToken(raw, key, 0));
//	if (tokens.size() < 2)
//		throw (Parser::InvalidValue(raw, key, 0));
	return tokens;
}

void Configuration::printConfig(Data::Server &config)
{
	cout << "Ip -> " << config.ip << endl;
	cout << "Ports -> ";
	Utils::print_vector(config.ports);

	cout << "Server Names -> ";
	Utils::print_vector(config.names);

	cout << "Root -> " << config.root << endl;
	cout << "Index -> " << config.index << endl;
	cout << "Accepted methods -> ";
	cout << "Max Body -> " << config.max_body_size << endl;

	cout << "Error Pages\n";
	cout << "404 -> " << config.errors.error_404 << endl;
	cout << "502 -> " << config.errors.error_502 << endl;

}

int Configuration::strport(std::string const &s)
{
	if (!Utils::isport(s) && s != "default_server;")
		throw (std::invalid_argument("invalid port"));

	return std::atoi(s.c_str());
}



void Configuration::set_default(Data::Server &data)
{
	data.root = DEFAULT_ROOT_DIR;
	data.index = DEFAULT_INDEX_FILE;
	data.max_body_size = DEFAULT_MAX_BODY;

}

void Configuration::set_default(Data::ErrorPages &data)
{
	data.error_404 = DEFAULT_404;
	data.error_502 = DEFAULT_502;
}

void Configuration::set_default(Data::Location &location, Data::Server const &bracket)
{
	// Dont add index
	location.root = bracket.root;
	location.accepted_methods = bracket.accepted_methods;
}

void Configuration::validateFiles(Data::Server &data)
{
	std::string root = data.root;

	if (!Utils::can_open_file(root + data.errors.error_502))
		throw (Parser::InvalidFile("", data.errors.error_502, 0));
}

void Configuration::parse_bracket(const std::string &server)
{
	Data::Server		config;
	std::stringstream 	data(server);
	string				line;
	size_t 				position = 0;

	this->set_default(config);
	cout << "====New Bracket====\n";
	while(std::getline(data, line) )
	{
		if (line.at(0) == Configuration::s_comment_char)
			continue;
		const std::vector<string>	tokens = parse_line(line, Keywords());
		const string 				&key = tokens.at(0);


		if (key == "location")
		{
			stringstream tmp;
			tmp << data.seekp(position).rdbuf();
		}
		position += line.size();
	}
	cout << BOLDGREEN << "====INFO====\n" << NC;
	Configuration::printConfig(config);
//	this->configuration.push_back(config);
	cout << BOLDGREEN << "====END====\n" << NC;


}


bool Configuration::getConfiguration(std::string conf_file)
{
	std::string file = Utils::read_file(conf_file);
	std::vector<string> servers;

	if (file.empty())
	{
		std::cout << "Empty configuration file: file empty\n";
		return false;
	}

	while (remaining_file(file))
	{
		size_t start = file.find("server");
		size_t first = file.find_first_not_of("\n");
		if (first < start)
		{
			if (file.at(first) != '\n')
			{
				std::cout << "Error in configuration file: server not found\n";
				return false;
			}
		}

		size_t open_key = file.find_first_of("{", start + 6);
		if (something_between_positions(file, start + 6, open_key))
		{
			std::cout << "Error in configuration file 2\n";
			return false;
		}

		if (get_end_key(file, open_key + 1) == 0)
		{
			throw (Parser::KeysNotClosed());
		}

		std::string tmp_server = file.substr(open_key + 2, (get_end_key(file, open_key + 1) - open_key) - 2);
//		std::cout << "\n---------\n" << tmp_server << "\n---------\n";
		servers.push_back(tmp_server);

		parse_bracket(tmp_server);
		file.erase(0,  get_end_key(file, open_key + 1) + 1);

		//std::cout << "---------\n" << file << "\n\n------------\n";
	}
	return true;
}
