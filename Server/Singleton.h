#ifndef TECMFS_CONTROLLER_SINGLETON_H
#define TECMFS_CONTROLLER_SINGLETON_H


#include "../DataStructures/LinkedList.h"
#include <SFML/Network.hpp>

class Singleton {
public:
    static Singleton *getInstance();

    static LinkedList<sf::TcpSocket*> *getClients();

    static LinkedList<sf::TcpSocket*> *getDisk();

private:
    static Singleton *instance;
    static LinkedList<sf::TcpSocket*> *clients;
    static LinkedList<sf::TcpSocket*> *disks;

    Singleton();
};


#endif //TECMFS_CONTROLLER_SINGLETON_H
