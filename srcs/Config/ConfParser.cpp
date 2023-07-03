#include "ConfParser.hpp"

void WebServ::ConfParser::print() const
{
    for (std::vector<Data::Server >::const_iterator it = this->m_serverBrackets.begin(); it != this->m_serverBrackets.end(); it++)
    {
        cout << "=====NEW BRACKET====\n";
        cout << "Ip -> " << (it)->ip << endl;
        cout << "Ports -> ";
        Utils::print_vector((it)->ports);

        cout << "Server Names -> ";
        Utils::print_vector((it)->names);

        cout << "Root -> " << (it)->root << endl;
        cout << "Index -> " << (it)->index << endl;
        cout << "Accepted methods -> ";
        Utils::print_vector(it->accepted_methods.methods);
        cout << "Max Body -> " << (it)->max_body_size << endl;

        cout << "Error Pages\n";
        cout << "404 -> " << (it)->errors.error_404 << endl;
        cout << "502 -> " << (it)->errors.error_502 << endl;

        if ((it)->locations.empty())
            cout << "No locations found\n";
        else
        {
            for (std::vector<Data::Location>::const_iterator location_it = it->locations.begin(); location_it != (it)->locations.end(); location_it++)
            {
                cout << "====Location====\n";
                cout << "\tRoute-> " << (location_it)->route << "\n";
                cout << "\tRoot-> " << (location_it)->root << "\n";
				cout << "\tIndex-> " << (location_it)->index << "\n";
				cout << "\tAccept -> ";
				Utils::print_vector(location_it->accepted_methods.methods);
				cout << endl;
            }
        }
    }
}

WebServ::ConfParser::ConfParser(const std::string &file): Parser::Reader(file), m_serverBracket_count(-1), m_locationBracket_count(-1) {

}

WebServ::ConfParser::~ConfParser() {
    this->m_serverBrackets.clear();
    cout << "Parser destructed\n";

}

void WebServ::ConfParser::init()
{
	this->m_rules.key_end = "";
	this->m_rules.useSemicolon = false;
	this->m_rules.comment = "#";
	this->m_rules.bracket_opener = '{';
	this->m_rules.bracket_closer = '}';
	this->m_rules.extension = "conf";
    this->m_rules.line_delimiter = '\n';
	this->m_checkFile();
	this->m_BlockHandlers["server"] = Parser::BlockHandler::handlerFactory<WebServ::ServerBlockParser>();
    this->m_BlockHandlers["location"] = Parser::BlockHandler::handlerFactory<WebServ::LocationBlockParser>();
	this->m_BlockHandlers["accept"] = Parser::BlockHandler::handlerFactory<WebServ::AcceptMethodBlockParser>();
}

void WebServ::ConfParser::save(Data::Conf *data)
{
    if (Data::Server *server = dynamic_cast<Data::Server *>(data))
    {
        this->m_serverBrackets.push_back(*server);
        this->m_serverBracket_count++;
    }
    else if (Data::Location *location = dynamic_cast<Data::Location *>(data))
    {
        this->m_serverBrackets.at(this->m_serverBracket_count).locations.push_back(*location);
		this->m_locationBracket_count++;
    }
	else if (Data::Accept *accept = dynamic_cast<Data::Accept *>(data))
	{
		this->m_serverBrackets.at(0).locations.at(0).accepted_methods.methods = accept->methods;
	}
    else
	{
		throw (std::runtime_error("casting in confParser save"));
	}
	data->clear();
}

void WebServ::ConfParser::start()
{
	this->m_find_bracket();
	if (this->total_depth != 0)
		throw (std::invalid_argument("Las llaves no estan cerradas!"));
}


std::vector<Data::Server> WebServ::ConfParser::getBrackets() const
{
    return this->m_serverBrackets;
}
