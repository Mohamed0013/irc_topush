#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"

#include <vector>
#include <map>
#include <poll.h>
#include <string>

#include <iostream>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <csignal>
#include <cstdlib>
#include <errno.h>

class execute;
class chanel;
class Server
{
private:
    int _port;
    std::string _pass;

    int _serverFd;
    int _epollFd;
    struct epoll_event _events[64];

    std::map<int, Client> _clients;           // all Client
    std::map<std::string, execute *> exCmd;   // all cmd
    std::map<std::string, chanel> All_chanel; // all chanel

    static volatile sig_atomic_t _running;

    void initCmd();
    void initSocket();
    void acceptNewClient();
    void handleClientRead(int fd);
    void handleClientWrite(int fd);
    void parseInput(int clientFd, const std::string &raw_command);
    void modifyEpollState(int fd, uint32_t state);
    void removeClient(int fd);
    static void signalHandler(int sig);

public:
    std::map<int, Client> &get_mapClient();
    const std::string &get_pass() const;
    int get_fdeppol() const;
    void setupSignals();

    Server(int port, std::string pas);
    ~Server();
    void run();

    void cretionChanel(std::string name, std::string pass, Client &client);
    std::map<std::string, chanel> &get_Chanel();
};

#include "channel.hpp"
#endif