//
// Created by isaac on 22/11/18.
//

#include "DiskManager.h"
#include "../Server/Singleton.h"
#include "MediaManager.h"
#include "FaultTolerance.h"
#include "DataBase.h"

int DiskManager::bloquesUsados = 0;

DiskManager::DiskManager() {

}

void DiskManager::agregarDisco(sf::TcpSocket *socket) {
    int  i = 0;

    while (i != 4) {
        if (Singleton::getDisk()->length() < 4) {
            Singleton::getDisk()->add(socket);
            cout << "Disco nuevo" << endl;
            break;
        }
        else {
            sf::TcpSocket *disk = Singleton::getDisk()->get(i);
            sf::Packet packet;
            packet << "verifyingConnection...";
            disk->send(packet);
            bool connected = false;

            if (disk->receive(packet) == sf::Socket::Done) {
                connected = true;
            }
            if (!connected) {
                Singleton::getDisk()->getNode(i)->setData(socket);
                break;
            }
        }
        i++;
    }
}

std::string DiskManager::getVideo(std::string nombre) {
    LinkedList<std::string> video = DataBase::getVideoEnTabla(nombre);

    if (video.length() > 0) {
        std::string parte1 = "";
        std::string parte2 = "";
        std::string parte3 = "";
        std::string parity = "";

        int currentPart = 0;

        for (int i = 1; i < 5; i++) {
            int discoNum = std::stoi(video.get(2 + i));
            std::string nombreParameter = nombre + std::to_string(currentPart + 1);

            if (i == 1) {
                parte1 = getPart(nombreParameter, discoNum);
            }
            else if (i == 2) {
                parte2 = getPart(nombreParameter, discoNum);
            }
            else if (i == 3) {
                parte3 = getPart(nombreParameter, discoNum);
            }
            else if (i == 4) {
                parity = getPart(nombre + "_parity", discoNum);
            }
            currentPart++;
        }

        cout << "byte 1: " << parte1 << endl;
        cout << "byte 2: " << parte2 << endl;
        cout << "byte 3: " << parte3 << endl;
        cout << "byte p: " << parity << endl;

        if (parte1.empty()) {
            parte1 = FaultTolerance::recuperarArchivo(parte2, parte3, parity);
        }
        else if (parte2.empty()) {
            parte2 = FaultTolerance::recuperarArchivo(parte1, parte3, parity);
        }
        else if (parte3.empty()) {
            parte3 = FaultTolerance::recuperarArchivo(parte1, parte2, parity);
        }

        std::string videoString = MediaManager::unirVideo(parte1, parte2, parte3);
        return videoString;
    }
}

void DiskManager::enviarPartes(std::vector<byte> data, std::string filename) {
    LinkedList<std::vector<byte>> partes = MediaManager::dividirVideo(data);
    std::vector<byte> paridadVector = FaultTolerance::calcularParidad(partes.get(0), partes.get(1), partes.get(2));

    LinkedList<std::string> lista = DataBase::splitString(filename, const_cast<char *>("."));
    std::string name = lista.get(0);
    std::string extension = lista.get(1);

    std::string nuevoElemento = name + ";" + extension + ";" + std::to_string(data.size()) + ";";
    std::string paridadDiscoString;

    int paridadDisco = getParidad();
    int currentPart = 0;

    for (int i = 0; i < 4; i++) {
        sf::Packet packet;
        packet << "savePart";

        if (i == paridadDisco) {
            packet << name + "_parity";
            packet << std::string(paridadVector.begin(), paridadVector.end());
            paridadDiscoString = std::to_string(i);
        }
        else {
            packet << name + std::to_string(currentPart + 1);
            std::vector<byte> parte = partes.get(currentPart);
            packet << std::string(parte.begin(), parte.end());
            nuevoElemento.append(std::to_string(i) + ";");
            currentPart++;
        }
        Singleton::getDisk()->get(i)->send(packet);
    }
    nuevoElemento.append(paridadDiscoString);
    DataBase::agregarContenido_a_Tabla(nuevoElemento);
}

/**
 * Obtiene el disco que le corresponde la paridad.
 * @return Posicion del disco que le corresponde la paridad.
 */
int DiskManager::getParidad() {
    int disco = paridad;
    paridad++;

    if (paridad > 3) {
        paridad = 0;
    }

    return disco;
}

std::string DiskManager::getPart(std::string nombre, int diskNum) {
    sf::TcpSocket *disco = Singleton::getDisk()->get(diskNum);

    sf::Packet packet;
    packet << "getPart";
    packet << nombre;
    disco->send(packet);

    sf::Packet paqueteRecibido;
    std::string mensajeRecibido;

    if (disco->receive(paqueteRecibido) == sf::Socket::Done) {
        paqueteRecibido >> mensajeRecibido;
    }

    return mensajeRecibido;
}

void DiskManager::restaurarDisco(int diskNum) {

}
