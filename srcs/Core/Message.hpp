#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "../Utilities/Utilities.hpp"
# include <unistd.h>
# include "Config.hpp"

# define HEADER_END "\r\n\r\n"


namespace Request
{
	enum Status
	{
		HEADER,
		BODY,
        BODY_HEADER,
		FINISHED_BODY
	};
}



typedef struct s_body
{
	std::vector<char>					data;
	string 								file_name;
	string 								file_extension;
	string 								boundary;
	std::map<std::string, std::string>	headers;
}	t_body;

typedef struct s_request
{
	std::string							method;
	std::string							target;
	std::string							connection;
	std::string							uri;
	std::string							version;
	size_t 								content_length;
	std::map<std::string, std::string>	headers;
}	t_request;

typedef struct s_response
{
	std::string							htmlFile;
	std::string							extension;
}	t_response;

class Message
{

public:
	Message();
	~Message();

private:
	Data::Server			m_configuration;
	t_request				m_request;
	t_response				m_response;
	t_body 					m_body;
	HttpStatus::Code		m_responseCode;
	Request::Status			m_readStatus;
	std::string 			m_server_message;

private:
	std::string				m_get();
	std::string				m_post();
	std::string				m_delete();
	string					m_readHeader(const fd client);
	void					m_parseHeader(const string &header);
	void 					m_parseBody(const string &header);
	void 					m_readBody(const fd client, const size_t fd_size);
	void					m_createFile(const string &filename, const string &extension);

public:
	void 					reset(void);
	void					setConfig(Data::Server &config);
	void					make_response(const fd client, size_t buffer_size);
	void 					buildHeader();
	void					handle_request(const fd client, size_t buffer_size);
	string					&getConnectionType(void);

	bool					finishedReading;

	static const int		s_maxSendErrors	= 10000;
	static const int		s_maxRecvErrors	= 10000;
};

#endif

