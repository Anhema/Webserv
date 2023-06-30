#include "BlockHandler.hpp"
#include "Keys.hpp"
#include "Config.hpp"

Parser::BlockHandler::BlockHandler(const std::string &id, const unsigned short max_deepness,
								   const unsigned short min_deepness):
								   m_identifier(id), m_max_deepness(max_deepness), m_min_deepness(min_deepness) {

}

Parser::BlockHandler::~BlockHandler() {

}

void Parser::BlockHandler::AddKeywordHandler(const string &key, Parser::Directive *handler)
{
	if (this->keyword_handler.find(key) == this->keyword_handler.end())
		this->keyword_handler[key] = handler;
}

WebServ::ServerBlockParser::ServerBlockParser(): Parser::BlockHandler("server", 0, 0)
{
	this->initHandlers();
}


WebServ::ServerBlockParser::~ServerBlockParser()
{

}


void WebServ::ServerBlockParser::validate_header(Data::Line const &header)
{
	if (header.tokens.size() != 1)
		throw (std::invalid_argument("invalid header"));

}

void WebServ::ServerBlockParser::initHandlers()
{
	this->AddKeywordHandler("ip", Parser::Directive::KeyFactory<Parser::Keys::Ip>());
	this->AddKeywordHandler("server_name", Parser::Directive::KeyFactory<Parser::Keys::ServerName>());
	this->AddKeywordHandler("ports", Parser::Directive::KeyFactory<Parser::Keys::Ports>());
	this->AddKeywordHandler("root", Parser::Directive::KeyFactory<Parser::Keys::Root>());
	this->AddKeywordHandler("max_body", Parser::Directive::KeyFactory<Parser::Keys::MaxBody>());
}

void WebServ::ServerBlockParser::process(Data::Line const &line, Data::Conf *dst)
{
	Data::Server *out = dynamic_cast<Data::Server *>(dst);

	if (this->keyword_handler.find(line.key) != this->keyword_handler.end())
	{
		this->keyword_handler[line.key]->handler(line.tokens, *out);
		this->keyword_handler[line.key]->Throw(line.raw);
	}
	else
	{
		throw (Parser::InvalidToken(line.raw, line.tokens.at(0), line.n));
	}
}
