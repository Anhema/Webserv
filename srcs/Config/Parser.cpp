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
	this->m_rules.bracket_opener = '{';
	this->m_rules.bracket_closer = '}';
	this->m_rules.extension = "conf";
	this->m_checkFile();

}

void Parser::Reader::m_find_bracket()
{
	std::string pre_line;
	for (std::string line; std::getline(this->m_filestream, line);)
	{
		this->m_current_line++;
		cout << "Parsing 1 -> " << line << endl;
		if (line.empty())
			continue;

		if (pre_line.empty() && line.find(this->m_rules.bracket_opener) != std::string::npos)
			throw (std::invalid_argument("JUAN"));
		else
			this->m_read_bracket(strline(pre_line));
//		if (line.find(this->m_rules.bracket_opener) != 0)
//			this->m_read_bracket(strline(pre_line));
		pre_line = line;
	}


}

void Parser::Reader::m_read_bracket(Data::Line const &header)
{
	BlockHandler *handler;
	Data::Conf * dst = new Data::Server;

	cout << "key-> " << header.key << endl;
	cout << "raw-> " << header.raw << endl;

	if (this->m_processor.find(header.key) != this->m_processor.end())
		 handler = this->m_processor[header.key];
	else
		throw (std::invalid_argument("VAYA PERRO EH, QUE PONES UN BLOQUE QUE NO DEBIAS EH!"));
	for (std::string line; std::getline(this->m_filestream, line);)
	{
		cout << "Parsing 2 -> " << line << endl;
		if (line.empty())
			continue;
//		cout << "Line: " << line << " | find: " << line.find(this->m_rules.bracket_closer, 0) << endl;
		if (line.find(this->m_rules.bracket_closer, 0) == 0)
			return;

		handler->process(strline(line), dst);
	}


	throw (std::invalid_argument("Cierra las llaves jaja"));
}

Data::Line Parser::Reader::strline(const std::string &raw) {
	std::stringstream	line(raw);
	string 				tmp;
	Data::Line			ret;

	ret.raw = raw;
	while (line >> tmp)
	{
		if (ret.tokens.empty())
			ret.key = tmp;
		ret.tokens.push_back(tmp);
	}
	return ret;
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
