#include "ConfParser.hpp"


WebServ::ConfParser::ConfParser(const std::string &file): Parser::Reader(file) {

}

WebServ::ConfParser::~ConfParser() {

}

void WebServ::ConfParser::init() {
	this->m_rules.key_end = "";
	this->m_rules.useSemicolon = false;
	this->m_rules.comment = "#";
	this->m_rules.bracket_opener = '{';
	this->m_rules.bracket_closer = '}';
	this->m_rules.extension = "conf";
	this->m_checkFile();
	this->m_processor["server"] = Parser::BlockHandler::handlerFactory<WebServ::ServerBlockParser>();
}

void WebServ::ConfParser::start()
{
	this->m_find_bracket();
}
