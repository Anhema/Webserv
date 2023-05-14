#include "Logger.hpp"


Logger::Logger() {}

Logger::Logger(const Logger &obj) { (void)obj; }

Logger::~Logger() {}

Logger::VerboseNoMatchException::VerboseNoMatchException(): std::invalid_argument("invalid verbose level") {}

void Logger::log_error(string const &message) {

    cout << BOLDRED << "[ERROR]" << NC << " " << message << endl;

}

void Logger::log(string const &message, levels level) {

    switch (level)
    {
        case ERROR:
            log_error(message);
            break;
        case WARNING:
            log_error(message);
            break;
        case INFO:
            log_error(message);
            break;
    }
    log_error(message);
    (void)level;
}
