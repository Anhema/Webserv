#include "Logger.hpp"
#include <fstream>
#include <sstream>

Logger::Logger() {}

Logger::Logger(const Logger &obj) { (void)obj; }

Logger::~Logger() {}

Logger::VerboseNoMatchException::VerboseNoMatchException(): std::invalid_argument("invalid verbose level") {}

void Logger::log_error(string const &message)
{
    std::fstream log_file;

    log_file.open("logs/test_log.log", std::ios_base::app);

    log_file << "[ERROR] " << message << endl;
    cout << BOLDRED << "[ERROR] " << NC << message << endl;

    log_file.close();

}

void Logger::log_warning(string const &message)
{
    std::fstream log_file;

    log_file.open("logs/test_log.log", std::ios_base::app);

    log_file << "[WARNING] " << message << endl;
    if (VERBOSE <= WARNING)
        cout << BOLDMAGENTA << "[WARNING] " << NC << message << endl;

    log_file.close();

}

void Logger::log_info(string const &message)
{
    std::fstream log_file;

    log_file.open("logs/test_log.log", std::ios_base::app);

    log_file << "[INFO] " << message << endl;
    if (VERBOSE <= INFO)
        cout << BOLDGREEN << "[INFO] " << NC << message << endl;

    log_file.close();

}

void Logger::log(string const &message, levels level)
{

    switch (level)
    {
        case ERROR:
            log_error(message);
            break;
        case WARNING:
            log_warning(message);
            break;
        case INFO:
            log_info(message);
            break;
    }
}
