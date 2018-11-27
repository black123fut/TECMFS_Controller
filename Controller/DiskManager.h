
#ifndef TECMFS_CONTROLLER_DISKMANAGER_H
#define TECMFS_CONTROLLER_DISKMANAGER_H

#include <iostream>
#include <SFML/Network/TcpSocket.hpp>

typedef unsigned char byte;

class DiskManager {
public:
    static int bloquesUsados;

    DiskManager();

    void agregarDisco(sf::TcpSocket *socket);

    std::string getVideo(std::string nombre);

    void enviarPartes(std::vector<byte> data, std::string filename);
private:
    int paridad = 0;

    int getParidad();

    std::string getPart(std::string nombre, int diskNum);

    void restaurarDisco(int diskNum);
};


#endif //TECMFS_CONTROLLER_DISKMANAGER_H
