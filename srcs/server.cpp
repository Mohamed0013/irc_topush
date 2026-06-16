#include "../headers/server.hpp"
#include "execute.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cctype>

volatile sig_atomic_t Server::_running = 1;

void Server::signalHandler(int sig)
{
    (void)sig;
    _running = 0;
}

void Server::initCmd()
{
    this->exCmd["PASS"] = new executePass();
    this->exCmd["NICK"] = new executeNick();
    this->exCmd["USER"] = new executeUser();
    this->exCmd["QUIT"] = new executeQuit();
    this->exCmd["PRIVMSG"] = new executePrivmsg();
    this->exCmd["JOIN"] = new executeJoin();
    this->exCmd["KICK"] = new executeKick();
    this->exCmd["INVITE"] = new executeInvite();
    this->exCmd["MODE"] = new executeMode();
    this->exCmd["TOPIC"] = new executeTopic();
}

std::map<int, Client> &Server::get_mapClient()
{
    return this->_clients;
}

const std::string &Server::get_pass() const
{
    return this->_pass;
}

int Server::get_fdeppol() const
{
    return this->_epollFd;
}

void Server::cretionChanel(std::string name, std::string pass, Client &client)
{
    this->All_chanel[name] = chanel();
    this->All_chanel[name].set_pass(pass);
    this->All_chanel[name].set_name(name);
    this->All_chanel[name].add_CTOchanel(client);
    client.addClientToMaps(name, this->All_chanel[name]);
    std::cout << "Chane a ete bien creat " << std::endl;
}

std::map<std::string, chanel> &Server::get_Chanel()
{
    return this->All_chanel;
}

Server::Server(int port, std::string pas) : _port(port), _pass(pas), _serverFd(-1), _epollFd(-1)
{
    struct sigaction sa;
    sa.sa_handler = Server::signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    initCmd();
    initSocket();
}

Server::~Server()
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        close(it->first);
    }
    close(_serverFd);
    close(_epollFd);

    for (std::map<std::string, execute *>::iterator it = exCmd.begin(); it != exCmd.end(); ++it)
    {
        delete it->second;
    }
}

void Server::initSocket()
{
    // Step 1: Create TCP socket
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
        throw std::runtime_error("Socket creation failed");

    // Step 2: Set SO_REUSEADDR option
    int opt = 1;
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Step 3: Set socket to non-blocking mode
    if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Fcntl failed");

    // Step 4: Prepare socket address structure
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);

    // Step 5: Bind socket to address/port
    if (bind(_serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Bind failed");

    // Step 6: Start listening for connections
    if (listen(_serverFd, SOMAXCONN) < 0)
        throw std::runtime_error("Listen failed");

    // Step 7: Create epoll instance
    _epollFd = epoll_create(1024);
    if (_epollFd < 0)
        throw std::runtime_error("Epoll creation failed");

    // Step 8: Add server socket to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = _serverFd;
    epoll_ctl(_epollFd, EPOLL_CTL_ADD, _serverFd, &ev);

    std::cout << "[+] Server started on port " << _port << " with epoll" << std::endl;
}

void Server::modifyEpollState(int fd, uint32_t state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev);
}

void Server::removeClient(int fd)
{
    std::map<std::string, chanel *> &clientChannels = _clients[fd].chaneel_clieent();
    std::map<std::string, chanel *>::iterator chIt;
    for (chIt = clientChannels.begin(); chIt != clientChannels.end(); ++chIt)
    {
        chIt->second->get_All_Cchanel().erase(fd);
        chIt->second->get_opChanel().erase(fd);
        if (chIt->second->get_All_Cchanel().empty())
        {
            std::string name = chIt->second->get_namechanel();
            // Clean up this channel reference from ALL other clients
            std::map<int, Client>::iterator cit;
            for (cit = _clients.begin(); cit != _clients.end(); ++cit)
            {
                if (cit->first != fd)
                    cit->second.chaneel_clieent().erase(name);
            }
            All_chanel.erase(name);
        }
    }
    clientChannels.clear();

    epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clients.erase(fd);
}

void Server::acceptNewClient()
{
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    int clientFd = accept(_serverFd, (struct sockaddr *)&cli_addr, &cli_len);
    if (clientFd < 0)
        return;

    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
    {
        close(clientFd);
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &ev) < 0)
    {
        close(clientFd);
        return;
    }

    std::string client_ip = inet_ntoa(cli_addr.sin_addr);
    _clients[clientFd] = Client(clientFd);
    _clients[clientFd].set_Id(clientFd);
    _clients[clientFd].set_host(client_ip);
    std::cout << "[NETWORK] New client connected. FD: " << clientFd << std::endl;
}

void Server::handleClientRead(int fd)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0)
    {
        std::cout << "[NETWORK] Client disconnected. FD: " << fd << std::endl;
        removeClient(fd);
        return;
    }

    if (_clients[fd].getRecvBuffer().length() + bytes > 4096)
    {
        removeClient(fd);
        return;
    }

    _clients[fd].getRecvBuffer().append(buffer, bytes);

    size_t pos;
    while ((pos = _clients[fd].getRecvBuffer().find('\n')) != std::string::npos)
    {
        std::string full_command = _clients[fd].getRecvBuffer().substr(0, pos);
        // Strip trailing \r if present
        if (!full_command.empty() && full_command[full_command.length() - 1] == '\r')
            full_command.erase(full_command.length() - 1);

        _clients[fd].getRecvBuffer().erase(0, pos + 1);

        if (!full_command.empty())
            parseInput(fd, full_command);

        if (_clients.find(fd) == _clients.end())
            return;
        if (this->_clients[fd].get_Close())
        {
            removeClient(fd);
            return;
        }
    }
}

void Server::parseInput(int clientFd, const std::string &raw_command)
{
    std::string cmd_name;
    std::vector<std::string> args;

    // Find command name
    size_t i = 0;
    // Skip leading spaces
    while (i < raw_command.size() && raw_command[i] == ' ')
        i++;
    size_t cmd_start = i;
    while (i < raw_command.size() && raw_command[i] != ' ')
        i++;
    cmd_name = raw_command.substr(cmd_start, i - cmd_start);

    if (cmd_name.empty()) return;

    // Convert to uppercase for lookup
    for (size_t j = 0; j < cmd_name.size(); j++)
        cmd_name[j] = toupper(cmd_name[j]);

    // Parse parameters
    while (i < raw_command.size())
    {
        // Skip spaces
        while (i < raw_command.size() && raw_command[i] == ' ')
            i++;
        if (i >= raw_command.size()) break;

        // Trailing parameter (starts with ':')
        if (raw_command[i] == ':')
        {
            args.push_back(raw_command.substr(i + 1));
            break;
        }

        // Regular parameter
        size_t arg_start = i;
        while (i < raw_command.size() && raw_command[i] != ' ')
            i++;
        args.push_back(raw_command.substr(arg_start, i - arg_start));
    }

    std::cout << "[LOGIC] Executing CMD: " << cmd_name << " arg" << args.size() << std::endl;

    if (exCmd.find(cmd_name) != exCmd.end())
    {
        exCmd[cmd_name]->executeCmd(*this, this->_clients[clientFd], args);

        if (!_clients[clientFd].getSendBuffer().empty())
        {
            if (_clients[clientFd].getSendBuffer().size() > 65536)
            {
                removeClient(clientFd);
                return;
            }
            modifyEpollState(clientFd, EPOLLIN | EPOLLOUT);
        }
    }
    else
    {
        this->_clients[clientFd].getSendBuffer() += "Unknown command.\n";
        if (_clients[clientFd].getSendBuffer().size() > 65536)
        {
            removeClient(clientFd);
            return;
        }
        modifyEpollState(clientFd, EPOLLIN | EPOLLOUT);
    }
}

void Server::handleClientWrite(int fd)
{
    std::string &response = _clients[fd].getSendBuffer();
    if (response.empty())
        return;

    ssize_t sent = send(fd, response.c_str(), response.length(), MSG_NOSIGNAL);
    if (sent < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // Socket buffer full — keep client, try again later
            return;
        }
        // Actual error — disconnect
        removeClient(fd);
        return;
    }
    if (sent == 0)
    {
        removeClient(fd);
        return;
    }

    _clients[fd].clearSendBuffer(sent);

    if (_clients[fd].getSendBuffer().empty())
        modifyEpollState(fd, EPOLLIN);
}

void Server::run()
{
    while (_running)
    {
        int nfds = epoll_wait(_epollFd, _events, 64, 100);
        if (nfds < 0)
            break;

        for (int i = 0; i < nfds; ++i)
        {
            int current_fd = _events[i].data.fd;

            if (current_fd == _serverFd)
            {
                acceptNewClient();
            }
            else
            {
                if (_events[i].events & EPOLLIN)
                {
                    handleClientRead(current_fd);
                    if (_clients.find(current_fd) == _clients.end())
                        continue;
                }
                if (_events[i].events & EPOLLOUT)
                    handleClientWrite(current_fd);
            }
        }
    }

    std::cout << "[+] Server shutting down gracefully" << std::endl;
}
