#include "webserv.hpp"

webserv::webserv()
{
    std::cout << "Server empty constructor called\n";
}

webserv::webserv(std::string conf_file) : _conf_file(conf_file)
{
    this->start_server();
}

webserv::~webserv()
{
    std::cout << "Unless we got chiken\n";
    close(this->_socket);
}

void webserv::start_server()
{
    std::cout << "LEEROOOYYYY JENKIIIIINS!!!\n";
    this->_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
			throw (webserv::SocketException());
    std::cout << this->_socket << "\n";
    std::cout << this->_conf_file << "\n";
}

void webserv::debug_log(std::string log)
{
    std::cout << log << "\n";
}
