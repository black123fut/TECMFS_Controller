#include "Singleton.h"

Singleton* Singleton::instance = nullptr;
LinkedList<sf::TcpSocket*>* Singleton::clients = nullptr;
LinkedList<sf::TcpSocket*>* Singleton::disks = nullptr;

Singleton *Singleton::getInstance() {
    if (instance == nullptr) {
        instance = new Singleton;
        clients = new LinkedList<sf::TcpSocket*>;
        disks = new LinkedList<sf::TcpSocket*>;
    }
    return instance;
}

LinkedList<sf::TcpSocket *> *Singleton::getClients() {
    if (instance == nullptr) {
        getInstance();
    }
    return clients;
}

LinkedList<sf::TcpSocket *> *Singleton::getDisk() {
    if (instance == nullptr) {
        getInstance();
    }
    return disks;
}

Singleton::Singleton() {

}
