#pragma once

#include <vector>
#include <map>

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

class chanel ;
class Client{
    private :
        std::string UserName ;
        std::string nickname ;
        std::string buffrev ;
        std::string buffsend ;
        std::string Realname ;

        std::string host_ip ;
        int IsRegister ;
        int Id ;
        int f_pass ;
        int f_user ;
        int f_nick ;
        int f_close ;
        int flag_send ;

        std::map <std::string , chanel *> chanel_client ; // chanell ta3 had clietn ;
    public :
        Client() ;
        Client(int fd) ;

        std::string get_user()  const;
        std::string get_nick()  const;
        std::string get_Realname() const;
        std::string get_host() const ;

        std::string& getRecvBuffer()   ;
        std::string& getSendBuffer()   ;
        
        bool get_Isregister() ;
        int get_Id() const ;
        int get_flagMsg() const;
        void set_flagMsg(int a);

        bool get_fPaa() const;
        bool get_fUser() const;
        bool get_fnick() const;

        void set_user(const  std::string & str) ;
        void set_nick(const std::string & str) ;
        void set_Realname(const std::string & str) ;

        void set_buff(const std::string & str) ;

        void set_host(const std::string &str ) ;

        void set_Isregister(const int a)  ;
        void set_Id(const int a)  ;

        int  get_Close() const  ;
        void set_Close(const int a)  ;

        void set_fPaa(const int a) ;
        void set_fUser(const int a) ;
        void set_fnick(const int a) ;

        void clearSendBuffer(size_t len) { buffsend.erase(0, len);}

        std::map<std::string , chanel *> & chaneel_clieent() ;
        void addClientToMaps(std::string name , chanel &chanell) ;

}; 

#include "channel.hpp"