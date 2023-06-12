#include "Message.hpp"
#include "Logger.hpp"
#include <sys/socket.h>
#include "Utilities.hpp"
#include <fcntl.h>

Message::Message(): m_readStatus(ReadType::HEADER), finishedReading(false) {}

Message::~Message() {}

string &Message::getConnectionType() { return this->_request.connection; }

void Message::setConfig(t_server_config &config)
{
	this->m_configuration = config;
}

void Message::buildHeader()
{

}

void Message::m_createFile(const std::string &filename, const std::string &extension, std::ofstream &outfile)
{
	char			buffer[100];
	time_t 			t;
	struct tm		*timeptr;

	t = time(NULL);
	timeptr = localtime(&t);
	strftime(buffer, sizeof(buffer), "%d-%m-%y_%H-%M", timeptr);

	string composition_name;

	composition_name.append("uploads/");
	composition_name.append(filename);
	composition_name.append(buffer);
	composition_name.append(extension);

	outfile.open(composition_name.c_str(), std::ios::binary);
	if (outfile.fail())
		throw (std::runtime_error("can't create outfile for POST"));
}

std::string Message::m_get()
{
	std::ostringstream message;
	std::string path = "www";

	if (this->_request.uri == "/")
		this->_response.htmlFile = read_file(path.append("/index.html"));
	else
		path.append(this->_request.uri);

	std::ifstream ifs(path);

	if (!ifs.is_open())
	{
		this->_response.htmlFile = read_file("www/404.html");
		this->_response.extension = get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Type: text/" << this->_response.extension
			<<"\nContent-Length: " << this->_response.htmlFile.size() << "\n\n" << this->_response.htmlFile;
	}
	else
	{
		this->_response.htmlFile = read_file(path);
		this->_response.extension = get_extension(path);
		message << "HTTP/1.1 200 OK\nContent-Type:";
		if (this->_response.extension == "png" || this->_response.extension == "jpg")
			message << "image/";
		else
			message << "text/";
		message << this->_response.extension
				<<"\nContent-Length: " << this->_response.htmlFile.size() << "\n\n" << this->_response.htmlFile;
	}
	return (message.str());
}

std::string Message::m_post()
{
	std::ostringstream message;
	std::string path = "www";
	if (this->_request.uri == "/")
		this->_response.htmlFile = read_file(path.append("/index.html"));
	else
		path.append(this->_request.uri);
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs.is_open())
	{
		this->_response.htmlFile = read_file("www/404.html");
		this->_response.extension = get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Type: text/" << this->_response.extension
			<<"\nContent-Length: " << this->_response.htmlFile.size() << "\n\n" << this->_response.htmlFile;
	}
	else
	{
		this->_response.htmlFile = read_file(path);
		this->_response.extension = get_extension(path);
		message << "HTTP/1.1 200 OK\nContent-Type: text/" << this->_response.extension
				<<"\nContent-Length: " << this->_response.htmlFile.size() << "\n\n" << this->_response.htmlFile;
	}
	return (message.str());
}

std::string Message::m_delete()
{
	std::ostringstream message;
	std::string path = "www";
	if (this->_request.uri == "/")
		this->_response.htmlFile = read_file(path.append("/index.html"));
	else
		path.append(this->_request.uri);
	std::ifstream ifs(path);
	if (!ifs.is_open())
	{
		this->_response.htmlFile = read_file("www/404.html");
		this->_response.extension = get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Type: text/" << this->_response.extension
			<<"\nContent-Length: " << this->_response.htmlFile.size() << "\n\n" << this->_response.htmlFile;
		cout << "Extension: " << this->_response.extension << endl;
		cout << "Path: " << path << endl;
	}
	else
	{
		this->_response.htmlFile = read_file(path);
		std::remove(path.c_str());
		this->_response.extension = get_extension(path);
		message << "HTTP/1.1 200 OK\nContent-Type: text/" << this->_response.extension
				<<"\nContent-Length: " << this->_response.htmlFile.size() << "\n\n" << this->_response.htmlFile;
	}
	//close()
	return (message.str());
}

void print_headers(std::map<string, string> headers)
{
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
		if (err_count >= Message::maxRecvErrors)
		{
			header.clear();
			return header;

		}
	}
	return header;
}

void Message::m_parseHeader(const std::string &header)
{
	std::vector<std::string> request = split(header, "\n");
	std::vector<std::string>::iterator line = request.begin();
	std::vector<std::string> r_line = split((*line), " ");

	while (++line != request.end())
	{
		char *tmp_key = std::strtok((char *)line->c_str(), ": ");
		if (!tmp_key)
			break;

		capitalize(tmp_key, " -");

		const string key(tmp_key);

		char *tmp_value = std::strtok(NULL, "\n\r");
		if (!tmp_value)
			break;

		const string value(&tmp_value[1]);
		this->_request.headers[key] = value;
	}

	this->_request.method = r_line[0];
	this->_request.uri = r_line[1];
	this->_request.version = r_line[2];
	this->_request.connection = this->_request.headers["Connection"];
	this->_request.content_length = std::atol(this->_request.headers["Content-Length"].c_str());

	if (this->_request.content_length)
	{
		this->m_readStatus = ReadType::BODY;
		cout << "Tiene body\n";
	}
	else
	{
		Logger::log("Finished read -> true", INFO);
		this->finishedReading = true;
	}
}

void Message::m_readBody(const fd client, const size_t fd_size, std::ofstream &file)
{
	static size_t	totalRead;
	size_t 			read_errors = 0;
	size_t 			loopRead = 0;
	ssize_t 		read_bytes;
	char			*buffer;
	const size_t 	buffer_size = 30720;


	buffer = new char[buffer_size];
	cout << "Entra a leer el body\n";
	cout << "Readed: " << totalRead << "/" << this->_request.content_length << "\n";
	while (loopRead < (size_t)fd_size && totalRead < this->_request.content_length)
	{
		read_bytes = recv(client, buffer, buffer_size, 0);

		if (read_errors >= Message::maxRecvErrors)
		{
			Logger::log("timeout reading request", WARNING);
			break;
		}
		if (read_bytes == -1)
		{
			read_errors++;
			continue;
		}
		else
			read_errors = 0;

		file.write(buffer, read_bytes);

		totalRead += read_bytes;
		loopRead += read_bytes;

		cout << "Read: " << totalRead << "/" << this->_request.content_length << endl;
//		cout << "Wrote: " << totalWrote << "/" << this->_request.content_length << endl;
	}

	if (totalRead >= this->_request.content_length || this->_request.content_length == 0)
	{
		Logger::log("Finished read -> FINISHED BODY", INFO);
		this->m_readStatus = ReadType::FINISHED_BODY;
		this->finishedReading = true;
		totalRead = 0;
	}

	delete [] buffer;
}

void Message::request(const fd client, size_t buffer_size)
{

	stringstream  	ss;
	stringstream  	ss_buffer;
	std::ofstream 	outfile;

	ss << "Request fd: " << client << " size: " << buffer_size;
	Logger::log(ss.str(), INFO);
	cout << "****Reading****" << endl;

	this->m_createFile("test", ".jpg", outfile);
	if (this->m_readStatus == ReadType::HEADER)
	{
		const string header = this->m_readHeader(client);
		if (header.empty())
		{
			Logger::log("Failed to read header", ERROR);
			return;
		}
		this->m_parseHeader(header);
		print_headers(this->_request.headers);
	}
	else if (this->m_readStatus == ReadType::BODY)
	{
		this->m_readBody(client, buffer_size, outfile);
		cout << "Body Size: " << this->_request.body.size() << " Body:\n";
//		cout << this->_request.body;
	}
	if (this->m_readStatus == ReadType::FINISHED_BODY)
		outfile.close();

	//request.clear();
	//r_line.clear();
}

void Message::response(const fd client, size_t __unused buffer_size)
{
	stringstream  ss;

	ss << "Responding fd: " << client;
	Logger::log(ss.str(), INFO);
	cout << "****Writing****" << endl;

	std::ostringstream message;
	if (this->_request.method == "GET")
		this->_server_message = m_get();
	else if (this->_request.method == "DELETE")
		this->_server_message = this->m_delete();
	else
		this->_server_message = this->m_post();

	ssize_t totalSent = 0;
	ssize_t send_bytes = 0;
	size_t err_count = 0;

	while (totalSent < (ssize_t)this->_server_message.size())
	{
		send_bytes = send(client, this->_server_message.c_str() + totalSent, this->_server_message.size() - totalSent, 0);
		if (err_count >= Message::maxSendErrors)
		{
			Logger::log("timeout sending request", WARNING);
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
//		cout << "Sent: " << send_bytes << " Expected: " << _server_message.size() << " BufferSize: " << buffer_size << endl;
	}
//	cout << "Total sent: " << totalSent << " of " << this->_server_message.size() << endl;
	//tmp = 0;
	//send_bytes = 0;
	if (totalSent == (long)this->_server_message.size())
	{
		Logger::log("SERVER RESPONSE SENT TO CLIENT", INFO);
	}
	else
	{
		Logger::log("SENDING RESPONSE TO CLIENT", ERROR);
		cout << "Sent: " << send_bytes << " Expected: " << _server_message.size() << endl;
	}

	this->_request.headers.clear();
}
