#include "Config.hpp"

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
		if (str.at(i) != '\n' && str.at(i) != ' ' && str.at(i) != '\t')
			return true;
		i++;
	}
	return false;
}

bool getConfiguration(std::string conf_file, std::vector<t_server_config> *configuration)
{
	std::string file = read_file(conf_file);

	(void)configuration;
	if (file.empty())
	{
		std::cout << "Empty configuration file\n";
		return false;
	}

	size_t start = file.find("server");
	size_t first = file.find_first_not_of("\n");
	if (first < start)
	{
		if (file.at(first) != '\n')
		{
			std::cout << "Error in configuration file\n";
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
		std::cout << "Error in configuration file 3\n";
		return false;
	}

	return true;
}
