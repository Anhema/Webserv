#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../Utilities/Utilities.hpp"

typedef struct	s_error_pages
{

}	t_error_pages;

typedef struct	s_location
{
	string				route;
	string 				root;
	string				directory_file;
	string 				index;
	bool				directory_listing;
	std::vector<string>	accepted_methods;
	string				redirection;
}	t_location;

typedef struct s_server_config
{
	std::vector<int>				ports;
	std::vector<string>				names;
	string 							ip ;
	string 							root;
	string 							index;
	size_t 							max_body_size;
	t_error_pages 					errorPages;
	std::vector<t_location *>			locations;
}	t_server_config;

class Configuration {

public:
	bool getConfiguration(std::string conf_file);
	std::vector<t_server_config> &getConfig() {return this->configuration; }

private:
	std::vector<t_server_config>			configuration;

private:

	template<class T>
	static void print_vector(T x)
	{
		for (typename T::iterator it = x.begin(); it != x.end(); it++)
			cout << *it << " ";
	}

	// When the source type is different from src
	// Accepts a function to convert to the needed type
	template<class T>
	static void	save(const std::vector<string> &src, std::vector<T> &dst, T (*f)(string const &))
	{
		for (std::vector<string>::const_iterator it = src.begin() + 1; it != src.end(); it++)
			dst.push_back((f)(*it));
	}
	// When the src type is the same as dst
	template<class T>
	static void	save(const std::vector<string> &src, std::vector<T> &dst)
	{
		for (std::vector<string>::const_iterator it = src.begin() + 1; it != src.end(); it++)
			dst.push_back(*it);
	}

	template<class T>
	static void	save(const std::vector<string> &src, T &dst)
	{
		dst = src.at(1);
	}

	void								parse_bracket(string const &server);
	std::vector<string>					parse_line(string const &raw, const std::vector<string> &keywords);
	void								parse_location(const string &bracket, const std::vector<string> &start, t_server_config &config);
	static std::vector<string>			tokenize(const string &raw);
	static void							printConfig(t_server_config &config);
	static int 							strport(std::string const &s);

	static const std::vector<string>	&Keywords();
	static const std::vector<string>	&Location_keywords();


public:
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

		virtual ~Exception() throw() {}

		virtual void	print() = 0;

		const char* what() const throw() {
				return message.c_str();
		}
	};
	class SyntaxError: public std::invalid_argument
	{
	public:
		explicit SyntaxError(const string &line);
		SyntaxError();
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
};





#endif
