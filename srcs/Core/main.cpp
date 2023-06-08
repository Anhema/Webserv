#include "Logger.hpp"
#include "ServerHandler.hpp"


int main(void)
{
    try
    {
        ServerHandler server(3);
        server.mainLoop();
		getadd

    }
    catch (std::exception &e)
    {
        Logger::log(e.what(), ERROR);
    }

}
