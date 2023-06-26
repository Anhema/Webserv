#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "../Utilities/Utilities.hpp"
#include <arpa/inet.h>

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
	std::vector<int>			ports;
	std::vector<string>			names;
	string 						ip;
	// All this attributes have a default values.
	std::vector<string>			accepted_methods;
	string 						root;
	string 						index;
	size_t 						max_body_size;
	t_error_pages 				errorPages;
	std::vector<t_location *>	locations;
}	t_server_config;

typedef void (*ConfigFunction)(const std::vector<std::string>&, t_server_config&);

class Configuration {

public:
	bool getConfiguration(std::string conf_file);

	std::vector<t_server_config> &getConfig() { return this->configuration; }

private:
	std::vector<t_server_config> 	configuration;
	static const char				s_comment_char = '#';

private:
	static std::string	check_server_name(std::string const &name);
	void				save_server_name(const std::vector<string> &names, t_server_config &config);

	static std::string	check_server_ip(std::string const &ip);
	void				save_server_ip(const std::vector<string> &ip, t_server_config &config);

	static int			check_server_ports(std::string const &port);
	void				save_server_ports(const std::vector<string> &ports, t_server_config &config);

	bool				check_server_root(std::string const &root);
	void				save_server_root(const std::vector<string> &root, t_server_config &config);

typedef void (Configuration::*ConfigMemberFunction)(const std::vector<std::string>&, t_server_config&);

private:
	static void save_to_server(string const &key, std::string const &line, const std::vector<string> &tokens, t_server_config &config);


	template<class T>
	static void print_vector(T x) {
		for (typename T::iterator it = x.begin(); it != x.end(); it++)
			cout << *it << " ";
		cout << "\n";
	}

	// When the source type is different from src
	// Accepts a function to convert to the needed type
	template<class T>
	void save(const std::vector<string> &src, std::vector<T> &dst, T (*f)(string const &)) {
		for (std::vector<string>::const_iterator it = src.begin() + 1; it != src.end(); it++)
			dst.push_back((f)(*it));
	}
	template<class T>
	void save(const std::vector<string> &src, T &dst, T (*f)(string const &)) {
		dst = (f)(src.at(1));
	}

//	// When the src type is the same as dst
//	template<class T>
//	void save(const std::vector<string> &src, std::vector<T> &dst) {
//		for (std::vector<string>::const_iterator it = src.begin() + 1; it != src.end(); it++)
//			dst.push_back(*it);
//	}
//
//	template<class T>
//	void save(const std::vector<string> &src, T &dst) {
//		dst = src.at(1);
//	}

private:
	void								parse_bracket(string const &server);
	void								parse_location(const string &bracket, const std::vector<string> &start, t_server_config &config);
	std::vector<string> 				parse_line(string const &raw, const std::vector<string> &keywords);
	static std::vector<string> 			tokenize(const string &raw);
	static void 						set_default(t_server_config &data);
	static void 						set_default(t_location &data);
	static void 						set_default(t_error_pages &data);


private:
	static void							printConfig(t_server_config &config);
	static int							strport(std::string const &s);
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
