#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "../Utilities/Utilities.hpp"
# include <unistd.h>

typedef struct s_request {
	std::string							method;
	std::string							target;
	std::string							connection;
	std::string							version;
	std::map<std::string, std::string>	headers;
	std::string							body;
}		t_request;


typedef struct s_response
{
	std::string							htmlFile;
	std::string							extension;
	int									n;
}	t_response;

class Message {

private:
	t_request	_request;
	t_response	_response;

public:
	void			response(const fd client);
	std::string		get();
	void			request(const fd client, size_t buffer_size);
	string 			_server_message;
	string			&getConnectionType(void);
};

#endif
