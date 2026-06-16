#include "../headers/channel.hpp"
#include <climits>
chanel::chanel(){
    this->name_chanel = "" ;
    this->pass_chanel = "" ;
    this->topic_chenal ="" ;
    // Mood : => :
    this->invitation = 0;
    this->topic = 0;
    this->operatorr = 0;
    this->limits = INT_MAX;
    this->cheak_pass = 0; // pass 
}


std::string chanel::get_topic_chanel() const {
    return this->topic_chenal ;
}
void chanel::set_topic_chanel(const std::string str) {
    this->topic_chenal = str ;
}

void chanel::set_pass(const std::string & str ){
    if(str.empty()){
        this->pass_chanel = "" ;
        this->cheak_pass = 0 ;
        return ;
    }
    this->pass_chanel = str ;
    this->cheak_pass = 1 ;
}

bool chanel::flag_topic() const {
    return this->topic ;
}
void chanel::set_flag_topic(const int a)  {
     this->topic = a;
}
bool chanel::get_invetation() const{
    return this->invitation;
}

bool chanel::get_islimite() const{
    if(this->limits == INT_MAX){
        return 0;
    }
    return 1;
} 
bool chanel::cheak_passw() const{
    if(this->pass_chanel.empty()){
        return 0;
    }
    return 1;
}

bool chanel::is_topic() const {
    if(this->topic_chenal.empty()){
        return 0;
    }
    return 1 ;
}   

void chanel::set_name(std::string & str ){
    this->name_chanel = str ;
}
void chanel::add_CTOchanel(Client & client){
    this->all_Cchanel[client.get_Id()] = &client ;
}

std::string chanel::get_namechanel() const {
    return this->name_chanel ;
}

std::map <int , Client *>& chanel::get_opChanel(){
    return this->op_chanel ;
}

std::map <int , Client *>& chanel::get_All_Cchanel(){
    return this->all_Cchanel ;
}

std::string chanel::get_passwordChanel() const{
    return this->pass_chanel ;
}
bool chanel::get_password() const{
    return this->cheak_pass;
}
int  chanel::get_limitchanell() const {
    return this->limits ;
}   
void  chanel::set_limit( int a)  {
    this->limits = a ;
}


void chanel::set_invetation(int a) {
     this->invitation = a;
}
bool chanel::is_member(int fd )  {
    std::map <int , Client *> :: iterator it ;
    it = this->all_Cchanel.find(fd);  
    if(it != this->all_Cchanel.end()){
        return true;
    }
    return false;
};

std::set <std::string> &chanel::get_list() {
    return this->wait_list;
}


bool chanel::is_operator(int fd )  {
    std::map <int , Client *> :: iterator it ;
    it = this->op_chanel.find(fd);  
    if(it != this->op_chanel.end()){
        return true;
    }
    return false;
};