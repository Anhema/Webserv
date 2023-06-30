#ifndef PARSER_HPP
# define PARSER_HPP

# include "DataContainers.hpp"
# include <fstream>
# include "Config.hpp"

#ifndef __WEBSERV_UNDEFINED_MAX_TOKENS
# define UNDEFINED -1
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
			char 		opener;
			char 		ender;
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

	protected:
		Rules						m_rules;
		std::ifstream				m_filestream;

		void						m_checkFile() const;
		void						m_find_bracket();



	private:
		std::string 				m_filename;
		std::vector<std::string>	m_lines;
		size_t 						m_current_line;
		static std::vector<string> 	m_tokenize(const string &raw);
	};
}

# endif
