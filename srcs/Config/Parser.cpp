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


Parser::Reader::BracketPosition::BracketPosition(const Parser::Reader::BracketPosition &rhs): type(rhs.type), pos(rhs.pos), depth(rhs.depth) {

}

Parser::Reader::Reader() {}

Parser::Reader::~Reader() {
    for (std::map<const string, Parser::BlockHandler *>::iterator it = this->m_BlockHandlers.begin(); it != this->m_BlockHandlers.end(); it++)
        delete it->second;
    cout << "Reader destructed\n";
}

Parser::Reader::Reader(const std::string &file): m_filestream(file.c_str()), m_filename(file), m_current_line(0) {
	this->total_depth = 0;
    this->total_pos = -1;

}

void Parser::Reader::init()
{
	this->m_rules.key_end = "";
	this->m_rules.useSemicolon = false;
	this->m_rules.comment = '#';
	this->m_rules.bracket_opener = '{';
	this->m_rules.bracket_closer = '}';
	this->m_rules.extension = "conf";
	this->m_checkFile();

}

bool Parser::Reader::lineIsOpener(const Data::Line &line)
{
    static int last_depth = total_depth;
    if (*(line.raw.end() -1) == this->m_rules.bracket_opener)
	{
		if (!line.key.empty())
		{
            for (std::list<Parser::Reader::BracketPosition>::iterator it = m_bracket_positions.begin(); it != m_bracket_positions.end(); it++)
                if (it->depth == total_depth)
                    total_pos = it->pos;
            if (last_depth < total_depth)
                total_pos = -1;
            total_pos++;
            this->m_bracket_positions.push_back(Parser::Reader::BracketPosition(total_pos, total_depth, line.key));
            last_depth = total_depth;
		}
		return true;
	}
	return false;
}

bool Parser::Reader::lineIsCloser(const Data::Line &line)
{
    if (line.raw.at(line.raw.find_first_not_of(WHITESPACE)) == this->m_rules.bracket_closer)
	{
		return true;
	}
	return false;
}

Parser::Reader::BracketPosition::BracketPosition(
        int pos, int depth, std::string const &type): type(type), pos(pos), depth(depth) {

}

Parser::Reader::BracketPosition::~BracketPosition() {

}

Parser::Reader::BracketPosition &Parser::Reader::getPostion(int x, int y)
{
    for (std::list<BracketPosition>::iterator it = m_bracket_positions.begin(); it != m_bracket_positions.end(); it++)
    {
        if (it->pos == x && it->depth == y)
            return *it;
    }
    throw (std::invalid_argument("can't find bracket"));

}

void Parser::Reader::print_brackets(std::list<BracketPosition> &brackets, int depth) {

	cout << "=============================\n";
	(void)depth;
	for (std::list<BracketPosition>::iterator it = brackets.begin(); it != brackets.end(); it++)
	{
		for (int i = 0; i < it->depth; i++)
			cout << '\t';
		cout << " type: " << it->type << " pos: " << it->pos << " depth: " << it->depth << endl;
	}

}

void Parser::Reader::m_getBracketData(std::stringstream &dst)
{
    Data::Line line("", "", this->m_current_line);

    for (std::string raw; std::getline(this->m_filestream, raw);)
    {
		if (raw.empty())
			continue;
		else if (raw.find_first_not_of(WHITESPACE) == '#')
			continue;
        raw.erase(0, raw.find_first_not_of(WHITESPACE) -1);

        this->m_current_line++;
        line.update(raw);
//		cout << "Parsing 3 -> " << raw << endl;
        if (lineIsCloser(line) || lineIsOpener(line))
        {
            this->m_filestream.seekg(this->m_filestream.tellg() - static_cast<std::streampos>(raw.size()));
            break;
        }
        dst << raw << endl;
    }
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
			cout << "Abre: " << line.raw << endl;
			this->total_depth++;
//            cout << "Is opener! " << line << endl;
            std::stringstream new_bracket;
            this->m_getBracketData(new_bracket);
//			cout << "Bracket filled:\n" << new_bracket.str();
            this->m_read_bracket(new_bracket, line);
        }
		else if (lineIsCloser(line))
		{

			cout << "Cierra: " << line.raw << endl;
			this->total_depth--;
		}
		else
			throw (std::invalid_argument("Debes debes de terminar de declarar todo la informacion antes de abrir un bracket"));
	}
	this->print_brackets(this->m_bracket_positions, 0);
}

void Parser::Reader::m_read_bracket(std::stringstream &bracket, Data::Line const &header)
{
	BlockHandler *handler;

	if (this->m_BlockHandlers.find(header.key) != this->m_BlockHandlers.end())
    {
        handler = this->m_BlockHandlers[header.key];
        handler->initHandlers();
    }
	else
		throw (std::invalid_argument("VAYA PERRO EH, QUE PONES UN BLOQUE QUE NO DEBIAS EH!"));

	handler->validate_header(header);

	if (handler->getMinDeepness() < this->total_depth || handler->getMaxDeepness() > this->total_depth)
		throw (std::invalid_argument("Block at invalid depth"));

    Data::Line line("", "", header.n);

	for (std::string raw; std::getline(bracket, raw);)
	{
        this->m_current_line++;

		if (raw.empty())
			continue;
//		else if (raw.find_first_not_of(WHITESPACE) == this->m_rules.comment)
//			continue;
		raw.erase(raw.begin(), raw.begin() + raw.find_first_not_of(WHITESPACE));
        line.update(raw);
        line.tokenize();


//		cout << "Parsing 2 -> " << line << endl;
        if (lineIsOpener(line) || lineIsCloser(line))
        {
            this->save(handler->getDestination());
            return;
        }
		handler->process(strline(raw), handler->getDestination());
	}
    this->save(handler->getDestination());
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
