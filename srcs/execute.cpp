#include "../headers/execute.hpp"


#include "execute.hpp"


void executePass::executeCmd(Server& server, Client& client, const std::vector<std::string>& args) {
    if (client.get_Isregister()) {
        client.getSendBuffer() += ":ft_irc_default 462 " + client.get_nake() + " :Unauthorized command (already registered)\r\n";
        return; 
    }
    if (client.get_fPaa()) {
        std::string str = client.get_nake().empty() ? "*" : client.get_nake();
        client.getSendBuffer() += ":ft_irc_default 462 " + str + " :Unauthorized command (already registered)\r\n";
        return;
    }
    if (args.empty()) {
        std::string str = client.get_nake().empty() ? "*" : client.get_nake();
        client.getSendBuffer() += ":ft_irc_default 461 " + str + " PASS :Not enough parameters\r\n"; 
        return;
    }
    if (server.get_pass() != args[0]) {
        std::string str = client.get_nake().empty() ? "*" : client.get_nake();
        client.getSendBuffer() += ":ft_irc_default 464 " + str + " :Password incorrect\r\n";
        client.set_Close(1);
        return ;
    }
    else {
        client.set_fPaa(true);
    }
}

void executeNick::executeCmd(Server& server, Client& client, const std::vector<std::string>& args) {
    if(client.get_fPaa() == 0){
        client.getSendBuffer() += "451 * :You have not registered\r\n";
        return ;
    }
    if (args.empty()) {
        std::string str = client.get_nake().empty() ? "*" : client.get_nake() ;
        client.getSendBuffer() += "431 " + str +" Not enough parameters\r\n"; 
        return;
    }
    if (args[0][0] == '#' || args[0][0] == '&' || args[0][0] == ':') {
        std::string str = client.get_nake().empty() ? "*" : client.get_nake();
        client.getSendBuffer() += "432 " + str + " " + args[0] + " :Erroneous nickname\r\n";
        return;
    }
    if(client.get_fNake() && client.get_nake() == args[0]){
        return ;
    }
    else{
        std::map < int , Client>::iterator temp ;
        for (temp =  server.get_mapClient().begin() ; temp!=server.get_mapClient().end() ; ++temp ){
            if(temp->second.get_nake() == args[0]){
                client.getSendBuffer() += "433 * " + args[0] + " :Nickname is already in use\r\n";
                return ;
            }
        }
        std::string old_nick = client.get_nake() ; 
        client.set_nake(args[0]);
        client.set_fNake(true);
        if(client.get_Isregister() == true){
            client.getSendBuffer() += ":" + old_nick + " NICK :" + args[0] + "\r\n";
            return ;
        }
        if(client.get_fUser() && client.get_fNake() && client.get_fPaa()){
            client.set_Isregister(true);
            client.getSendBuffer() += "001 " + args[0] + " :Welcome to the Internet Relay Network " + args[0] + "\r\n";
            return ;
        }
    }
}
void executeUser::executeCmd(Server& server, Client& client, const std::vector<std::string>& args) {
    if(client.get_fPaa() == 0){
        client.getSendBuffer() += "451 * :You have not registered\r\n";
        return ;
    }
    if (args.empty()) {
        std::string str = client.get_nake().empty() ? "*" : client.get_nake() ;
        client.getSendBuffer() += "431 " + str +" Not enough parameters\r\n"; 
        return;
    }
    if(args.size() < 4){
          std::string str = client.get_nake().empty() ? "*" : client.get_nake() ;
        client.getSendBuffer() += "461 " + str + " USER :Not enough parameters\r\n";
        return;
    }
    if(client.get_fUser() || client.get_Isregister()){
        std::string prefix = client.get_nake().empty() ? "*" : client.get_nake();
        client.getSendBuffer() += "462 " + prefix + " :Unauthorized command (already registered)\r\n";
        return;
    }else {
        client.set_user(args[0]);
        client.set_Realname(args[3]);
        client.set_fUser(true);
        if(client.get_fUser() && client.get_fNake() && client.get_fPaa()){
            client.set_Isregister(true);
            client.getSendBuffer() += "001 " + args[0] + " :Welcome to the Internet Relay Network " + args[0] + "\r\n";
            return ;
        }
    }
}

void executeQuit::executeCmd(Server& server, Client& client, const std::vector<std::string>& args) {
    
    client.set_Close(1);
    client.getRecvBuffer().clear();
    
}

std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss;
    ss << str.c_str() ;
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}
void executePrivmsg::executeCmd(Server& server, Client& client, const std::vector<std::string>& args) {
    if(client.get_Isregister() == false){
        std::string str = client.get_nake().empty() ? "*" : client.get_nake() ;
        client.getSendBuffer() += ":localhost 451 " + str + " :You have not registered\r\n";
        return ;
    }
    if(args.empty()){
        client.getSendBuffer() += ":localhost 411 " + client.get_nake() + " :No recipient given (PRIVMSG)\r\n";
        return ;
    }
    if(args.size() < 2 || args[1].empty()){
        client.getSendBuffer() += ":localhost 412 " + client.get_nake() + " :No text to send\r\n";
        return;
    }
    std::vector <std::string > newarg  = split(args[0] , ','); // arg[0]
    if(newarg.size() > 5){
        client.getSendBuffer() += ":localhost 407  :Too many recipients. Message not delivered.\r\n";
        return ;
    }
    int i = 0  , p;
    while (i < newarg.size())
    {
        p = 0;
        if(newarg[i][0] == '#' || newarg[i][0] == '&'){
            std::cout << "1 - ana hna" << std::endl;
            std::string packet = ":" + client.get_nake() + "!" + client.get_user() + "@"+ client.get_host()+" PRIVMSG " + newarg[i] + " :" + args[1] + "\r\n";
            std::map <std::string, chanel > :: iterator it  = server.get_Chanel().find(newarg[i]);
            if(it != server.get_Chanel().end()){
                std::cout << "2 - ana hna" << std::endl;
                if(it->second.is_member(client.get_Id()) == 0){
                    client.getSendBuffer() += ":localhost 404 " + client.get_nake() + " " + args[0] + " :Cannot send to channel\r\n";
                    return ;
                }
                std::map < int, Client * > :: iterator itt ;
                for(itt = it->second.get_All_Cchanel().begin() ; itt != it->second.get_All_Cchanel().end() ;++itt){
                    if (itt->first == client.get_Id()) {
                        continue; 
                    }
                    p = 1 ;
                    itt->second->getSendBuffer() += packet;
                    struct epoll_event ev;   // cheak 
                    ev.events = EPOLLIN | EPOLLOUT;  // cheak
                    ev.data.fd = itt->first;     // cheak
                    epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, itt->first, &ev); // cheak
                }
            }  
        }
        else
        {
            std::cout << "3 - ana hna" << std::endl;
            std::map<int , Client> :: iterator it ;
            for(it = server.get_mapClient().begin() ; it != server.get_mapClient().end()  ;++it){
                if(it->second.get_nake() == newarg[i]){
                    p = 1 ;
                    std::string packet = ":" + client.get_nake() + "!" + client.get_user() + "@"+ client.get_host()+" PRIVMSG " + newarg[i] + " :" + args[1] + "\r\n";
                    it->second.getSendBuffer() += packet;
                    struct epoll_event ev;   // cheak 
                    ev.events = EPOLLIN | EPOLLOUT;  // cheak
                    ev.data.fd = it->first;     // cheak
                    epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, it->first, &ev); // cheak
                    break;        
                }
            }
        }
        if(p == 0) {
            client.getSendBuffer() += ":localhost 401 " + client.get_nake() + " " + newarg[i] + " :No such nick/channel\r\n";
        }
        i++ ;
    }
    return;       
}
//  cmd     arg[0]                    arg[1]
/// JOIN    #room1,romm2,room3         PAS
void executeJoin::executeCmd(Server& server, Client& client, const std::vector<std::string>& args) {    
    if(client.get_Isregister() == false){
        std::string str = client.get_nake().empty() ? "*" : client.get_nake() ;
        client.getSendBuffer() += ":localhost 451 " + str + " :You have not registered\r\n";
        return ;
    }
    if(args.empty() || args[0].empty()){
        client.getSendBuffer() += ":localhost 461 " + client.get_nake() + " JOIN :Not enough parameters\r\n";
        return ;
    }
    std::vector <std::string > newarg  = split(args[0] , ',');
    std::vector <std::string > newPas ;
    if(args.size() == 1){
        newPas.clear() ;
    } else {
        newPas = split(args[1] , ','); 
    }
    int  i = 0 ;
    while (i < newarg.size())
    {
        if(newarg[i].size() < 2 || (newarg[i][0] != '#' && newarg[i][0] != '&')){
            client.getSendBuffer() += ":localhost 403 " + client.get_nake() + " " + newarg[i] + " :No such channel\r\n";
            i++;
            continue;
        }
        std::map<std::string, chanel> ::iterator it = server.get_Chanel().find(newarg[i]);
        if(it == server.get_Chanel().end()){
            if(i < newPas.size()){
                server.cretionChanel(newarg[i],newPas[i],client);
            }else{
                server.cretionChanel(newarg[i],"",client);
            }
            server.get_Chanel()[newarg[i]].get_opChanel()[client.get_Id()] = &client ;
            std::string packet = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " JOIN :" + newarg[i] + "\r\n";
            std::string msg1 = ":ft_irc_default 331 " + client.get_nake() + " " + newarg[i] + " :No topic is set\r\n";
            std::string msg2 = ":ft_irc_default 353 " + client.get_nake() + " = " + newarg[i] + " :@" + client.get_nake() + "\r\n";
            std::string msg3 = ":ft_irc_default 366 " + client.get_nake() + " " + newarg[i] + " :End of /NAMES list.\r\n";
            client.getSendBuffer() += packet + msg1 + msg2 + msg3;
            i++;
        }   
        else {
            std::stringstream buff ;
            std::map<std::string, chanel> ::iterator iter = server.get_Chanel().find(newarg[i]);
            if(iter->second.is_member(client.get_Id()) == 0){
                std::set <std::string> ::iterator it_liste = iter->second.get_list().find(client.get_nake());
                if(iter->second.get_invetation()){
                    if(it_liste == iter->second.get_list().end()){
                        std::string msg = ":ft_irc_default 473 " + client.get_nake() + " " + iter->first + " :Cannot join channel (+i)\r\n";
                        client.getSendBuffer() += msg;
                        return ;
                    }
                }
                if(iter->second.get_password()){
                    if( newPas.size() == 0 ||  newPas[i] != iter->second.get_passwordChanel()){
                        client.getSendBuffer() += ":ft_irc_default 475 " + client.get_nake() + 
                                                " " + iter->second.get_namechanel()+ " :Cannot join channel (+k)\r\n";
                        return ;
                    }
                }
                if(iter->second.get_islimite() && iter->second.get_All_Cchanel().size() >= iter->second.get_limitchanell()){
                    client.getSendBuffer()+= ":ft_irc_default 471 " + client.get_nake() + " " + iter->second.get_namechanel() 
                                        + " :Cannot join channel (+l)\r\n";
                    return ;
                }
                if(iter->second.get_invetation()){
                    iter->second.get_list().erase(it_liste);
                    if(iter->second.get_list().size() == 0){
                        iter->second.set_invetation(0);
                    }
                }
                iter->second.get_All_Cchanel()[client.get_Id()] = &client ;

                std::map<int, Client *> ::iterator index ;
                std::string join_msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() 
                                        + " JOIN :" + server.get_Chanel()[newarg[i]].get_namechanel() + "\r\n";
                for(index = iter->second.get_All_Cchanel().begin() ; index != iter->second.get_All_Cchanel().end() ; ++index){
                    index->second->getSendBuffer() += join_msg;
                    struct epoll_event ev;   // cheak 
                    ev.events = EPOLLIN | EPOLLOUT;  // cheak
                    ev.data.fd = index->first;     // cheak
                    epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, index->first, &ev); // cheak
                }
                if (iter->second.is_topic() == 0) {
                    client.getSendBuffer() += ":ft_irc_default 331 " + client.get_nake() + " " + iter->second.get_namechanel() + " :No topic is set\r\n";
                }else {
                    client.getSendBuffer() += ":ft_irc_default 332 " + client.get_nake() + " " + iter->second.get_namechanel() + " :" + iter->second.get_topic_chanel() + "\r\n";
                }
                buff << ":ft_irc_default 353 " << client.get_nake() << " = " << iter->second.get_namechanel() << " :";
                for(index = iter->second.get_All_Cchanel().begin() ; index != iter->second.get_All_Cchanel().end() ; ++index){
                    
                    if(server.get_Chanel()[newarg[i]].get_opChanel().find(index->second->get_Id()) != server.get_Chanel()[newarg[i]].get_opChanel().end() ){
                        buff << "@" ;
                    }
                    buff << index->second->get_nake() << " "; 
                }
                buff << "\r\n";
                std::string msg366 = ":ft_irc_default 366 " + client.get_nake() + " " + server.get_Chanel()[newarg[i]].get_namechanel() + " :End of /NAMES list.\r\n";
                client.getSendBuffer() += buff.str() ;
                client.getSendBuffer() +=  msg366 ;
            }
            i++;
        }

    }
}
//       arg[0]     arg[1]  orr
// KICK  <channel>  <user>  [<comment>]
 void executeKick::executeCmd(Server& server, Client& client, const std::vector<std::string>& args){
    if(client.get_Isregister() == false){
        std::string str = client.get_nake().empty() ? "*" : client.get_nake() ;
        client.getSendBuffer() += ":localhost 451 " + str + " :You have not registered\r\n";
        return ;
    }
    if(args.empty()){
        client.getSendBuffer() += ":localhost 411 " + client.get_nake() + " :No recipient given (KICK)\r\n";
        return ;
    }
    if(args.size() < 2 ){
        client.getSendBuffer() += ":localhost 412 " + client.get_nake() + " :No text to send\r\n";
        return;
    }
    std::vector <std::string> newchanel = split(args[0],',');
    std::vector <std::string> newUser  = split(args[1],',');
    if(newchanel.size() > 1 && newchanel.size() != newUser.size()){
        client.getSendBuffer() += ":localhost 461 " + client.get_nake() + " KICK :Syntax error, channel/user mismatch\r\n";
        return ;
    }
    if(newchanel.size() == 1 && newUser.size() >= 1) {
        std::cout << " Ana dakhol l hna  1 1 1 " <<  std::endl;
        if(newchanel[0][0] != '#' && newchanel[0][0] != '&'){
            client.getSendBuffer() += ":localhost 403 " + client.get_nake() + " " + newchanel[0] + " :No such channel\r\n";
            return ;
        }
        std::map <std::string, chanel> :: iterator it = server.get_Chanel().find(newchanel[0]);
        if(it != server.get_Chanel().end()){
            if(it->second.is_operator(client.get_Id()) == 0){
                client.getSendBuffer() += ":localhost 482 " + client.get_nake() + " " + newchanel[0] + " :You're not channel operator\r\n";
                return ;
            }
            std::map <int, Client *> ::iterator itt ;
            int i = 0 , p ;
            std::cout <<  newUser.size() << std::endl;
            while (i < newUser.size())
            {
                p = 0;
                for(itt = it->second.get_All_Cchanel().begin(); itt != it->second.get_All_Cchanel().end(); ++itt){
                    if(itt->second->get_nake() == newUser[i]){
                        p = 1;
                        std::string str = (args.size() > 2) ? args[2] : client.get_nake() ;
                        std::string packet = ":" + client.get_nake() + "!" +client.get_user()+ "@" +client.get_host()+ " KICK " + newchanel[0]+ " " +newUser[i]+" :"+str+"\r\n";               
                        std::map <int, Client *>::iterator broad_it;
                        for (broad_it = it->second.get_All_Cchanel().begin(); broad_it != it->second.get_All_Cchanel().end(); ++broad_it) {
                            broad_it->second->getSendBuffer() += packet;
                            struct epoll_event ev;   // cheak 
                            ev.events = EPOLLIN | EPOLLOUT;  // cheak
                            ev.data.fd = broad_it->first;     // cheak
                            epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, broad_it->first, &ev); // cheak
                        }
                        if(it->second.is_operator(itt->second->get_Id()) == 1){
                            it->second.get_opChanel().erase(itt->first);
                        }
                        it->second.get_All_Cchanel().erase(itt);
                        break;
                    }
                }
                if(p == 0){
                    client.getSendBuffer() += ":localhost 441 " + client.get_nake() + " " + newUser[i] + " " + args[0] + " :They aren't on that channel\r\n" ;
                }
                std::cout << " Ana dakhol l " << i << std::endl;
                i++ ;
            }
            if (it->second.get_All_Cchanel().empty()){
                server.get_Chanel().erase(it);
            }
        }else {
             client.getSendBuffer() += ":localhost 403 " + client.get_nake() + " " + newchanel[0] + " :No such channel\r\n";
        }

    }
//          arg[0]                        arg[1]                   orr
    // KICK  <channel><channel><channel>  <user> <user> <user>   [<comment>]
    else if(newchanel.size() == newUser.size()){
        int i = 0 , p;
        std::cout <<  newchanel.size() << std::endl;
        while (i  < newchanel.size())
        {
            if(newchanel[i][0] != '#' && newchanel[i][0] != '&'){
                client.getSendBuffer() += ":localhost 403 " + client.get_nake() + " " + newchanel[i] + " :No such channel\r\n";
                i++ ;
                continue;
            }
            std::map <std::string, chanel> :: iterator it = server.get_Chanel().find(newchanel[i]);
            if(it != server.get_Chanel().end())
            {
                if(it->second.is_operator(client.get_Id()) == 0){
                    client.getSendBuffer() += ":localhost 482 " + client.get_nake() + " " + newchanel[i] + " :You're not channel operator\r\n";
                    i++ ;
                    continue;
                }
                std::map <int, Client *> ::iterator itt ; p = 0 ;
                for(itt = it->second.get_All_Cchanel().begin() ; itt != it->second.get_All_Cchanel().end()  ; ++itt){
                    if(itt->second->get_nake() == newUser[i]){
                        std::string str = (args.size() > 2) ? args[2] : client.get_nake() ;
                        std::string packet = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + 
                                             " KICK " + newchanel[i] + " " + newUser[i] + " :" + str + "\r\n"; 
                        std::map <int, Client *> ::iterator it_send ;
                        for(it_send = it->second.get_All_Cchanel().begin() ; it_send !=it->second.get_All_Cchanel().end() ;++it_send){
                            it_send->second->getSendBuffer() += packet ; 
                            struct epoll_event ev;   // cheak 
                            ev.events = EPOLLIN | EPOLLOUT;  // cheak
                            ev.data.fd = it_send->first;     // cheak
                            epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, it_send->first, &ev); // cheak
                        }
                        p = 1;
                        if(it->second.is_operator(itt->second->get_Id()) == 1){
                            it->second.get_opChanel().erase(itt->first);
                        }
                        it->second.get_All_Cchanel().erase(itt);
                        break;
                    }
                }
                if(p == 0){
                    client.getSendBuffer() += ":localhost 441 " + client.get_nake() + " " + newUser[i] + " " + args[0] + 
                                                " :They aren't on that channel\r\n" ;
                }
                if (it->second.get_All_Cchanel().empty()){
                    server.get_Chanel().erase(it);
                }
            }
            else {
                client.getSendBuffer() += ":localhost 403 " + client.get_nake() + " " + newchanel[i] + " :No such channel\r\n";
            }
            i ++ ;
        }
        
    }
    
 }

 // client wa7ed : INVITE <nickname> <channel>
void executeInvite::executeCmd(Server& server, Client& client, const std::vector<std::string>& args){
    if(client.get_Isregister() == false){
        std::string str = client.get_nake().empty() ? "*" : client.get_nake() ;
        client.getSendBuffer() += ":localhost 451 " + str + " :You have not registered\r\n";
        return ;
    }
    if(args.empty()){
        client.getSendBuffer() += ":localhost 411 " + client.get_nake() + " :No recipient given (KICK)\r\n";
        return ;
    }
    if (args.size() < 2) {
        client.getSendBuffer() += ":localhost 461 " + client.get_nake() + " INVITE :Not enough parameters\r\n";
        return;
    }
    std::map <std::string, chanel> :: iterator it = server.get_Chanel().find(args[1]);
    if(it != server.get_Chanel().end()){
        if(it->second.is_member(client.get_Id()) == 0){
            client.getSendBuffer() += ":localhost 442 " + client.get_nake() + " " + args[1] + " :You're not on that channel\r\n";
            return;
        }
        if(it->second.is_operator(client.get_Id()) == 0){
            client.getSendBuffer() += ":localhost 482 " + client.get_nake() + " " + args[1] + " :You're not channel operator\r\n";
            return;
        }
        std::map <int , Client> :: iterator All_client ; int p = 0 ;
        for(All_client = server.get_mapClient().begin() ; All_client != server.get_mapClient().end() ; ++All_client){
            if(All_client->second.get_nake() == args[0]){
                p = 1;
                break;
            }
        }
        if(p == 0){
            client.getSendBuffer() += ":localhost 401 " + client.get_nake() + " " + args[0] + " :No such nick/channel\r\n";
            return;
        }
        std::map <int , Client*> :: iterator it_client ;
        for(it_client = it->second.get_All_Cchanel().begin() ;it_client !=it->second.get_All_Cchanel().end() ; ++it_client){
            if(it_client->second->get_nake() == args[0]){
                client.getSendBuffer() += ":localhost 443 " + client.get_nake() + " " + args[0] + " " + args[1] + " :is already on channel\r\n";
                return;
            }
        }
        it->second.get_list().insert(args[0]);
        std::string invite_packet = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " INVITE " + All_client->second.get_nake() + " :" + args[1] + "\r\n";
        All_client->second.getSendBuffer() += invite_packet;
        struct epoll_event ev;   
        ev.events = EPOLLIN | EPOLLOUT;  
        ev.data.fd = All_client->second.get_Id();     
        epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, All_client->second.get_Id(), &ev);
        client.getSendBuffer() += ":localhost 341 " + client.get_nake() + " " + All_client->second.get_nake() + " " + args[1] + "\r\n";
    }
    else{
        client.getSendBuffer() += ":localhost 403 " + client.get_nake() + " " + args[1] + " :No such channel\r\n";
          return;
    }
}
            // size == 5 
            // index  : 2         3     4
// MODE #zaza +litki-k secret123 khkjh  dfjkds
//       0       1
void execute_mode_k(Server& server,char &sig, Client& client, const std::vector<std::string>& args , int &i){
    if (sig  == 0){
        // all domme likha3lin n7athom 
        return ;
    }
    else{
        if(i  >= args.size()){
            return ;
        }
        std::map<std::string , chanel> ::iterator it_ch = server.get_Chanel().find(args[0]);
        std::string msg;
        int p = 0;
        if(sig == '+') {
            if(it_ch->second.cheak_passw() == 1){
                msg = ":localhost  467 " + client.get_nake() + " " + args[0] + " :Channel key already set\r\n";
                client.getSendBuffer() += msg;
                i++;
                return ;
            }
            else{
                it_ch->second.set_pass(args[i]);
                msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " MODE " + args[0] + " +k " + args[i] + "\r\n";
                i++;
                p = 1;            
            }
        }
        else if(sig == '-'){
            if(it_ch->second.cheak_passw() == 1){
                if(it_ch->second.get_passwordChanel() != args[i]){
                    i++;
                    return ;
                }
                else {
                    it_ch->second.set_pass("");
                    msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " MODE " + args[0] + " -k " + args[i] + "\r\n";
                    p = 1;
                    i++;
                }
            }else{
                i++;
            }
        }
        if (p == 1){
            std::map<int , Client*> ::iterator it ;
            for(it = it_ch->second.get_All_Cchanel().begin() ;it != it_ch->second.get_All_Cchanel().end() ;++it){
                it->second->getSendBuffer()+= msg ;
                struct epoll_event ev;   
                ev.events = EPOLLIN | EPOLLOUT;  
                ev.data.fd = it->second->get_Id();  
                epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, it->second->get_Id(), &ev);
            }
        }

    }
}
void execute_mode_i(Server& server,char &sig, Client& client, const std::vector<std::string>& args){
    if (sig  == 0 ){
        // all domme likha3lin n7athom 
        return ;
    }
    else {
        std::map<std::string , chanel> ::iterator it_ch = server.get_Chanel().find(args[0]);
        std::string msg ;
        std::string str(1, sig); 
        msg +=":" + client.get_nake() + "!" + client.get_user()+ "@" + client.get_host() + " MODE " + args[0] + " " + str + "i\r\n";
        int p = 0;
        if(sig == '+' && it_ch->second.get_invetation() == 0){
            it_ch->second.set_invetation(1);
            p = 1;
        }
        else if(sig == '-' && it_ch->second.get_invetation() == 1){
            it_ch->second.set_invetation(0);
            p = 1;
        }
        if(p == 1){
            std::map<int , Client*> ::iterator it ;
            for(it = it_ch->second.get_All_Cchanel().begin() ;it != it_ch->second.get_All_Cchanel().end() ;++it){
                it->second->getSendBuffer()+= msg ;
                struct epoll_event ev;   
                ev.events = EPOLLIN | EPOLLOUT;  
                ev.data.fd = it->second->get_Id();  
                epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, it->second->get_Id(), &ev);
            }
        }
    }  
   
}
void execute_mode_o(Server& server,char &sig, Client& client, const std::vector<std::string>& args , int &i){
     if (sig  == 0 ){
        // all domme likha3lin n7athom 
        return ;
    }
    else {
        if(i  >= args.size()){
            return ;
        }
        std::map<std::string , chanel> ::iterator it_ch = server.get_Chanel().find(args[0]);
        std::string msg ;
        int p = 0;
        std::map <int , Client> :: iterator All_client ;
        for(All_client = server.get_mapClient().begin() ; All_client != server.get_mapClient().end() ; ++All_client){
            if(All_client->second.get_nake() == args[i]){
                p = 1;
                break;
            }
        }
        if(p == 0){
            client.getSendBuffer() += ":localhost 441 " + client.get_nake() + " " + args[i] + " " + it_ch->second.get_namechanel() + " :They aren't on that channel\r\n";  
            i++;
            return;
        }
        if(it_ch->second.is_member(All_client->second.get_Id()) == 0){
            client.getSendBuffer() += ":localhost 441 " + client.get_nake() + " " + args[i] + " " +  it_ch->second.get_namechanel() + " :They aren't on that channel\r\n";
            i++;
            return;
        }
        p = 0;
        if(sig == '+'){
            if(it_ch->second.is_operator(All_client->second.get_Id()) == 1){
                i++ ;
                return ;
            }
            it_ch->second.get_opChanel()[All_client->second.get_Id()] = &All_client->second ;
            msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " MODE " + it_ch->second.get_namechanel() + " +o " + args[i] + "\r\n";
            p = 1;
            i++;
        }
        else if( sig == '-' ){
            if (it_ch->second.is_operator(All_client->second.get_Id()) == 0){
                i++ ;
                return ;
            }
            it_ch->second.get_opChanel().erase(All_client->first);
            msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " MODE " + it_ch->second.get_namechanel() + " -o " + args[i] + "\r\n";
            p = 1;
            i++;
        }
        if(p == 1){
            std::map<int , Client*> ::iterator it ;
            for(it = it_ch->second.get_All_Cchanel().begin() ;it != it_ch->second.get_All_Cchanel().end() ;++it){
                it->second->getSendBuffer()+= msg ;
                struct epoll_event ev;   
                ev.events = EPOLLIN | EPOLLOUT;  
                ev.data.fd = it->second->get_Id();  
                epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, it->second->get_Id(), &ev);
            }
        }
             
    } 
}
#include <iostream>
#include <string>
#include <climits>
void execute_mode_l(Server& server,char &sig, Client& client, const std::vector<std::string>&args, int &i){
     if (sig  == 0 ){
        // all domme likha3lin n7athom 
        return ;
    }
    else {

        std::map<std::string , chanel> ::iterator it_ch = server.get_Chanel().find(args[0]);
        std::string msg ;
        int p = 0;
        if(sig == '+'){
            if(i  >= args.size()){
                return ;
            }
            if (args[i].find_first_not_of("0123456789") != std::string::npos){
                i++;
                return ;
            }
            if (args[i].length() > 10 || (args[i].length() == 10 && args[i] > "2147483647")) {
                i++;
                return;
            }
            int num = atoi(args[i].c_str());
            if (it_ch->second.get_limitchanell() == num) {
                i++;
                return;
            }
            it_ch->second.set_limit(num);
            msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " MODE " + it_ch->second.get_namechanel() + " +l " + args[i] + "\r\n";            p = 1;
            i++;
        }
        else if(sig == '-'){
            if(it_ch->second.get_limitchanell() == INT_MAX){
                return ;
            }
            it_ch->second.set_limit(INT_MAX);
            msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " MODE " + it_ch->second.get_namechanel() + " -l\r\n";            p = 1;
        }
        if(p == 1){
            std::map<int , Client*> ::iterator it ;
            for(it = it_ch->second.get_All_Cchanel().begin() ;it != it_ch->second.get_All_Cchanel().end() ;++it){
                it->second->getSendBuffer()+= msg ;
                struct epoll_event ev;   
                ev.events = EPOLLIN | EPOLLOUT;  
                ev.data.fd = it->second->get_Id();  
                epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, it->second->get_Id(), &ev);
            }
        }       
    } 
}
void execute_mode_t(Server& server, char &sig, Client& client, const std::vector<std::string>& args) {
    if (sig == 0) {
        return; 
    }
    std::string msg;
    int p = 0;
    std::map<std::string , chanel> ::iterator it_ch = server.get_Chanel().find(args[0]);
    if (sig == '+') {
        if (it_ch->second.flag_topic() == true) {
            return;
        }
        it_ch->second.set_flag_topic(1);  
        msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " MODE " + it_ch->second.get_namechanel() + " +t\r\n";
        p = 1;
    }
    else if (sig == '-') {
        if (it_ch->second.flag_topic() == false) {
            return;
        }
        it_ch->second.set_flag_topic(false);
        msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() + " MODE " + it_ch->second.get_namechanel() + " -t\r\n";
        p = 1;
    }
    if (p == 1) {
        std::map<int, Client*>::iterator it;
        for (it = it_ch->second.get_All_Cchanel().begin(); it != it_ch->second.get_All_Cchanel().end(); ++it) {
            it->second->getSendBuffer() += msg;
            
            struct epoll_event ev;   
            ev.events = EPOLLIN | EPOLLOUT;  
            ev.data.fd = it->second->get_Id();  
            epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, it->second->get_Id(), &ev);
        }
    }
}
// MODE #chanel +dsf sdsad
void executeMode::executeCmd(Server& server, Client& client, const std::vector<std::string>& args){
    if(client.get_Isregister() == false){
        std::string str = client.get_nake().empty() ? "*" : client.get_nake() ;
        client.getSendBuffer() += ":localhost 451 " + str + " :You have not registered\r\n";
        return ;
    }
    if(args.empty()){
        client.getSendBuffer() += ":localhost 411 " + client.get_nake() + " :No recipient given (MODE)\r\n";
        return ;
    }
    std::map<std::string , chanel> ::iterator it_ch = server.get_Chanel().find(args[0]);
    if(it_ch != server.get_Chanel().end()){
        if(it_ch->second.is_member(client.get_Id()) == 0){
            client.getSendBuffer() += ":localhost 442 " + client.get_nake() + " " + args[0] + " :You're not on that channel\r\n";
            return;
        }
        if (args.size() == 1) {
        std::string active_modes = "+";
        std::string mode_params = "";
        if (it_ch->second.get_invetation())
            active_modes += "i";
        if (it_ch->second.is_topic())       
            active_modes += "t";
        if (it_ch->second.cheak_passw())    
            active_modes += "k";
        if (it_ch->second.get_islimite())   
            active_modes += "l";
        if (it_ch->second.cheak_passw()) {
            if (it_ch->second.is_operator(client.get_Id()) == 1) {
                mode_params += " " + it_ch->second.get_passwordChanel();
            }else {
                mode_params += " *"; 
            }
        }
        if (it_ch->second.get_islimite()) {
            std::stringstream str;
            str << it_ch->second.get_limitchanell();
            mode_params += " " + str.str();
        }
        client.getSendBuffer() += ":localhost 324 " + client.get_nake() + " " + args[0] + " " + active_modes + mode_params + "\r\n";
            return; 
        }
        if(it_ch->second.is_operator(client.get_Id()) == 0){
            client.getSendBuffer() += ":localhost 482 " + client.get_nake() + " " + args[0] + " :You're not channel operator\r\n";
            return ;
        }
        int i = 0  , index = 2;
        char sig = 0;
        while (i  < args[1].size())
        {
            if(args[1][i] == '+'  || args[1][i] == '-'){
                sig = args[1][i] ;
                i++;
                continue; 
            }
            switch (args[1][i])
            {
                case 'i':
                    execute_mode_i(server,sig,client,args);
                    break;
                case 'k':
                    execute_mode_k(server,sig,client,args,index);
                    break;
                case 'o':
                    execute_mode_o(server,sig,client,args,index);
                    break;
                case 'l':
                    execute_mode_l(server,sig,client,args,index);
                    break;
                case 't':
                    execute_mode_t(server,sig,client,args);
                    break;
                default:
                    client.getSendBuffer() += ":localhost 472 " + client.get_nake() + " " + args[1][i] + " :is unknown mode char to me\r\n";
                    break;
            };
            i ++;
            index ++;
        }
        
    }else{
        client.getSendBuffer() += ":localhost 403 " + client.get_nake() + " " + args[0] + " :No such channel\r\n";
        return;
    }
}

void executeTopic::executeCmd(Server& server, Client& client, const std::vector<std::string>& args){
    if (client.get_Isregister() == false) {
        std::string str = client.get_nake().empty() ? "*" : client.get_nake();
        client.getSendBuffer() += ":localhost 451 " + str + " :You have not registered\r\n";
        return;
    }
    if (args.empty() || args[0].empty()) {
        client.getSendBuffer() += ":localhost 461 " + client.get_nake() + " TOPIC :Not enough parameters\r\n";
        return;
    }
    std::map<std::string, chanel>::iterator it = server.get_Chanel().find(args[0]);
    if (it == server.get_Chanel().end()) {
        client.getSendBuffer() += ":ft_irc_default 403 " + client.get_nake() + " " + args[0] + " :No such channel\r\n";

        return;
    }
    if (it->second.is_member(client.get_Id()) == 0) {
        client.getSendBuffer() += ":ft_irc_default 442 " + client.get_nake() + " " + it->second.get_namechanel() + " :You're not on that channel\r\n";
        return;
    }
    if (args.size() == 1) {
        if (it->second.is_topic() == 0) {
            client.getSendBuffer() += ":ft_irc_default 331 " + client.get_nake() + " " + it->second.get_namechanel() + " :No topic is set\r\n";
        } else {
            client.getSendBuffer() += ":ft_irc_default 332 " + client.get_nake() + " " + it->second.get_namechanel() + " :" + it->second.get_topic_chanel() + "\r\n";
        }
        return; 
    }
    if(args.size()  > 1){
        if(it->second.flag_topic() && it->second.is_operator(client.get_Id()) == 0){
            client.getSendBuffer() += ":ft_irc_default 482 " + client.get_nake() + " " + it->second.get_namechanel() + " :You're not channel operator\r\n";
            return ;
        }
        it->second.set_topic_chanel(args[1]);
        std::map<int , Client*> ::iterator it_l ;
        std::string msg = ":" + client.get_nake() + "!" + client.get_user() + "@" + client.get_host() 
                        + " TOPIC " + it->second.get_namechanel()+ " :" + args[1] + "\r\n";
        for(it_l = it->second.get_All_Cchanel().begin() ;it_l != it->second.get_All_Cchanel().end() ;++it_l){
            it_l->second->getSendBuffer()+= msg ;
            struct epoll_event ev;   
            ev.events = EPOLLIN | EPOLLOUT;  
            ev.data.fd = it_l->second->get_Id();  
            epoll_ctl(server.get_fdeppol(), EPOLL_CTL_MOD, it_l->second->get_Id(), &ev);
        }
    }
}