#include "Parser.hpp"

Parser::UBT::Node *Parser::UBT::getNode(TYPES type)
{
	switch (type)
	{
		case LOCATION:
			return new LocationNode;
		case SERVER:
			return new ServerNode;
		case ERRORS:
			return new ErrorsNode;
		case METHODS:
			return new MethodsNode;
		default:
			return NULL;
	}
}

Parser::UBT::UBT()
{}

Parser::UBT::UBT(const Parser::UBT *rhs): m_root(rhs->m_root)
{}

size_t Parser::UBT::size() const
{
	return this->m_size;
}

bool Parser::UBT::empty() const {
	return this->m_size <= 0;
}

void Parser::UBT::clear() {

}

Parser::UBT::Node::Node(TYPES type): type(type), attr(type), left(NULL), right(NULL)
{}

Parser::UBT::NodeAttributes::NodeAttributes(Parser::UBT::TYPES type)
{
	switch (type)
	{
		case LOCATION:
			this->canNest = false;
			this->max_concurrencies = UNDEFINED;
		case SERVER:
			this->canNest = false;
			this->max_concurrencies = 1;
		case ERRORS:
			this->canNest = false;
			this->max_concurrencies = 1;
		case METHODS:
			this->canNest = false;
			this->max_concurrencies = 1;
		default:
			throw (std::invalid_argument("node attributes"));
	}
}

Parser::UBT::LocationNode::LocationNode(Data::Server const &context): Node(LOCATION), data(context) {}

Parser::UBT::LocationNode::LocationNode(): Node(LOCATION), data() {}

Parser::UBT::ServerNode::ServerNode(): Node(SERVER), data() {}

Parser::UBT::ErrorsNode::ErrorsNode(): Node(ERRORS), data() {}

Parser::UBT::MethodsNode::MethodsNode(): Node(METHODS), data() {}


Parser::Reader::Reader() {}

Parser::Reader::~Reader() {}

Parser::Reader::Reader(const std::string &file): m_filestream(file), m_filename(file), m_current_line(0) {

}

void Parser::Reader::init()
{
	this->m_rules.key_end = "";
	this->m_rules.useSemicolon = false;
	this->m_rules.comment = "#";
	this->m_rules.opener = '{';
	this->m_rules.ender = '}';
	this->m_rules.extension = "conf";
	this->m_checkFile();

}

void Parser::Reader::m_find_bracket()
{
	static std::string pre_line;
	for (std::string line; std::getline(this->m_filestream, line);)
	{
		this->m_current_line++;
		if (line.empty())
			continue;


		if (line.find(this->m_rules.opener) != std::string::npos && pre_line == "server")
		{
		}

		pre_line = line;
	}


}

std::vector<string> Parser::Reader::m_tokenize(const string &raw) {
	std::stringstream	line(raw);
	std::vector<string>	tokens;
	string 				tmp;

	while (line >> tmp)
		tokens.push_back(tmp);
	return tokens;
}

void Parser::Reader::m_checkFile() const
{
	// TODO better exceptions
	if (Utils::get_extension(this->m_filename) != this->m_rules.extension)
		throw (Parser::InvalidFile("", this->m_filename, 10));
	if (!this->m_filestream.is_open() || !this->m_filestream.good())
		throw (Parser::InvalidFile("", this->m_filename, 10));
}

void Parser::Reader::setRules(const Parser::Reader::Rules &rules)
{
	this->m_rules = rules;
}
