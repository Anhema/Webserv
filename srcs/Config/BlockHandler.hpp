#ifndef BLOCK_HANDLER_HPP
# define BLOCK_HANDLER_HPP

# include "string"

namespace Parser
{
	class BlockHandler
	{
	public:
		BlockHandler();
		virtual ~BlockHandler();

		virtual void valid_header() = 0;
		virtual void keyword_handler() = 0;

		template<class T>
		BlockHandler *handlerFactory()
		{
			return new T;
		}

	protected:
		std::string		m_identifier;
		unsigned short	m_max_deepness;
		unsigned short	m_min_deepness;

	};
}

namespace WebServ
{
	class ServerBlockParser: public Parser::BlockHandler
	{

	};

//	class LocationBlockParser: public Parser::BlockHandler
//	{
//
//	};
}

#endif
