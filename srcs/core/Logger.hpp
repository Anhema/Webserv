#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "Utilities.hpp"

# ifndef VERBOSE
# define VERBOSE 2
# endif



// Verbose level 2 = all messages will be displayed in the terminal
// Verbose level 1 = only errors and warning will be displayed
// Verbose level 0 = only errors will be displayed

class Logger {
private:

    enum colors {
        RED,
        GREEN,
        BLUE
    };

    enum levels {
        ERROR,
        WARNING,
        INFO
    };

public:

    Logger();                   // Default constructor
    Logger(Logger const &obj);  // Copy constructor
    ~Logger();                  // Default destructor

    static levels   get_level(void);

    static void     log(string const &message, levels level);
    static void     log(string &message, levels level);
    static void     log(string const &message, levels level, colors color);
    static void     log(string &message, levels, colors color);

class VerboseNoMatchException: public std::invalid_argument {
public:
    VerboseNoMatchException();
};

};
#endif
