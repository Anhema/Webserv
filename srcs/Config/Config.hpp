#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../Utilities/Utilities.hpp"

typedef struct	s_error_pages
{

}	t_error_pages;

typedef struct	s_route_config
{
	string				route;
	string 				source_dir;
	string				directory_file;
	bool				directory_listing;
	std::vector<string>	accepted_methods;
	string				redirection;
}	t_route_config;

typedef struct s_server_config
{
	std::vector<int>			ports;
	string 						ip;
	std::vector<string>			names;
	size_t 						max_body_size;
	t_error_pages 				errorPages;
	std::vector<int>			routes;
}	t_server_config;


class Configuration
{


private:


};

bool	getConfiguration(std::string conf_file, std::vector<t_server_config> *configuration);

#endif
