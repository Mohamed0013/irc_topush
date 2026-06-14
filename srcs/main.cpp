#include "../headers/header.hpp"

int main(int ac , char **av) {
    if(ac != 3){
        std::cout << "Error: argument " << std::endl;
        return -1 ;
    }
    try {
        Server irc(atoi(av[1]) , av[2]);
        irc.run();
    } catch (const std::exception& e) {
        std::cerr << "Server Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}