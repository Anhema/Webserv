#include "Config.hpp"

s_server_config &s_server_config::operator=(s_server_config &rhs)
{
	this->ports = rhs.ports;
	this->names = rhs.names;
	this->ip = rhs.ip;
	return (*this);
}
