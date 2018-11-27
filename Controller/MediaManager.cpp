#include <SFML/Network/Packet.hpp>
#include "MediaManager.h"


std::vector<byte> MediaManager::recibirVideo(sf::TcpSocket *cliente) {
    std::vector<byte> videoBytes;
    sf::Packet paqueteRecibido;
    std::string stringRecibido;

    if (cliente->receive(paqueteRecibido) == sf::Socket::Done) {
        paqueteRecibido >> stringRecibido;
        videoBytes = std::vector<byte>(stringRecibido.begin(), stringRecibido.end());
    }
    return videoBytes;
}

LinkedList<std::vector<byte>> MediaManager::dividirVideo(std::vector<byte> videoBytes) {
    int partSize = (int) videoBytes.size() / 3;
    int start = 0, end = partSize;

    LinkedList<std::vector<byte>> partesVideo = LinkedList<std::vector<byte>>();

    for (int i = 0; i < 3; i++) {
        if (i == 2) {
            end = (int) videoBytes.size();
        }
        std::vector<byte> subVideo(&videoBytes[start], &videoBytes[end]);
        partesVideo.add(subVideo);
        start = end;
        end += partSize;
    }
    return partesVideo;
}

/**
 * Une las partes del video para recuperar el original.
 * @param parte1 Primera parte.
 * @param parte2 Segunda parte.
 * @param parte3 Tercera parte.
 * @return String con el video original.
 */
std::string MediaManager::unirVideo(std::string parte1, std::string parte2, std::string parte3) {
    std::vector<byte> vector1(parte1.begin(), parte1.end());
    std::vector<byte> vector2(parte2.begin(), parte2.end());
    std::vector<byte> vector3(parte3.begin(), parte3.end());

    vector1.insert(vector1.end(), vector2.begin(), vector2.end());
    vector1.insert(vector1.end(), vector3.begin(), vector3.end());

    std::string video = std::string(vector1.begin(), vector1.end());
    return video;
}
