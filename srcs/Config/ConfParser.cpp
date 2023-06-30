#include "ConfParser.hpp"


WebServ::ConfParser::ConfParser(const std::string &file): Parser::Reader(file) {

}

WebServ::ConfParser::~ConfParser() {

}

void WebServ::ConfParser::init() {
	this->m_rules.key_end = "";
	this->m_rules.useSemicolon = false;
	this->m_rules.comment = "#";
	this->m_rules.opener = '{';
	this->m_rules.ender = '}';
	this->m_rules.extension = "conf";
	this->m_checkFile();
}

void WebServ::ConfParser::start()
{
	this->m_find_bracket();
}
