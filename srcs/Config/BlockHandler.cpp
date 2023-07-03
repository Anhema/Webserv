#include "BlockHandler.hpp"
#include "Keys.hpp"
#include "Config.hpp"

Parser::BlockHandler::BlockHandler(const std::string &id, const unsigned short max_deepness,
								   const unsigned short min_deepness):
								   m_identifier(id), m_max_deepness(max_deepness), m_min_deepness(min_deepness)
                                   {}

Parser::BlockHandler::~BlockHandler() {
    for (std::map<const string, Parser::Directive *>::iterator it = this->keyword_handler.begin(); it != this->keyword_handler.end(); it++)
        delete it->second;
    this->keyword_handler.clear();
    // cout << "BlockHandler Parent destructed\n";
}

void Parser::BlockHandler::AddKeywordHandler(const string &key, Parser::Directive *handler)
{
	if (this->keyword_handler.find(key) == this->keyword_handler.end())
		this->keyword_handler[key] = handler;
    else
        delete handler;
}

void Parser::BlockHandler::process(const Data::Line &line, Data::Conf *dst)
{
	if (this->keyword_handler.find(line.key) != this->keyword_handler.end())
	{
		this->keyword_handler[line.key]->handler(line.tokens, dst);
		this->keyword_handler[line.key]->Throw(line.raw);
	}
	else
	{
		throw (Parser::InvalidToken(line.raw, line.tokens.at(0), line.n));
	}
}

WebServ::ServerBlockParser::ServerBlockParser(): Parser::BlockHandler("server", 0, 0), dst(NULL)
{
    this->dst = new Data::Server;
}


WebServ::ServerBlockParser::~ServerBlockParser()
{
    delete this->dst;
    //cout << this->m_identifier << " *************************Block Handler destructed\n";
}


void WebServ::ServerBlockParser::validate_header(Data::Line const &header)
{
//    cout << "checking header: " << header << endl;
//    cout << "key: " << header.key << " size: " << header.tokens.size() << " || " << *header.tokens.begin() << endl;

//    Utils::print_vector(header.tokens);

    if (header.tokens.size() != 1 ||  header.key != this->m_identifier)
        throw (std::invalid_argument("Invalid header"));
}

void WebServ::ServerBlockParser::initHandlers()
{
    if (this->keyword_handler.empty())
    {
        this->AddKeywordHandler("ip", Parser::Directive::KeyFactory<Parser::Keys::Ip>());
        this->AddKeywordHandler("server_name", Parser::Directive::KeyFactory<Parser::Keys::ServerName>());
        this->AddKeywordHandler("ports", Parser::Directive::KeyFactory<Parser::Keys::Ports>());
        this->AddKeywordHandler("root", Parser::Directive::KeyFactory<Parser::Keys::Root>());
        this->AddKeywordHandler("max_body", Parser::Directive::KeyFactory<Parser::Keys::MaxBody>());
    }
}

Data::Conf *WebServ::ServerBlockParser::getDestination() const
{
    return dst;
}

/**
 * LOCATION BLOCK PARSER
 */

WebServ::LocationBlockParser::LocationBlockParser(): Parser::BlockHandler("location", 0, 0)
{
    //this->initHandlers();
    this->dst = new Data::Location;
}


WebServ::LocationBlockParser::~LocationBlockParser()
{
    delete this->dst;
    //cout << this->m_identifier << " *************************Block Handler destructed\n";
}


void WebServ::LocationBlockParser::validate_header(Data::Line const &header)
{
//    cout << "checking header: " << header << endl;
//    cout << "key: " << header.key << " size: " << header.tokens.size() << " || " << *header.tokens.begin() << endl;
//    Utils::print_vector(header.tokens);
    if (header.tokens.size() != 2 ||  header.key != this->m_identifier)
        throw (std::invalid_argument("Invalid header"));
}

void WebServ::LocationBlockParser::initHandlers()
{
    if (this->keyword_handler.empty())
    {
        this->AddKeywordHandler("root", Parser::Directive::KeyFactory<Parser::Keys::Root>());
		this->AddKeywordHandler("route", Parser::Directive::KeyFactory<Parser::Keys::LocationPath>());
		this->AddKeywordHandler("index", Parser::Directive::KeyFactory<Parser::Keys::Index>());
    }
}

Data::Conf *WebServ::LocationBlockParser::getDestination() const
{
    return dst;
}

WebServ::AcceptMethodBlockParser::AcceptMethodBlockParser(): Parser::BlockHandler("accept", 2, 2) {
	this->dst = new Data::Accept;
}

WebServ::AcceptMethodBlockParser::~AcceptMethodBlockParser() {

}

Data::Conf *WebServ::AcceptMethodBlockParser::getDestination() const {
	return dst;
}

void WebServ::AcceptMethodBlockParser::initHandlers() {
	this->AddKeywordHandler("method", Parser::Directive::KeyFactory<Parser::Keys::AcceptMethod>());
}

void WebServ::AcceptMethodBlockParser::validate_header(const Data::Line &header) {
	if (header.tokens.size() != 1 ||  header.key != this->m_identifier)
		throw (std::invalid_argument("Invalid header"));
}
