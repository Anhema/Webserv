#include "webserv.hpp"

int main(int argc, char **argv)
{
    std::string conf_file;

    (void) argc;
    (void) argv;
    // if (argc != 2)
    // {
    //     std::cout << "Invalid number of arguments\n";
    //     return (0);
    // }

    // conf_file = read_file(argv[1]);
    // if (conf_file == "")
    //     return (0);

    try
    {
        webserv server("0.0.0.0", 8080);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}
