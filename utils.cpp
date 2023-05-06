#include "webserv.hpp"

std::string read_file(std::string file_name)
{
    std::ifstream   file;
    std::string     result;
    std::string     line;

    file.open(file_name.c_str());
    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            result.append(line);
            result.append("\n");
        }
    }
    else
    {
        std::cout << "Error opening file\n";
    }
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
