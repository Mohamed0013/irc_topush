#pragma once
#include <iostream>
#include <map>
#include "Client.hpp"
#include <set>
class chanel{

    private :
        std::string name_chanel ;
        std::string pass_chanel ;
        std::string topic_chenal;
        bool invitation;
        bool cheak_pass; // pass 
        bool operatorr;
        bool topic;
        int  limits;

        std::map <int , Client *> all_Cchanel;
        std::map <int , Client *> op_chanel; // ga3 les admin dyal had chanel ;
        std::set <std::string> wait_list ;
    public :
        chanel();
        void set_pass(const std::string & str) ;
        void set_name(std::string & str) ;
        void add_CTOchanel(Client & client) ;

        

        std::map <int , Client *>& get_opChanel();
        std::map <int , Client *>& get_All_Cchanel();
        const std::map <int , Client *>& get_All_Cchanel() const;
        std::set <std::string> &get_list() ; 

        bool get_password() const ;
        bool get_invetation() const ;
        bool get_islimite() const ;
        bool cheak_passw() const;
        bool is_topic() const ;
        bool flag_topic() const ;
        void set_flag_topic(const int a) ;
        int  get_limitchanell() const ;

        void set_invetation(int a)  ;
        void set_limit( int a) ;

        bool is_member(int fd ) ;
        bool is_operator(int fd ) ;

        std::string get_passwordChanel() const ;
        std::string get_namechanel() const ;

        std::string get_topic_chanel() const ;
        void set_topic_chanel(const std::string str) ;    
};