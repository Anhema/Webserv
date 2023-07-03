#ifndef BLOCK_HANDLER_HPP
# define BLOCK_HANDLER_HPP

# include "string"
# include "map"
# include "Directive.hpp"
# include "utility"
//# include "Parser.hpp"

namespace Parser
{
	class BlockHandler
	{
	public:
		BlockHandler(std::string const &id, const unsigned short max_deepness, const unsigned short min_deepness);
		virtual ~BlockHandler();

		virtual void validate_header(Data::Line const &header) = 0;

		void process(Data::Line const &line, Data::Conf *dst);
		virtual void initHandlers() = 0;
        virtual Data::Conf* getDestination() const = 0;

		template<class T>
		static BlockHandler *handlerFactory()
		{
			return new T;
		}

	protected:
		std::map<const std::string, Parser::Directive *> keyword_handler;
		const std::string		m_identifier;
		const unsigned short	m_max_deepness;
		const unsigned short	m_min_deepness;
		void					AddKeywordHandler(const std::string &key, Directive *handler);
	};
}

namespace WebServ
{
	class ServerBlockParser: public Parser::BlockHandler
	{
	public:
		ServerBlockParser();
		~ServerBlockParser();
        Data::Conf* getDestination() const;
        void initHandlers();

	private:
		void validate_header(Data::Line const &header);
        Data::Server  *dst;
	};

    class LocationBlockParser: public Parser::BlockHandler
    {
    public:
        LocationBlockParser();
        ~LocationBlockParser();
        Data::Conf* getDestination() const;
        void initHandlers();

    private:
        void validate_header(Data::Line const &header);
        Data::Location  *dst;
    };

	class AcceptMethodBlockParser: public Parser::BlockHandler
	{
	public:
		AcceptMethodBlockParser();
		~AcceptMethodBlockParser();
		Data::Conf* getDestination() const;
		void initHandlers();

	private:
		void validate_header(Data::Line const &header);
		Data::Accept  *dst;
	};
}

#endif
