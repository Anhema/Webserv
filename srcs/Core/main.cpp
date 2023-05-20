#include "Logger.hpp"
#include "Server.hpp"
#include "../Utilities/Utilities.hpp"

/* void Server::monitorConnection(const fd connection) {

    EV_SET(_serverEvents, connection, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, 0);

    if (kevent(_kq, _serverEvents, 1, NULL, 0, NULL) == -1) {

        stringstream ss;

        ss << "kevent failed to monitor fd:" << connection;
        throw (std::runtime_error(ss.str()));
    }


}

int Server::acceptClient() {

    const fd new_connection = accept(_socket_fd, (sockaddr *)&_socketAddress, (socklen_t *)&_socketAddress_len);
    if (new_connection == -1)
        throw (std::runtime_error("Failed to accept incoming connection"));

    this->monitorConnection(new_connection);
    stringstream ss;

    ss << "Accepted connection: " << new_connection;
    Logger::log(ss.str(), INFO);
    return (new_connection);
}

*/
/*void disconnectClient(const fd client) {

    EV_SET(_serverEvents, client, EVFILT_READ | EVFILT_WRITE, EV_DELETE, 0, 0, 0);
    close(client);
} */

/*eventLoop(int new_events)
{
    for (int i = 0; i < new_events; i++)
    {
        const fd event_fd = _serverEvents[i].ident;
        if (_serverEvents[i].flags & EV_ERROR)
        {
            Logger::log("EV_FLAG", ERROR);
            continue;
        }
        if (event_fd == _socket_fd)
        {
            this->acceptClient();
            continue;
        }
        if (_serverEvents[i].flags & EV_EOF)
            this->disconnectClient(event_fd);
        if (_serverEvents[i].filter & EVFILT_READ)
            this->_message.request(event_fd, _serverEvents[i].data);
        if (_serverEvents[i].filter &EVFILT_WRITE)
            this->_message.response(event_fd);
    }
} */


void startServers(int starting_ip,  std::vector<Server *> &server_list, int n)
{
        for (int i = 0; i < n; i++)
        {
            try
            {
                stringstream name;
                name << "Server " << i;
                server_list.push_back(new Server("0.0.0.0", starting_ip + i, name.str()));
            }
            catch (std::exception &e)
            {
                if (starting_ip > 8090)
                {
                    Logger::log("Max ip reached", ERROR);
                    exit(0);
                }
                Logger::log(e.what(), ERROR);
                startServers(starting_ip + n, server_list, n);
            }

        }
}


int main(void)
{
    std::vector<Server *>server_list;
    std::unique_ptr<struct kevent[]>socketEvents;
    struct kevent events[Server::maxEvents];


    startServers(8080, server_list, 3);

    socketEvents.reset(new struct kevent[server_list.size()]);
    int kq = kqueue();

    for (size_t i = 0; i < server_list.size(); i++)
    {
        cout << server_list.at(i)->getName() << " with socket: " << server_list.at(i)->getSocket() << "\n";

    }

    for (size_t i = 0; i < server_list.size(); i++)
    {
        EV_SET(socketEvents.get(), server_list.at(i)->getSocket(), EVFILT_READ, EV_ADD, 0, 0, 0);
        cout << "EV set -> " << server_list.at(i)->getSocket() << "\n";
        kevent(kq, socketEvents.get(), 1, NULL, 0, NULL);
        cout << "Waiting socket -> " << i << "\n";
    }

    for (;;)
    {
        int new_events = kevent(kq, NULL, 0, events, Server::maxEvents, NULL);

//        cout << "Enters!\n";
        if (new_events == -1)
            throw (std::runtime_error("error retrieving new events"));

        for (int i = 0; i < new_events; i++)
        {
            const fd event_fd = events[i].ident;

            if (events[i].flags & EV_ERROR)
            {
                Logger::log("EV_FLAG", ERROR);
                continue;
            }
            for (std::vector<Server *>::iterator it = server_list.begin(); it != server_list.end(); it++)
            {
                if ((*it)->getSocket() == event_fd)
                {
                    (*it)->acceptClient(events, kq);
                    continue;
                }
            }
            if (events[i].flags & EV_EOF)
                close(event_fd);
            if (events[i].filter & EVFILT_READ)
                server_list.at(0)->message.request(event_fd, events[i].data);
            if (events[i].filter &EVFILT_WRITE)
                server_list.at(0)->message.response(event_fd);
        }
    }

    for (std::vector<Server *>::iterator it = server_list.begin(); it != server_list.end(); it++)
        delete *it;

}
