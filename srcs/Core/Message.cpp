#include "Message.hpp"
#include "Logger.hpp"
#include <sys/socket.h>
#include "Utilities.hpp"
#include <fcntl.h>

Message::Message(): m_responseCode(HttpStatus::OK), m_readStatus(ReadType::HEADER), finishedReading(false) {}

Message::~Message() {}

string &Message::getConnectionType() { return this->m_request.connection; }

void Message::setConfig(t_server_config &config)
{
	this->m_configuration = config;
}

void Message::buildHeader()
{

}

void Message::m_createFile(const std::string &filename, const std::string &extension, std::ofstream &outfile)
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
	composition_name.append(extension);

	outfile.open(composition_name.c_str(), std::ios::binary);
	if (outfile.fail())
		throw (std::runtime_error("can't create outfile for POST"));
}

std::string Message::m_get()
{
	std::ostringstream message;
	std::string path = "www";

	if (this->m_request.uri == "/")
		this->m_response.htmlFile = read_file(path.append("/index.html"));
	else
		path.append(this->m_request.uri);

	std::ifstream ifs(path);

	if (!ifs.is_open())
	{
		this->m_response.htmlFile = read_file("www/404.html");
		this->m_response.extension = get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Type: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
	}
	else
	{
		this->m_response.htmlFile = read_file(path);
		this->m_response.extension = get_extension(path);
		message << "HTTP/1.1 "<< this->m_responseCode << " OK\nContent-Type:";
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
		this->m_response.htmlFile = read_file(path.append("/index.html"));
	else
		path.append(this->m_request.uri);
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs.is_open())
	{
		this->m_response.htmlFile = read_file("www/404.html");
		this->m_response.extension = get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Type: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
	}
	else
	{
		this->m_response.htmlFile = read_file(path);
		this->m_response.extension = get_extension(path);
		message << "HTTP/1.1 200 OK\nContent-Type: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
	}
	return (message.str());
}

std::string Message::m_delete()
{
	std::ostringstream message;
	std::string path = "www";
	if (this->m_request.uri == "/")
		this->m_response.htmlFile = read_file(path.append("/index.html"));
	else
		path.append(this->m_request.uri);
	std::ifstream ifs(path);
	if (!ifs.is_open())
	{
		this->m_response.htmlFile = read_file("www/404.html");
		this->m_response.extension = get_extension("/404.html");
		message << "HTTP/1.1 404 Not Found\nContent-Type: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
		cout << "Extension: " << this->m_response.extension << endl;
		cout << "Path: " << path << endl;
	}
	else
	{
		this->m_response.htmlFile = read_file(path);
		std::remove(path.c_str());
		this->m_response.extension = get_extension(path);
		message << "HTTP/1.1 200 OK\nContent-Type: text/" << this->m_response.extension
				<< "\nContent-Length: " << this->m_response.htmlFile.size() << "\n\n" << this->m_response.htmlFile;
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
		this->m_request.headers[key] = value;
	}

	this->m_request.method = r_line[0];
	this->m_request.uri = r_line[1];
	this->m_request.version = r_line[2];
	this->m_request.connection = this->m_request.headers["Connection"];
	this->m_request.content_length = std::atol(this->m_request.headers["Content-Length"].c_str());

	if (this->m_request.content_length)
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
	cout << "Readed: " << totalRead << "/" << this->m_request.content_length << "\n";
	while (loopRead < (size_t)fd_size && totalRead < this->m_request.content_length)
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

		file.write(buffer, read_bytes);

		totalRead += read_bytes;
		loopRead += read_bytes;

		cout << "Read: " << totalRead << "/" << this->m_request.content_length << endl;
//		cout << "Wrote: " << totalWrote << "/" << this->m_request.content_length << endl;
	}

	if (totalRead >= this->m_request.content_length || this->m_request.content_length == 0)
	{
		Logger::log("Finished read -> FINISHED BODY", INFO);
		this->m_readStatus = ReadType::FINISHED_BODY;
		this->finishedReading = true;
		totalRead = 0;
	}

	delete [] buffer;
}

void Message::handle_request(const fd client, size_t buffer_size)
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
		print_headers(this->m_request.headers);
	}
	else if (this->m_readStatus == ReadType::BODY)
	{
		this->m_readBody(client, buffer_size, outfile);
		cout << "Body Size: " << this->m_request.body.size() << " Body:\n";
	}
	if (this->m_readStatus == ReadType::FINISHED_BODY)
		outfile.close();
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
//		cout << "Sent: " << send_bytes << " Expected: " << m_server_message.size() << " BufferSize: " << buffer_size << endl;
	}
//	cout << "Total sent: " << totalSent << " of " << this->m_server_message.size() << endl;
	//tmp = 0;
	//send_bytes = 0;
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
