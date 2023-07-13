#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "../Utilities/Utilities.hpp"
# include <unistd.h>
# include "DataContainers.hpp"

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

#define HTTP_1_1 "HTTP/1.1";

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
	std::string							body;
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
	Data::Location			*m_current_location;

private:
	std::string				m_get();
	std::string				m_post();
	std::string				m_delete();
	string					m_readHeader(const fd client);
	void					m_parseHeader(const string &header);
	void 					m_parseBody(const string &header);
	void 					m_readBody(const fd client, const size_t fd_size);
	void					m_createFile(const string &filename, const string &extension);
	void					m_update_location(const std::string &path);
	bool					m_valid_method(void);
	void					m_send_message(const fd client);
	void					m_make_redir(void);
	void					m_make_autoindex(void);
	std::string				m_make_html_link(std::string const &link);
	std::string 			m_expand_file();

public:
	void 					reset(void);
	void					setConfig(Data::Server &config);
	void					make_response(const fd client, size_t buffer_size);
	void 					buildHeader();
	void					handle_request(const fd client, size_t buffer_size);
	string					&getConnectionType(void);

	std::string				error_page(std::string error);

	bool					finishedReading;

	static const int		s_maxSendErrors	= 10000;
	static const int		s_maxRecvErrors	= 10000;
};

#endif

