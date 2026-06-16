#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <ctime>

class Client;
class Server;

class Bot {
private:
    std::string _nick;
    std::string _user;
    std::string _host;
    std::vector<std::string> _facts;
    std::vector<std::string> _jokes;

    void initFacts();
    void initJokes();
    std::string getTimeResponse() const;
    std::string getDateResponse() const;
    std::string getHelp() const;
    std::string getInfoResponse(const Server& server) const;
    std::string getFortune() const;
    std::string getJoke() const;
    std::string processCommand(const std::string& cmd, const std::vector<std::string>& args, const Server& server) const;

public:
    Bot();
    ~Bot();

    const std::string& getNick() const;
    const std::string& getUser() const;
    const std::string& getHost() const;

    std::string evaluateExpr(const std::string& expr) const;
    std::string handleMessage(const std::string& message, const Server& server) const;
    std::string handleDirectCommand(const std::string& cmd, const std::vector<std::string>& args, const Server& server) const;
};

#endif