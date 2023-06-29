#include "Config.hpp"
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

s_location::s_location():
		route(), root(), directory_file(), index(),
		directory_listing(false), accepted_methods(), redirection()
{}

s_location::s_location(t_location const &cpy):
route(cpy.route), root(cpy.root), directory_file(cpy.directory_file), index(cpy.index),
directory_listing(cpy.directory_listing), accepted_methods(cpy.accepted_methods), redirection(cpy.redirection)
{}

std::string Configuration::check_server_name(const std::string &name)
{
	// TODO
	if (name.find("www.") != 0)
		throw (Parser::BadCheck(name));

	return name;
}

void Configuration::save_server_name(const std::vector<string> &names, t_server_config &config)
{
	Configuration::save(names, config.names, Configuration::check_server_name);
}

int Configuration::check_server_ports(const std::string &port)
{
	if (!Utils::isport(port))
		throw (Parser::BadCheck(port));

	return std::atoi(port.c_str());
}

void Configuration::save_server_ports(const std::vector<string> &ports, t_server_config &config)
{
	Configuration::save(ports, config.ports, Configuration::check_server_ports);
}

std::string Configuration::check_server_ip(const std::string &ip)
{
	const std::string delimiter = ".";
	const int min_numbers = 4;
	const std::vector<string>numbers = Utils::split(ip, ".");

	if (numbers.size() != min_numbers)
		throw (Parser::BadCheck(ip));

	for (std::vector<string>::const_iterator it = numbers.begin(); it != numbers.end(); it++)
		if (std::atoi(it->c_str()) > 255)
			throw (Parser::BadCheck(ip));

	if (inet_addr(ip.c_str()) == (in_addr_t)-1)
		throw (Parser::BadCheck(ip));
	return ip;
}

void Configuration::save_server_ip(const std::vector<string> &ip, t_server_config &config)
{
	if (ip.size() != 2)
		throw (Parser::TooManyArguments("", 1, ip.size() - 1));
	Configuration::save(ip, config.ip, Configuration::check_server_ip);
}

std::string Configuration::check_server_root(const std::string &root)
{
	if (!Utils::can_open_dir(root.c_str()))
		throw (Parser::BadCheck(root));
	return root;
}

void Configuration::save_server_root(const std::vector<string> &root, t_server_config &config)
{
	if (root.size() != 2)
		throw (Parser::TooManyArguments("", 1, root.size() - 1));
	Configuration::save(root, config.root, check_server_root);
}

void Configuration::save_max_body(const std::vector<string> &root, t_server_config &config)
{
	if (root.size() != 2)
		throw (Parser::TooManyArguments("", 1, root.size() - 1));
	Configuration::save(root, config.max_body_size, Configuration::check_max_body);
}

size_t Configuration::check_max_body(const string &value)
{
	char *end;

	const size_t input = std::strtol(value.c_str(), &end, 10);

	if (strlen(end) != 1)
		throw (Parser::BadCheck(value));

	const char unit = *end;

	switch (unit)
	{
		case 'M':
			return input * 1024 * 1000;
		case 'K':
			return input * 1024;
		default:
			throw (Parser::BadCheck(value));
	}

	return 0;
}

void Configuration::save_error_page(const std::vector<string> &pages, t_server_config &config)
{
	if (pages.size() != 3)
		throw (Parser::TooManyArguments("", 2, pages.size() - 1));

	const std::string &error = pages.at(1);
	const std::string &file = pages.at(2);

	if (Utils::get_extension(file) != "html")
		throw (Parser::BadCheck(file));

	if (error == "404")
		config.errors.error_404 = file;
	else if (error == "502")
		config.errors.error_502 = file;
	else
		throw (Parser::BadCheck(error));

}

void Configuration::save_to_server(const std::string &key, const std::string &line,  const std::vector<string> &tokens, struct s_server_config &config)
{
	static std::map<string, Parser::Directive *> directives;

//	(void)config;
//	std::vector<std::string>tokens2;
//	for (std::vector<std::string>::const_iterator it = tokens.begin() + 1; it != tokens.end(); it++)
//		tokens2.push_back(*it);
	if (directives.empty())
	{
		directives["server_name"] = Parser::Directive::KeyFactory<Parser::Keys::ServerName>();
		directives["ip"] = Parser::Directive::KeyFactory<Parser::Keys::Ip>();
		directives["ports"] = Parser::Directive::KeyFactory<Parser::Keys::Ports>();
		directives["root"] = Parser::Directive::KeyFactory<Parser::Keys::Root>();
		directives["max_body"] = Parser::Directive::KeyFactory<Parser::Keys::MaxBody>();
	}
	if (directives.find(key) != directives.end())
	{
		directives[key]->handler(tokens, config);
		directives[key]->Throw(line);
	}
	else
	{
		throw (Parser::InvalidToken(line, key, 10));
	}
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

std::string get_location_path(std::string server_str)
{
	std::string path;

	size_t start = server_str.find("location");
	size_t open_key = server_str.find_first_of("{", start + 8);
	path = server_str.substr(start + 8, open_key);
	//path = trim(path);

	if (open_key < start)
	{
		std::cout << "Error in configuration file: location error\n";
	}
	//size_t open_key = server_str.find_first_not_of("{", start + 6);
	
	//(void) server_str;
	return (path);
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

	if (std::find(keywords.begin(), keywords.end(), key) == keywords.end())
		throw (Parser::InvalidToken(raw, key, 0));
	if (tokens.size() < 2)
		throw (Parser::InvalidValue(raw, key, 0));
	return tokens;
}

void Configuration::printConfig(t_server_config &config)
{
	cout << "Ip -> " << config.ip << endl;
	cout << "Ports -> ";
	print_vector(config.ports);

	cout << "Server Names -> ";
	print_vector(config.names);

	cout << "Root -> " << config.root << endl;
	cout << "Index -> " << config.index << endl;
	cout << "Accepted methods -> ";
	print_vector(config.accepted_methods);
	cout << "Max Body -> " << config.max_body_size << endl;

	cout << "Error Pages\n";
	cout << "404 -> " << config.errors.error_404 << endl;
	cout << "502 -> " << config.errors.error_502 << endl;

	if (config.locations.empty())
		cout << "No locations found\n";
	else
	{
		for (std::vector<t_location>::iterator it = config.locations.begin(); it != config.locations.end(); it++)
		{
			cout << "====Location====\n";
			cout << "\tRoute-> " << (*it).route << "\n";
			cout << "\tRoot-> " << (*it).root << "\n";
		}
	}

}

int Configuration::strport(std::string const &s)
{
	if (!Utils::isport(s) && s != "default_server;")
		throw (std::invalid_argument("invalid port"));

	return std::atoi(s.c_str());
}



void Configuration::set_default(t_server_config &data)
{
	data.accepted_methods.push_back(GET_METHOD);
	data.root = DEFAULT_ROOT_DIR;
	data.index = DEFAULT_INDEX_FILE;
	data.max_body_size = DEFAULT_MAX_BODY;

}

void Configuration::set_default(t_error_pages &data)
{
	data.error_404 = DEFAULT_404;
	data.error_502 = DEFAULT_502;
}

void Configuration::set_default(t_location &location, t_server_config const &bracket)
{
	// Dont add index
	location.root = bracket.root;
	location.accepted_methods = bracket.accepted_methods;
}

void Configuration::validateFiles(t_server_config &data)
{
	std::string root = data.root;

	if (!Utils::can_open_file(root + data.errors.error_502))
		throw (Parser::InvalidFile("", data.errors.error_502, 0));
}

void Configuration::parse_location(const std::string &bracket, const std::vector<string> &start, t_server_config &config)
{
	std::stringstream 	data(bracket);
	t_location location;
	std::string line;

	cout << "Location bracket: " << data.str() <<endl;

	set_default(location, config);
	location.route = start.at(1);
	while(std::getline(data, line) && (line.find("}") == std::string::npos))
	{
		if (line.at(0) == Configuration::s_comment_char)
			continue;
	}
	cout << "Root: " << location.root << endl;
	cout << "Route: " << location.route << endl;
	config.locations.push_back(location);
}

void Configuration::parse_bracket(const std::string &server)
{
	t_server_config		config;
	std::stringstream 	data(server);
	string				line;
	size_t 				position = 0;

	this->set_default(config);
	this->set_default(config.errors);
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
			this->parse_location(tmp.str(), tokens, config);
		}
		this->save_to_server(key, line, tokens, config);
		position += line.size();
	}
	cout << BOLDGREEN << "====INFO====\n" << NC;
	Configuration::printConfig(config);
	Configuration::validateFiles(config);
	this->configuration.push_back(config);
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
		get_location_path(tmp_server);
		parse_bracket(tmp_server);
		file.erase(0,  get_end_key(file, open_key + 1) + 1);

		//std::cout << "---------\n" << file << "\n\n------------\n";
	}
	return true;
}
