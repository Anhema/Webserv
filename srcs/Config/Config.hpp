#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "../Utilities/Utilities.hpp"
# include <arpa/inet.h>
# include "Keys.hpp"
# include "DataContainers.hpp"

namespace Parser
{
	// Exceptions
	class KeysNotClosed: public std::invalid_argument
	{
	public:
		KeysNotClosed();
	};

	class Exception: public std::exception
	{
	protected:
		const string m_line;
		const string m_token;
		const int n;
		const string message;

	public:
		Exception(const std::string& line, const std::string &token, int n);

		virtual 		~Exception() throw() {}
		virtual void	print() = 0;

		const char* 	what() const throw() {
			return message.c_str();
		}
	};
	class SyntaxError: public std::invalid_argument
	{
	public:
		explicit SyntaxError(const string &line);
		SyntaxError();
	};

	class BadCheck
	{
	public:
		BadCheck(std::string const &bad_token);
		const std::string &token;
	};

	class TooManyArguments: public Exception
	{
	public:
		TooManyArguments(const std::string &line, const int expected, const int have);
		void print();
		int m_expected;
		int m_have;
	};

	class InvalidFile: public Exception
	{
	public:
		InvalidFile(const std::string &line, const std::string &file, int n);
		void print();
	};

	class InvalidToken: public Exception
	{
	public:
		InvalidToken(const std::string &line, const std::string &token, int n);
		void print();
	};

	class InvalidValue: public Exception
	{
	public:
		InvalidValue(const std::string &line, const std::string &token, int n);
		void print();
	};
}

typedef void (*ConfigFunction)(const std::vector<std::string>&, Data::Server&);

class Configuration {

public:
	bool getConfiguration(std::string conf_file);

	std::vector<Data::Server> &getConfig() { return this->configuration; }

private:
	std::vector<Data::Server> 	configuration;
	static const char				s_comment_char = '#';



	private:
	void								parse_bracket(string const &server);
	void								parse_location(const string &bracket, const std::vector<string> &start, Data::Server &config);
	std::vector<string> 				parse_line(string const &raw, const std::vector<string> &keywords);
	static std::vector<string> 			tokenize(const string &raw);
	static void 						set_default(Data::Server &data);
	static void							set_default(Data::Location &location, Data::Server const &bracket);
	static void 						set_default(Data::ErrorPages &data);
	static void							validateFiles(Data::Server &data);


private:
	static void							printConfig(Data::Server &config);
	static int							strport(std::string const &s);
	static const std::vector<string>	&Keywords();
	static const std::vector<string>	&Location_keywords();
	static const std::vector<string>	&Accepted_errors();


public:

};





#endif
