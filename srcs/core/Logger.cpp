#include "Logger.hpp"


Logger::Logger() {}

Logger::Logger(const Logger __unused &obj) {}

Logger::~Logger() {}

Logger::VerboseNoMatchException::VerboseNoMatchException(): std::invalid_argument("invalid verbose level") {}

Logger::levels Logger::get_level() {

    switch (VERBOSE) {
        case ERROR:
            return (ERROR);
            break;
        case WARNING:
            return (WARNING);
            break;
        case INFO:
            return (INFO);
            break;
        default:
            throw (Logger::VerboseNoMatchException());

    }
}
