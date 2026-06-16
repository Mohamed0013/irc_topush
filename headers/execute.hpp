#pragma once

#include <iostream>
#include "server.hpp"
class execute {
    public :
        virtual void executeCmd(Server& server, Client& client, const std::vector<std::string>& args) = 0;
        virtual ~execute(){};
};


class executePass : public execute {
    public :
        void executeCmd(Server& server, Client& client, const std::vector<std::string>& args) ;
        virtual ~executePass(){};
};


class executeNick : public execute {
    public :
        void executeCmd(Server& server, Client& client, const std::vector<std::string>& args) ;
        virtual ~executeNick(){};
};


class executeUser : public execute {
    public :
         void executeCmd(Server& server, Client& client, const std::vector<std::string>& args);
        virtual ~executeUser(){};
};

class executePrivmsg : public execute {
    public :
        void executeCmd(Server& server, Client& client, const std::vector<std::string>& args);
        virtual ~executePrivmsg(){};
};

class executeJoin : public execute {
    public :
        void executeCmd(Server& server, Client& client, const std::vector<std::string>& args);
        virtual ~executeJoin(){};
};

class executeKick : public execute {
    public :
        void executeCmd(Server& server, Client& client, const std::vector<std::string>& args);
        virtual ~executeKick(){};
};

class executeInvite : public execute {
    public :
        void executeCmd(Server& server, Client& client, const std::vector<std::string>& args);
        virtual ~executeInvite(){};
};

class executeMode : public execute {
    public :
        void executeCmd(Server& server, Client& client, const std::vector<std::string>& args);
        virtual ~executeMode(){};
};


class executeTopic : public execute {
    public :
        void executeCmd(Server& server, Client& client, const std::vector<std::string>& args);
        virtual ~executeTopic(){};
};