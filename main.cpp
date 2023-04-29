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
        std::cout << "Error opening configuration file\n";
    }
    file.close();
    return (result);
}

int main(int argc, char **argv)
{
    std::string conf_file;
    if (argc != 2)
    {
        std::cout << "Invalid number of arguments\n";
        return (0);
    }

    conf_file = read_file(argv[1]);
    std::cout << conf_file << "\n";
    return (0);
}
