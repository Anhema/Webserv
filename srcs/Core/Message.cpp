#include "Message.hpp"
#include "Logger.hpp"
#include <sys/socket.h>
#include "Utilities.hpp"
#include <fcntl.h>

Message::Message(): m_responseCode(HttpStatus::OK), m_readStatus(Request::HEADER), finishedReading(false)
{
}

Message::~Message() {}

string &Message::getConnectionType() { return this->m_request.connection; }

void Message::setConfig(Data::Server &config)
{
	this->m_configuration = config;
}

void Message::buildHeader()
{

}

void Message::m_createFile(const std::string &filename, const std::string &extension)
{
	char			time_str[100];
	time_t 			t;
	struct tm		*time_ptr;

	t = time(NULL);
	time_ptr = localtime(&t);
	strftime(time_str, sizeof(time_str), "%d-%m-%y_%H-%M", time_ptr);

	string composition_name;

	composition_name.append("uploads/");
	composition_name.append(filename);
	composition_name.append(time_str);
	composition_name.append(".");
	composition_name.append(extension);

	std::ofstream outfile(composition_name, std::ios::out | std::ios::binary);
	if (outfile.fail())
		throw (std::runtime_error("can't create outfile for POST"));

	string			needle;

	needle.append("\r\n");
	needle.append(this->m_body.boundary);

	std::vector<char>::iterator data_end = this->m_body.data.end() - 1;
	size_t i = 0;

	while (data_end != this->m_body.data.begin())
	{
		if (i == needle.size())
			break;
		if (*data_end == needle.at(needle.size() - 1 - i))
			i++;
		data_end--;
	}

	for (std::vector<char>::iterator it = this->m_body.data.begin(); it != data_end; it++)
		outfile.write(&*it, 1);
}

std::string Message::error_page(std::string error)
{
	std::ostringstream	message;
	std::string			error_name;
	std::string 		path;

	if (error == "400")
	{
		path = this->m_configuration.errors.error_400;
		error_name = "Bad Request";
	}
	else if (error == "403")
	{

		path = this->m_configuration.errors.error_403;
		error_name = "Forbidden";
	}
	else if (error == "404")
	{
		error_name = "Not Found";
		path = this->m_configuration.errors.error_404;

	}
	else if (error == "405")
	{
		path = this->m_configuration.errors.error_405;
		error_name = "Method Not Allowed";
	}
	else if (error == "500")
	{
		path = this->m_configuration.errors.error_500;
		error_name = "Internal Server Error";
	}
	else if (error == "501")
	{
		path = this->m_configuration.errors.error_501;
		error_name = "Not Implemened";
	}
	else if (error == "502")
	{
		path = this->m_configuration.errors.error_502;
		error_name = "Bad Gateway";
	}
	else if (error == "505")
	{
		path = this->m_configuration.errors.error_505;
		error_name = "Not Implemented";
	}

	cout << "Manda la pagina de error de: " << this->m_configuration.root + path << endl;
	this->m_response.htmlFile = Utils::read_file(this->m_configuration.root + path);

	if (this->m_response.htmlFile.empty())
	{
		cout << "llega aqui\n";
		message << "HTTP/1.1 200 OK\r\n Content-Type: text/plain\r\nContent-Length: 3\r\n\r\n" << "404";
	}
	else
	{
		this->m_response.extension = "html";
		message << "HTTP/1.1 "<< error << " " << error_name << "\r\nContent-Status: text/" << this->m_response.extension
				<< "\r\nContent-Length: " << this->m_response.htmlFile.size() << "\r\n\r\n" << this->m_response.htmlFile;
	}

	this->m_server_message = message.str();
	return (message.str());

}

void Message::m_update_location(const string &path)
{


	static Data::Location default_location;


	for (std::vector<Data::Location>::iterator it = this->m_configuration.locations.begin(); it != this->m_configuration.locations.end(); it++)
	{
		if (it->route == path)
		{
			this->m_current_location = &(*it);
			return;
		}
	}
	default_location.route = path;
	this->m_current_location = &default_location;
}

std::string Message::m_get()
{
	std::ostringstream	message;
	std::string 		path;

	path.append(this->m_current_location->root);

	if (*(this->m_request.uri.end() - 1) == '/' && !this->m_current_location->index.empty())
		path.append(this->m_current_location->index);
	else
		path.append(this->m_request.uri);

	std::ifstream ifs(path);

	if (access(path.c_str(), F_OK) != 0)
		message << this->error_page("404");
	else if (access(path.c_str(), R_OK) != 0)
		message << this->error_page("403");
	else if (!ifs.is_open())
		message << this->error_page("400");
	else
	{
		this->m_response.htmlFile = Utils::read_file(path);
		this->m_response.extension = Utils::get_extension(path);
		message << "HTTP/1.1 "<< this->m_responseCode << " OK\nContent-Status:";
		if (this->m_response.extension == "png" || this->m_response.extension == "jpg")
			message << "image/";
		else
			message << "text/";
		message << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\r\n\r\n" << this->m_response.htmlFile;
	}
	return (message.str());
}

std::string Message::m_post()
{
	std::ostringstream message;
	std::string path = "www";

	//path.append(this->m_request.uri);
	path.append("/test.py");

	std::ifstream ifs(path, std::ios::binary);

	if (access(path.c_str(), F_OK) != 0)
		message << this->error_page("404");
	else if (access(path.c_str(), X_OK) != 0)
		message << this->error_page("403");
	else if (!ifs.is_open())
		message << this->error_page("400");
	else
	{
		int pipefd[2];
		if (pipe(pipefd) == -1)
		{
			std::cout << "ERROR\n";
			return ("");
		}

		std::string interpreter = "/usr/bin/python3";
		std::string script = "www/test.py";

		char* args[] = {&interpreter[0], &script[0], NULL};
		char* env[] = {NULL};

		pid_t pid = fork();
		if (pid == 0)
		{
			close(pipefd[0]);
			dup2(pipefd[1], 1);
			dup2(pipefd[1], 2);
			close(pipefd[1]);
			execve(args[0], args, env);
			std::cerr << "Error al ejecutar el script CGI." << std::endl;
			exit (1);
		}
		else if (pid > 0)
		{
			close(pipefd[1]);
			char buffer[1024];
			ssize_t bytesRead;
			string output = "";
			while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) != 0)
			{
				buffer[bytesRead] = '\0';
				output.append(buffer);
			}
			string out = "<!DOCTYPE html>\n<html>\n<head>\n<title>Bean Emporium</title>\n<meta charset='UTF-8'>\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n</head>\n<body>\n";
			out.append(output);
			out.append("</body>\n</html>\n");
			close(pipefd[0]);

			std::cout << output << "<------\n";
			this->m_response.body = out;
		}
		else
			//CGI Error
			this->m_response.extension = "text/plain";
		message << "HTTP/1.1 200 OK\nContent-Status: text/html\n"
				<< "Content-Length: " << this->m_response.body.size() << "\r\n\r\n" << this->m_response.body;
	}
	return (message.str());
}

std::string Message::m_delete()
{
	std::ostringstream message;
	std::string path = "www";
	if (this->m_request.uri == "/")
		this->m_response.htmlFile = Utils::read_file(path.append("/index.html"));
	else
		path.append(this->m_request.uri);
	std::ifstream ifs(path);

	if (access(path.c_str(), R_OK) != 0)
		message << this->error_page("403");
	else if (access(path.c_str(), F_OK) != 0)
		message << this->error_page("404");
	else if (!ifs.is_open())
		message << this->error_page("400");
	else
	{
		this->m_response.htmlFile = Utils::read_file(path);
		std::remove(path.c_str());
		this->m_response.extension = Utils::get_extension(path);
		message << "HTTP/1.1 200 OK\nContent-Status: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\r\n\r\n" << this->m_response.htmlFile;
	}
	return (message.str());
}

void print_headers(std::map<string, string> headers, t_request &request)
{
	cout << "Target -> " << request.target << endl;
	cout << "Method -> " << request.method << endl;
	for (std::map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		cout << "(" << it->first << ") -> (" << it->second << ")\n";
	}

}

string Message::m_readHeader(const fd client)
{
	string 			header;
	ssize_t			read_bytes = 0;
	size_t			err_count = 0;
	char			buffer;

	while (header.find(HEADER_END) == string::npos)
	{
		read_bytes = recv(client, &buffer, 1, 0);
		header.push_back(buffer);
		if (read_bytes == -1)
		{
			err_count++;
			continue;
		}
		if (err_count >= Message::s_maxRecvErrors)
		{
			header.clear();
			return header;
		}
	}
	return header;
}

void Message::m_parseHeader(const std::string &header)
{
	if (header.empty())
		throw (std::runtime_error("bad header parsing"));

	std::vector<std::string> request = Utils::split(header, "\n");
	std::vector<std::string>::iterator line = request.begin();
	std::vector<std::string> r_line = Utils::split((*line), " ");

	while (++line != request.end())
	{
		char *tmp_key = std::strtok((char *)line->c_str(), ": ");
		if (!tmp_key)
			break;

		Utils::capitalize(tmp_key, " -");

		const string key(tmp_key);

		char *tmp_value = std::strtok(NULL, "\n\r");
		if (!tmp_value)
			break;

		const string value(&tmp_value[1]);

		this->m_request.headers.insert(std::make_pair(key, value));
	}

	this->m_request.method = r_line[0];
	this->m_request.uri = r_line[1];
	this->m_request.version = r_line[2];
	this->m_request.connection = this->m_request.headers["Connection"];
	this->m_request.content_length = std::atol(this->m_request.headers["Content-Length"].c_str());

	if (this->m_request.content_length)
	{
		Logger::log("READ STATUS -> BODY HEADER", INFO);
		this->m_readStatus = Request::BODY_HEADER;
	}
	else
	{
		Logger::log("Finished read -> true", INFO);
		this->finishedReading = true;
	}
}

void Message::m_parseBody(const std::string &header)
{
	if (header.empty())
		throw (std::runtime_error("bad header parsing"));


	string  filename;
	char    *tmp;

	this->m_body.boundary = header.substr(0, header.find("\r\n"));

	strtok((char *)&header.c_str()[header.find("filename=")], "\"=");
	tmp = strtok(NULL, ".");

	if (tmp)
		this->m_body.file_name = &tmp[1];

	tmp = strtok(NULL, "\"");

	this->m_body.file_extension = tmp;
	this->m_request.content_length -= header.size();
}

void Message::m_readBody(const fd client, const size_t fd_size)
{
	static size_t	totalRead;
	size_t 			read_errors = 0;
	size_t 			loopRead = 0;
	ssize_t 		read_bytes;
	const size_t 	buffer_size = 30720;
	char		    buffer[buffer_size];

	while (loopRead < fd_size && totalRead < this->m_request.content_length)
	{
		read_bytes = recv(client, buffer, buffer_size, 0);

		if (read_errors >= Message::s_maxRecvErrors)
		{
			Logger::log("timeout reading handle_request", WARNING);
			break;
		}
		if (read_bytes == -1)
		{
			read_errors++;
			continue;
		}
		else
			read_errors = 0;

		for (ssize_t i = 0; i < read_bytes; i++)
			this->m_body.data.push_back(buffer[i]);

		totalRead += read_bytes;
		loopRead += read_bytes;

		cout << "Read: " << totalRead << "/" << this->m_request.content_length << endl;
	}
	if (totalRead >= this->m_request.content_length || this->m_request.content_length == 0)
	{
		Logger::log("READ STATUS -> FINISHED BODY", INFO);
		this->m_readStatus = Request::FINISHED_BODY;
		this->finishedReading = true;
		totalRead = 0;
	}
}

void Message::handle_request(const fd client, size_t buffer_size)
{

	stringstream  	ss;
	stringstream  	ss_buffer;

	ss << "Request fd: " << client << " size: " << buffer_size;
	Logger::log(ss.str(), INFO);
	cout << "****Reading****" << endl;

	switch (this->m_readStatus)
	{
		case Request::HEADER:
			this->m_parseHeader(this->m_readHeader(client));
//			print_headers(this->m_request.headers, this->m_request);
			break;
		case Request::BODY_HEADER:
			this->m_parseBody(this->m_readHeader(client));
			this->m_readStatus = Request::BODY;
			break;
		case Request::BODY:
			this->m_readBody(client, buffer_size);
			break;
		case Request::FINISHED_BODY:
			break;
	}

	if (this->m_readStatus == Request::FINISHED_BODY)
		this->m_createFile(this->m_body.file_name, this->m_body.file_extension);
}

bool Message::m_valid_method()
{
	for (std::vector<std::string>::iterator it = this->m_current_location->accepted_methods.methods.begin(); it != this->m_current_location->accepted_methods.methods.end(); it++)
		if (*it == this->m_request.method)
			return true;
	return false;
}

void Message::m_send_message(const fd client)
{
	ssize_t totalSent = 0;
	ssize_t send_bytes = 0;
	size_t err_count = 0;

	while (totalSent < (ssize_t)this->m_server_message.size())
	{
		send_bytes = send(client, this->m_server_message.c_str() + totalSent, this->m_server_message.size() - totalSent, 0);
		if (err_count >= Message::s_maxSendErrors)
		{
			Logger::log("timeout sending handle_request", WARNING);
			break;
		}
		if (send_bytes == -1)
		{
			err_count++;
			continue;
		}
		else
			err_count = 0;
		totalSent += send_bytes;
	}

	if (totalSent == (long)this->m_server_message.size())
	{
		Logger::log("SERVER RESPONSE SENT TO CLIENT", INFO);
	}
	else
	{
		Logger::log("SENDING RESPONSE TO CLIENT", ERROR);
		cout << "Sent: " << send_bytes << " Expected: " << m_server_message.size() << endl;
	}

	this->m_request.headers.clear();
}

void Message::m_make_redir(void)
{
	std::stringstream message;
	const std::string EOL("\r\n");

	cout << "***MAKING REDIR TO -> " << this->m_current_location->redirection << endl;

	message << "HTTP/1.1" << " " << "301" << " " << "Moved Permanently" << EOL
	<< "Location: " << this->m_current_location->redirection << EOL
	<< "Connection: close" << EOL
	<< "Content-Length: 0" << EOL
	<< EOL;
	this->m_server_message = message.str();
}

std::string Message::m_make_html_link(const string &link)
{
	std::string result;

	result.append("<li>");
	result.append("<a href=\"");
	result.append(link);
	result.append("\">");
	result.append(link);
	result.append("</a>");
	result.append("</li>");
	return result;
}

void Message::m_make_autoindex(void)
{
	const std::string path(this->m_current_location->root.substr(0, this->m_current_location->root.size() -1) + this->m_request.uri);
	const std::string EOL("\r\n");
	std::stringstream message;
	std::stringstream html;

	cout << "autoindex path: " << path << endl;

	html	<< "<html>\n<head>\n\t<title>Directory Links</title>\n</head>\n<body>\n\t"
			<< "<h1>Directory Links</h1>\n\t<ul>\n";

	std::vector<std::string> dir_entries = Utils::read_dir(path);

	for (std::vector<std::string>::iterator it = dir_entries.begin(); it != dir_entries.end(); it++)
		html << "\t\t" << this->m_make_html_link(*it) << endl;

	html << "\t</ul>\n"
			<< "</body>\n</html>"
			<< "\n";
			cout << "html:\n" << html.str() << endl;

	message << "HTTP/1.1 200 OK" << EOL
			<< "Content-Type: text/html" << EOL
			<< "Content-Length: " << html.str().size() << EOL
			<< "Connection: close" << EOL
			<< EOL
			<< html.str();
	this->m_server_message = message.str();
}

void Message::make_response(const fd client, size_t __unused buffer_size)
{
	stringstream  ss;

	ss << "Responding fd: " << client;
	Logger::log(ss.str(), INFO);
	cout << "****Writing****" << endl;
	this->m_update_location(this->m_request.uri.substr(0, this->m_request.uri.find_last_of('/') + 1));

	cout << "Using location:\n" << *this->m_current_location << endl;

	if (!this->m_valid_method())
	{
		this->error_page("405");
		this->m_send_message(client);
		return;
	}
	if (!this->m_current_location->redirection.empty())
	{
		this->m_make_redir();
		this->m_send_message(client);
		return;
	}
	if (*(this->m_request.uri.end() -1) == '/' && this->m_current_location->autoindex && this->m_current_location->index.empty())
	{
		this->m_make_autoindex();
		this->m_send_message(client);
		return;
	}
	else if (*(this->m_request.uri.end() -1) == '/' && !this->m_current_location->autoindex && this->m_current_location->index.empty())
	{
		this->error_page("404");
		this->m_send_message(client);
	}

	std::ostringstream message;
	if (this->m_request.method == GET_METHOD)
		this->m_server_message = m_get();
	else if (this->m_request.method == DELETE_METHOD)
		this->m_server_message = this->m_delete();
	else if (this->m_request.method == POST_METHOD)
		this->m_server_message = this->m_post();
	else
		this->error_page("405");

	this->m_send_message(client);
}

void Message::reset()
{
	Logger::log("Resetting message class", INFO);
	this->m_body.data.clear();
	this->m_request.method.clear();
	this->m_request.target.clear();
	this->m_request.connection.clear();
	this->m_request.uri.clear();
	this->m_request.version.clear();
	this->m_request.content_length = 0;
	this->m_request.headers.clear();
	this->m_request.headers.clear();

	std::memset(&this->m_response, 0, sizeof(t_response));
	std::memset(&this->m_body, 0, sizeof(t_body));

	this->m_responseCode = HttpStatus::OK;
	this->m_readStatus = Request::HEADER;
	this->m_server_message.clear();
	this->finishedReading = false;


}
