#include "Logger.hpp"


int main(void)
{
	Logger::log("test", ERROR);
	Logger::log("test2", INFO);
	Logger::log("test3", WARNING);

	//Logger::log("test", ERROR);
}
