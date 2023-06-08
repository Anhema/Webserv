#include "Message.hpp"
#include "Logger.hpp"


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
		cout << it->first << ":" << it->second << "\n";
	
}

void Message::request(const fd client, size_t buffer_size)
{

	stringstream  ss;
	stringstream  ss_buffer;

	int body_start = 0;
	
	ss << "Request fd: " << client << " size: " << buffer_size;
	Logger::log(ss.str(), INFO);
	
	char *buff= new char[buffer_size + 1];

	if (read(client, buff, buffer_size) < 0)
	{
		Logger::log("Failed to read bytes from client socket connection", ERROR);
		return ;
	}
	buff[buffer_size] = '\0';
	string str_buff(buff);

	for (size_t i = 0; i < str_buff.length(); i++)
	{
		if (str_buff[i] == '\n' && str_buff[i + 1] != '\n')
			body_start++;
		if (str_buff[i] == '\n' && str_buff[i + 1] == '\n')
			break;
	}

	cout << str_buff << "\n";

	std::vector<std::string> request = split(str_buff, "\n");
	
	std::vector<std::string>::iterator start = request.begin();
	std::vector<std::string> r_line = split((*start), " ");
	this->_request.method = r_line[0];
	this->_request.uri = r_line[1];
	this->_request.version = r_line[2];
	start++;

	bool is_body = false;

	int i = 1;
	for (std::vector<string>::iterator it = start; it != request.end(); it++)
	{
		i++;
		if (i == body_start)
			is_body = true;
			
		if ((*it) == "\n")
		{
			is_body = true;
			continue;
		}
		if (is_body)
		{
			this->_request.body.append(*it);
		}
		else
		{
			if ((*it) == "")
				continue;
			std::vector<std::string> tmp = split((*it), ":");
			string val = ""; 
			for (size_t i = 1; i < tmp.size(); i++)
			{
				val.append(tmp[i]);
				if (i + 1 != tmp.size())
					val.append(":");
			}
			this->_request.headers.insert(std::make_pair(tmp[0], val));
			tmp.clear();
		}
	}
	request.clear();
	r_line.clear();
	delete[] buff;

	cout << "\n\nHEADERS\n";
	print_headers(this->_request.headers);
	cout << "\n\nBODY\n" << this->_request.body << "\n\n";
}


void Message::response(const fd client)
{
	stringstream  ss;

	ss << "Responding fd: " << client;
	Logger::log(ss.str(), INFO);
	size_t send_bytes = 0;

	std::ostringstream message;
	if (this->_request.method == "GET")
		this->_server_message = m_get();
	else if (this->_request.method == "DELETE")
		this->_server_message = this->m_delete();
	else
		this->_server_message = this->m_post();

	send_bytes = write(client, this->_server_message.c_str(), this->_server_message.size());
	if (send_bytes == this->_server_message.size())
	{
		Logger::log("SERVER RESPONSE SENT TO CLIENT", INFO);
	}
	else
		Logger::log("SENDING RESPONSE TO CLIENT", ERROR);
	this->_request.headers.clear();
}
