#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "Utilities.hpp"

# ifndef VERBOSE
# define VERBOSE 2
# endif


 enum levels {
        ERROR,
        WARNING,
        INFO
    };
// Verbose level 2 = all messages will be displayed in the terminal
// Verbose level 1 = only errors and warning will be displayed
// Verbose level 0 = only errors will be displayed

class Logger {
private:



public:

    Logger();                   // Default constructor
    Logger(Logger const &obj);  // Copy constructor
    ~Logger();                  // Default destructor

    static levels   get_level(void);

    static void     log_error(string const &message);

    static void     log(string const &message, levels level);
    static void     log(string &message, levels level);
    static void     log(string const &message, levels level, string color);
    static void     log(string &message, levels, string color);

class VerboseNoMatchException: public std::invalid_argument {
public:
    VerboseNoMatchException();
};

};
#endif
