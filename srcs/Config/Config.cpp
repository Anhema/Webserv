#include "Config.hpp"

// std::string trim(const std::string &s)
// {
//     auto start = s.begin();
//     while (start != s.end() && std::isspace(*start)) {
//         start++;
//     }
 
//     auto end = s.end();
//     do {
//         end--;
//     } while (std::distance(start, end) > 0 && std::isspace(*end));
 
//     return std::string(start, end + 1);
// }
 

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

bool reamining_file(std::string str)
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

t_server_config create_server(std::string server_str)
{
	t_server_config newServer;

	std::vector<string> rules = Utils::split(server_str, ";");

	return (newServer);
}

bool getConfiguration(std::string conf_file, std::vector<t_server_config> *configuration)
{
	std::string file = Utils::read_file(conf_file);
	std::vector<string> servers;

	(void)configuration;
	if (file.empty())
	{
		std::cout << "Empty configuration file: file empty\n";
		return false;
	}

	while (reamining_file(file))
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
			std::cout << "Error in configuration file: all keys must be closed\n";
			return false;
		}

		std::string tmp_server = file.substr(open_key + 1, (get_end_key(file, open_key + 1) - open_key) - 1);
		std::cout << "\n---------\n" << tmp_server << "\n---------\n";
		servers.push_back(tmp_server);
		get_location_path(tmp_server);
		file.erase(0,  get_end_key(file, open_key + 1) + 1);

		//std::cout << "---------\n" << file << "\n\n------------\n";
	}



	return true;
}
