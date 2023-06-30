#ifndef KEYS_HPP
# define KEYS_HPP

# include "string"
# include "vector"
# include "Directive.hpp"
# include "DataContainers.hpp"

namespace Parser
{
	namespace Keys
	{
		class ServerName: public Parser::Directive
		{
		public:
			ServerName();
			~ServerName();

		private:
			void	m_validate_token(const std::string &token);
			void	m_format_checker(const std::vector<std::string> &tokens);
			void	m_save(const std::vector<std::string> &tokens, Data::Server &config);
		};

		class Ip: public Parser::Directive
		{
		public:
			Ip();
			~Ip();

		private:
			void	m_validate_token(const std::string &token);
			void	m_format_checker(const std::vector<std::string> &tokens);
			void	m_save(const std::vector<std::string> &tokens, Data::Server &config);
		};

		class Ports: public Parser::Directive
		{
		public:
			Ports();
			~Ports();

		private:
			void	m_validate_token(const std::string &token);
			void	m_format_checker(const std::vector<std::string> &tokens);
			void	m_save(const std::vector<std::string> &tokens, Data::Server &config);
		};

		class Root: public Parser::Directive
		{
		public:
			Root();
			~Root();

		private:
			void	m_validate_token(const std::string &token);
			void	m_format_checker(const std::vector<std::string> &tokens);
			void	m_save(const std::vector<std::string> &tokens, Data::Server &config);
		};

		class MaxBody: public Parser::Directive
		{
		public:
			MaxBody();
			~MaxBody();

		private:
			void	m_validate_token(const std::string &token);
			void	m_format_checker(const std::vector<std::string> &tokens);
			void	m_save(const std::vector<std::string> &tokens, Data::Server &config);

		private:
			size_t	m_max_bytes;
		};
	}

}

#endif
