#ifndef DATA_CONTAINERS_HPP
# define DATA_CONTAINERS_HPP

# include "string"
# include "vector"

namespace Data
{
	struct Server;

	struct	ErrorPages
	{
		ErrorPages();
		ErrorPages(ErrorPages const &rhs);
		~ErrorPages();

		std::string	error_404;
		std::string	error_502;

	};

	struct	Location
	{
		Location();
		Location(Data::Server const &context);
		Location(Location const &rhs);
		~Location();

		std::string							route;
		std::string 						root;
		std::string							directory_file;
		std::string 						index;
		bool								directory_listing;
		std::vector<std::string>			accepted_methods;
		std::string							redirection;
	};

	struct Server
	{
		Server();
		Server(const Data::Server &rhs);
		~Server();

		std::vector<int>					ports;
		std::vector<std::string>			names;
		std::string 						ip;
		// All this attributes have a default values.
		std::vector<std::string>			accepted_methods;
		std::string 						root;
		std::string 						index;
		size_t 								max_body_size;
		ErrorPages							errors;
		std::vector<Data::Location>			locations;
	};
}

#endif
