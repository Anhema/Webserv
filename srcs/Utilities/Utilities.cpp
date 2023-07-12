#include "Utilities.hpp"

std::string Utils::read_file(std::string file_name)
{
	std::ifstream   file;
	std::string     result;
	std::string     line;
	std::ostringstream buffer;

	file.open(file_name.c_str());

	if (file.fail())
	{
		std::cout << "Error opening file\n";
		file.close();
		return ("");
	}


	buffer << file.rdbuf();
	result = buffer.str();
	
	file.close();
	return (result);
}

bool	Utils::can_open_dir(std::string const &directory)
{
	DIR *dir = opendir(directory.c_str());

	if (!dir)
		return false;
	else
		closedir(dir);
	return true;
}

std::string Utils::get_extension(std::string const file_name)
{
	std::string file(file_name);
	std::vector<std::string> ext = Utils::split(file, ".");
	std::vector<std::string>::iterator ite = ext.end();
	ite -= 1;

	return (*ite);
}

bool Utils::isport(const std::string &s)
{
	for (std::string::const_iterator it = s.begin(); it != s.end(); it++)
		if (!isdigit(*it))
			return false;

	return std::atoi(s.c_str()) <= USHRT_MAX;
}

bool Utils::can_open_file(const string &file)
{
	std::ifstream infile(file.c_str());

	if (infile.is_open())
	{
		infile.close();
		return true;
	}
	return false;
}

std::vector<std::string>    Utils::split(std::string str, std::string del)
{
	std::vector<std::string>    result;
	std::string                 line;

	int end = str.find(del);
	while (end != -1)
	{
		result.push_back(str.substr(0, end));
		str.erase(str.begin(), str.begin() + end + 1);
		end = str.find(del);
	}
	result.push_back(str.substr(0, end));
	return (result);
}

bool	Utils::isSet(char c, string set)
{
	for (string::iterator it = set.begin(); it != set.end(); it++)
		if (*it == c)
			return true;
	return false;
}

char *Utils::capitalize(char *s, string sep)
{
	for (size_t i = 0; i < std::strlen(s); i++)
	{
		s[i] = std::toupper(s[i]);
		while (s[++i] && !isSet(s[i], sep))
			continue;
	}
	return s;
}
void Utils::capitalize(string &s, string sep)
{
	for (string::iterator it = s.begin(); it != s.end(); it++)
	{
		if (isSet(*it, sep))
			continue;
		std::toupper(*it);
	}
}
