#include "Utilities.hpp"

std::string read_file(std::string file_name)
{
	std::ifstream   file;
	std::string     result;
	std::string     line;
	std::ostringstream buffer;

	file.open(file_name.c_str());

	if (file.fail())
	{
		std::cout << "Error opening file\n";
	}


	buffer << file.rdbuf();
	result = buffer.str();
	
	file.close();
	return (result);
}

std::string get_extension(std::string file_name)
{
	std::vector<std::string> ext = split(file_name, ".");
	std::vector<std::string>::iterator ite = ext.end();
	ite -= 1;
	return (*ite);
}

void debug_log(std::string log)
{
	std::cout << log << "\n";
}

std::vector<std::string>    split(std::string str, std::string del)
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

bool	isSet(char c, string set)
{
	for (string::iterator it = set.begin(); it != set.end(); it++)
		if (*it == c)
			return true;
	return false;
}

char *capitalize(char *s, string sep)
{
	for (size_t i = 0; i < std::strlen(s); i++)
	{
		s[i] = std::toupper(s[i]);
		while (s[++i] && !isSet(s[i], sep))
			continue;
	}
	return s;
}
void capitalize(string &s, string sep)
{
	for (string::iterator it = s.begin(); it != s.end(); it++)
	{
		if (isSet(*it, sep))
			continue;
		std::toupper(*it);
	}
}
