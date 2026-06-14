#include "../headers/Client.hpp"
#include "../headers/channel.hpp"
Client::Client(int fd){
    this->UserName = "";
    this->Nakename = "";
    this->Realname = "";
    this->host_ip = "" ;
    this->IsRegister = 0;
    this->Id = fd ;
    this->f_pass = 0 ;
    this->f_user = 0 ;
    this->f_nake = 0 ;
    this->f_close = 0 ;
}
Client::Client(){

    this->UserName = "";
    this->Nakename = "";
    this->Realname = "";
    this->host_ip = "" ;
    this->IsRegister = 0 ;
    this->Id = -1 ;
    this->f_pass = 0;
    this->f_user = 0;
    this->f_nake = 0;
    this->f_close = 0;
}

std::string Client::get_user()const {
    return this->UserName ;
}
std::string Client::get_nake()const{
    return this->Nakename ;
}
std::string Client::get_Realname() const{
    return this->Realname ;
}
std::string Client::get_host ()const{
    return this->host_ip ;
}

std::string & Client::getRecvBuffer(){
    return this->buffrev ;
}
std::string & Client::getSendBuffer(){
    return this->buffsend ;
}  
bool Client::get_Isregister()  {
    return this->IsRegister;
}

void Client::set_Isregister(int a)  {
    this->IsRegister = a;
}

int Client::get_Id() const {
    return this->Id ;
}

bool Client::get_fPaa() const{
    return this->f_pass ;
}
bool Client::get_fUser() const{
    return this->f_user ;
}
bool Client::get_fNake() const{
    return this->f_nake ;
}

void Client::set_fPaa(const int a) {
    this->f_pass = a ;
}
void Client::set_fUser(const int a) {
    this->f_user = a ; 
}
void Client::set_fNake(const int a) {
    this->f_nake = a ;
}

void Client::set_Close(const int a) {
    this->f_close = a ;
}

int Client::get_Close() const {
       return this->f_close  ;
}

void Client::set_user(const std::string & str) {
    this->UserName = str;
}
void Client::set_nake(const std::string & str){
     this->Nakename  = str;
}

void Client::set_host(const std::string & str){
     this->host_ip  = str;
}
   
void Client::set_Realname(const std::string & str){
     this->Realname  = str;
}
void Client::set_Id(const int a)  {
    this->Id = a ;
}

std::map<std::string , chanel *> & Client::chaneel_clieent() {
    return this->chanel_client ;
}
void Client::addClientToMaps(std::string name , chanel &channel) {
    this->chanel_client[name] = &channel ;
}