//
// Created by isaac on 23/11/18.
//

#ifndef TECMFS_CONTROLLER_MEDIAHANDLER_H
#define TECMFS_CONTROLLER_MEDIAHANDLER_H


#include <vector>
#include <SFML/Network/TcpSocket.hpp>
#include "../DataStructures/LinkedList.h"

typedef unsigned char byte;

class MediaManager {
public:
    static std::vector<byte> recibirVideo(sf::TcpSocket *cliente);

    static LinkedList<std::vector<byte>> dividirVideo(std::vector<byte> videoBytes);

    static std::string unirVideo(std::string parte1, std::string parte2, std::string parte3);
};


#endif //TECMFS_CONTROLLER_MEDIAHANDLER_H
