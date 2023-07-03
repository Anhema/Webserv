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
			this->max_concurrencies = PARSER_UNDEFINED_MAX_TOKENS;
            break;
		case SERVER:
			this->canNest = false;
			this->max_concurrencies = 1;
            break;
		case ERRORS:
			this->canNest = false;
			this->max_concurrencies = 1;
            break;
		case METHODS:
			this->canNest = false;
			this->max_concurrencies = 1;
            break;
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

Parser::Reader::~Reader() {
    for (std::map<string, Parser::BlockHandler *>::iterator it = this->m_BlockHandlers.begin(); it != this->m_BlockHandlers.end(); it++)
        delete it->second;
    cout << "Reader destructed\n";
}

Parser::Reader::Reader(const std::string &file): m_filestream(file.c_str()), m_filename(file), m_current_line(0) {

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

bool Parser::Reader::lineIsOpener(const Data::Line &line)
{
    // Case ->
    // server
    // {
//    cout << "checking: " << *(line.raw.end() -1) << endl;
    return *(line.raw.end() -1) == this->m_rules.bracket_opener;
//    else if (pre_has_key && !current_is_key)


//    return false;
}

bool Parser::Reader::lineIsCloser(const Data::Line &line)
{
    return (*line.raw.begin() == this->m_rules.bracket_closer);
}

void Parser::Reader::m_getBracketData(std::stringstream &dst)
{
    Data::Line line("", "", this->m_current_line);

    for (std::string raw; std::getline(this->m_filestream, raw);)
    {
        raw.erase(0, raw.find_first_not_of(WHITESPACE) -1);

        this->m_current_line++;
        line.update(raw);
        if (lineIsCloser(line) || lineIsOpener(line))
        {
            this->m_filestream.seekg(this->m_filestream.tellg() - static_cast<std::streampos>(raw.size()));
            break;
        }
        dst << raw << endl;
    }
    cout << "Bracket data:\n" << dst.str();
}

void Parser::Reader::m_find_bracket()
{
    Data::Line line("", "", this->m_current_line);

	for (std::string raw; std::getline(this->m_filestream, raw);)
	{
        raw.erase(raw.begin(), raw.begin() + raw.find_first_not_of(WHITESPACE));
		this->m_current_line++;

        line.update(raw);
        line.tokenize();
        if (raw.empty())
            continue;
        cout << "Parsing 1 -> " << line << endl;
        if (this->lineIsOpener(line))
        {
            cout << "Is opener! " << line << endl;
            std::stringstream new_bracket;

            this->m_getBracketData(new_bracket);

//            new_bracket << this->m_getBracketData().str();
            this->m_read_bracket(new_bracket, line);
            cout << "Bracket filled:\n" << new_bracket.str();
        }
	}
}

void Parser::Reader::m_read_bracket(std::stringstream &bracket, Data::Line const &header)
{
	BlockHandler *handler;

	cout << "key-> " << header.key << endl;
	cout << "raw-> " << header.raw << endl;

	if (this->m_BlockHandlers.find(header.key) != this->m_BlockHandlers.end())
    {
        handler = this->m_BlockHandlers[header.key];
        handler->initHandlers();
    }
	else
		throw (std::invalid_argument("VAYA PERRO EH, QUE PONES UN BLOQUE QUE NO DEBIAS EH!"));

	handler->validate_header(header);
    Data::Line line("", "", header.n);

	for (std::string raw; std::getline(bracket, raw);)
	{
        raw.erase(raw.begin(), raw.begin() + raw.find_first_not_of(WHITESPACE));
        this->m_current_line++;

        line.update(raw);
        line.tokenize();

		cout << "Parsing 2 -> " << line << endl;
        if (lineIsOpener(line) || lineIsCloser(line))
        {
            this->save(handler->getDestination());
//            handler->clear();
            return;
        }
		if (raw.empty())
			continue;
		handler->process(strline(raw), handler->getDestination());
	}
    this->save(handler->getDestination());
//    handler->clear();
}

Data::Line Parser::Reader::strline(const std::string &raw)
{
	std::stringstream	line(raw);
	string 				tmp;
	Data::Line			ret;

	ret.raw = raw;
	while (line >> tmp)
	{
		if (ret.tokens.empty() && ret.key.empty())
			ret.key = tmp;
        else
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
