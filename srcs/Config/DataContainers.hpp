#ifndef DATA_CONTAINERS_HPP
# define DATA_CONTAINERS_HPP

# include "string"
# include "vector"

typedef struct	s_error_pages
{
	std::string	error_404;
	std::string	error_502;

}	t_error_pages;

typedef struct	s_location
{
	s_location(s_location const &cpy);
	s_location();
	string						route;
	string 						root;
	string						directory_file;
	string 						index;
	bool						directory_listing;
	std::vector<string>			accepted_methods;
	string						redirection;
}	t_location;

typedef struct s_server_config
{
	std::vector<int>			ports;
	std::vector<string>			names;
	string 						ip;
	// All this attributes have a default values.
	std::vector<string>			accepted_methods;
	string 						root;
	string 						index;
	size_t 						max_body_size;
	t_error_pages 				errors;
	std::vector<t_location>		locations;
}	t_server_config;

#endif
