#include "Message.hpp"
#include "../Logger/Logger.hpp"


std::string Message::get() {
	std::ostringstream message;
	std::string path = "www";
	if (this->_request.target == "/")
		this->_response.htmlFile = read_file(path.append("/index.html"));
	else
	{
		path.append(this->_request.target);
		std::cout << "FILE: ---" << path << "---\n";
		this->_response.htmlFile = read_file(path);
	}
	this->_response.extension = get_extension(path);
	std::cout << "EXTENSION: ---" << this->_response.extension << "---\n";
	message << "HTTP/1.1 200 OK\nContent-Type: text/" << this->_response.extension
			<<"\nContent-Length: " << this->_response.htmlFile.size() << "\n\n" << this->_response.htmlFile;
	return (message.str());
}

void Message::request(const fd client, size_t buffer_size) {

	stringstream  ss;

	ss << "Request fd: " << client << " size: " << buffer_size;
	Logger::log(ss.str(), INFO);
	std::unique_ptr<char []>buffer;

	buffer.reset(new char[buffer_size]);
	if (read(client, buffer.get(), buffer_size) < 0)
	{
		 cout << ("Failed to read bytes from client socket connection\n");
		 return ;
	}


	std::vector<std::string> request = split(buffer.get(), "\n");
	std::vector<std::string>::iterator ite = request.end();
	std::vector<std::string>::iterator start = request.begin();
	std::vector<std::string> r_line = split((*start), " ");
	this->_request.method = r_line[0];
	this->_request.target = r_line[1];
	this->_request.version = r_line[2];
	start++;
	for (std::vector<std::string>::iterator it = start; it != ite; it++)
		cout << *it << "\n";
	cout << this->_request.method << endl;
	cout << this->_request.target << endl;
	cout << this->_request.version << endl;
}

void Message::response(const fd client) {
	stringstream  ss;

	ss << "Responding fd: " << client;
	Logger::log(ss.str(), INFO);
	size_t send_bytes = 0;

	std::ostringstream message;
	if (this->_request.method == "GET")
		this->_server_message = get();
	else
	{
		// Post...
	}

	send_bytes = write(client, this->_server_message.c_str(), this->_server_message.size());
	if (send_bytes == this->_server_message.size())
		cout << ("-------- SERVER RESPONSE SENT TO CLIENT --------\n");
	else
		cout << ("------- ERROR SENDING RESPONSE TO CLIENT -------\n");
}

