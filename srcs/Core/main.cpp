#include "Logger.hpp"
#include "Server.hpp"
#include "ServerHandler.hpp"


int main(void)
{
    try
    {
        ServerHandler server(1);
        server.mainLoop();

    }
    catch (std::exception &e)
    {
        Logger::log(e.what(), ERROR);
    }

}
