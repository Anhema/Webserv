#ifndef PARSER_HPP
# define PARSER_HPP

# include "DataContainers.hpp"
# include <fstream>
# include "Config.hpp"
# include "BlockHandler.hpp"
# include "list"

#ifndef PARSER_UNDEFINED_MAX_TOKENS
# define PARSER_UNDEFINED_MAX_TOKENS (-1)
#endif



namespace Parser
{
	class UBT
	{
		enum TYPES
		{
			LOCATION,
			SERVER,
			ERRORS,
			METHODS
		};

	public:
		struct NodeAttributes
		{
			NodeAttributes(TYPES type);
			bool			canNest;
			short			max_concurrencies;
		};

		struct Node
		{
			Node(TYPES type);
			Parser::UBT::TYPES		type;
			const NodeAttributes	attr;
			Node 					*left;
			Node 					*right;
		};

		struct ServerNode: Node
		{
			ServerNode();

			Data::Server				data;
		};

		struct LocationNode: Node
		{
			LocationNode(Data::Server const &context);
			LocationNode();

			Data::Location 				data;
		};

		struct ErrorsNode: Node
		{
			ErrorsNode();

			Data::ErrorPages			data;
		};

		struct MethodsNode: Node
		{
			MethodsNode();

			std::vector<std::string>	data;
		};

	public:
		UBT();
		explicit UBT(UBT const *rhs);

		size_t					size() const;
		bool					empty() const;
		void					clear();
		static Node 			*getNode(TYPES type);

	private:
		std::vector<Node*>		m_root;
//		Node					*m_current;
		size_t 					m_size;
	};

	class Reader
	{

	public:
		struct Rules
		{
			bool		useSemicolon;
			char 		bracket_opener;
			char 		bracket_closer;
            char        line_delimiter;
			std::string extension;
			std::string comment;
			std::string key_end;
		};
	public:
		Reader();
		Reader(std::string const &file);
		Reader(Reader const &rhs);
		virtual ~Reader();

		void			setRules(Rules const &rules);

		virtual void				init() = 0;
		virtual void				start() = 0;
        virtual void                save(Data::Conf *data) = 0;

	protected:
		Rules												m_rules;
		std::ifstream										m_filestream;
		std::map<const std::string, Parser::BlockHandler *>	m_BlockHandlers;

		void						m_checkFile() const;
		void						m_find_bracket();
		void						m_read_bracket(std::stringstream &bracket, Data::Line const &header);
        void                        m_getBracketData(std::stringstream &dst);
        bool                        lineIsOpener(const Data::Line &line);
        bool                        lineIsCloser(const Data::Line &line);
		int 						total_depth;

	private:
		struct BracketPosition
		{
			BracketPosition(int pos, int depth, std::string const &type, int id);
			BracketPosition(struct BracketPosition const &rhs);
			~BracketPosition();
			int 								id;
			const std::string 					type;
			int 								pos;
			int 								depth;
			std::vector<struct BracketPosition> next_level;
		};
		std::string 										m_filename;
		std::vector<std::string>							m_lines;
		size_t 												m_current_line;
		std::list<BracketPosition>							m_bracket_positions;
		Data::Line 					strline(const string &raw);
	public:
		void						print_brackets(std::list<BracketPosition> &brackets, int depth);
	};
}

# endif
