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

std::string Message::m_get()
{
	std::ostringstream message;
	std::string path = "www";

	if (this->m_request.uri == "/")
		this->m_response.htmlFile = Utils::read_file(path.append("/index.html"));
	else
		path.append(this->m_request.uri);

	std::ifstream ifs(path);

	if (!ifs.is_open())
	{
		this->m_response.htmlFile = Utils::read_file("www/404.html");
		this->m_response.extension = Utils::get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Status: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
	}
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
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
	}
	return (message.str());
}

std::string Message::m_post()
{
	std::ostringstream message;
	std::string path = "www";
	if (this->m_request.uri == "/")
		this->m_response.htmlFile = Utils::read_file(path.append("/index.html"));
	else
		path.append(this->m_request.uri);
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs.is_open())
	{
		this->m_response.htmlFile = Utils::read_file("www/404.html");
		this->m_response.extension = Utils::get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Status: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
	}
	else
	{
		this->m_response.htmlFile = Utils::read_file(path);
		this->m_response.extension = Utils::get_extension(path);
		message << "HTTP/1.1 200 OK\nContent-Status: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
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
	if (!ifs.is_open())
	{
		this->m_response.htmlFile = Utils::read_file("www/404.html");
		this->m_response.extension = Utils::get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Status: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
		cout << "Extension: " << this->m_response.extension << endl;
		cout << "Path: " << path << endl;
	}
	else
	{
		this->m_response.htmlFile = Utils::read_file(path);
		std::remove(path.c_str());
		this->m_response.extension = Utils::get_extension(path);
		message << "HTTP/1.1 200 OK\nContent-Status: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
	}
	//close()
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
            print_headers(this->m_request.headers, this->m_request);
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

void Message::make_response(const fd client, size_t __unused buffer_size)
{
	stringstream  ss;

	ss << "Responding fd: " << client;
	Logger::log(ss.str(), INFO);
	cout << "****Writing****" << endl;

	std::ostringstream message;
	if (this->m_request.method == "GET")
		this->m_server_message = m_get();
	else if (this->m_request.method == "DELETE")
		this->m_server_message = this->m_delete();
	else
		this->m_server_message = this->m_post();

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
