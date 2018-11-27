#include <SFML/Network/Packet.hpp>
#include <string.h>
#include "DataBase.h"
#include "../Server/Singleton.h"
#include "FaultTolerance.h"
#include "DiskManager.h"

void DataBase::crearTabla() {
    std::string contenido = "filename;extension;size;part1;part2;part3;parity/";
    enviar(contenido, "save");
}

void DataBase::agregarContenido_a_Tabla(std::string contenido) {
    std::string dataBase = getTabla();
    dataBase.append(contenido);
    dataBase.append("/");
    cout << "DataBase: " << dataBase << endl;

    LinkedList<std::string> filas = splitString(dataBase, const_cast<char *>("/"));
    DiskManager::bloquesUsados = filas.length() - 1;

    enviar(dataBase, "save");
}

std::string DataBase::getTabla() {
    std::string part1 = "";
    std::string part2 = "";
    std::string part3 = "";
    std::string parity = "";

    for (int i = 0; i < 4; i++) {
        sf::Packet packet;
        packet << "database";
        packet << "get";

        sf::TcpSocket *disco = Singleton::getDisk()->get(i);
        disco->send(packet);

        sf::Packet recibirPacket;
        std::string recibirString;

        if (disco->receive(recibirPacket) == sf::Socket::Done) {
            recibirPacket >> recibirString;
            if (i == 0) {
                part1 = recibirString;
            }
            else if (i == 1) {
                part2 = recibirString;
            }
            else if (i == 2) {
                part3 = recibirString;
            }
            else if (i == 3) {
                parity = recibirString;
            }
        }
    }

    //Caso en el que algun disco tuvo fallo
    if (part1.empty()) {
        part1 = FaultTolerance::recuperarArchivo(part2, part3, parity);
    }
    else if (part2.empty()) {
        part2 = FaultTolerance::recuperarArchivo(part1, part3, parity);
    }
    else if (part3.empty()) {
        part3 = FaultTolerance::recuperarArchivo(part1, part2, parity);
    }

    part1 += (part2 + part3);

    cout << "Parte1 getTabla: " << part1 << endl;
    return part1;
}

LinkedList<std::string> DataBase::splitString(std::string contenido, char *splitCharacter) {
    LinkedList<std::string> lista = LinkedList<std::string>();
    char* mensage = strdup(contenido.c_str());
    char* elemento = strtok(mensage, splitCharacter);

    while (elemento != nullptr) {
        std::string str(elemento);
        lista.add(str);
        elemento = strtok(nullptr, splitCharacter);
    }

    return lista;
}

LinkedList<std::string> DataBase::getVideoEnTabla(std::string name) {
    LinkedList<std::string> filas = splitString(getTabla(), const_cast<char *>("/"));

    for (int i = 0; i < filas.length(); i++) {
        std::string fila = filas.get(i);
        LinkedList<std::string> dato = splitString(fila, const_cast<char *>(";"));
        cout << "Fila " << i << " : " << dato.get(0) << endl;

        if (strcmp(dato.get(0).c_str(), name.c_str()) == 0) {
            return dato;
        }
    }

    return LinkedList<std::string>();
}
/**
 * Obtiene la informacion de un disco.
 * @param diskNum Numero del disco.
 * @return String
 */
std::string DataBase::getParte(int diskNum) {
    std::string parte1 = "";
    std::string parte2 = "";
    std::string parte3 = "";
    std::string paridad = "";

    for (int i = 0; i < 4; i++) {
        if (i != diskNum) {
            sf::Packet packet;
            packet << "database";
            packet << "get";

            sf::TcpSocket *diskSocket = Singleton::getDisk()->get(i);
            diskSocket->send(packet);

            sf::Packet paqueteRecibido;
            std::string mensajeRecibido;
            if (diskSocket->receive(paqueteRecibido) == sf::Socket::Done) {
                paqueteRecibido >> mensajeRecibido;
            }
            if (i == 0) {
                parte1 = mensajeRecibido;
            }
            else if (i == 1) {
                parte2 = mensajeRecibido;
            }
            else if (i == 2) {
                parte3 = mensajeRecibido;
            }
            else if (i == 3) {
                paridad = mensajeRecibido;
            }
        }
    }

    switch (diskNum) {
        case 0:
            parte1 = FaultTolerance::recuperarArchivo(parte2, parte3, paridad);
            return parte1;
        case 1:
            parte2 = FaultTolerance::recuperarArchivo(parte1, parte3, paridad);
            return parte2;
        case 2:
            parte3 = FaultTolerance::recuperarArchivo(parte1, parte2, paridad);
            return parte3;
        case 3:
            std::vector<byte> vector1(parte1.begin(), parte1.end());
            std::vector<byte> vector2(parte2.begin(), parte2.end());
            std::vector<byte> vector3(parte3.begin(), parte3.end());
            std::vector<byte> vectorParidad = FaultTolerance::calcularParidad(vector1, vector2, vector3);
            std::string stringParidad(vectorParidad.begin(), vectorParidad.end());
            return stringParidad;
    }
}

void DataBase::enviar(std::string contenido, std::string opcode) {
    int size = (int) contenido.size() / 3;

    int start = 0;
    int end = size;

    LinkedList< std::vector<byte> > partesTotal = LinkedList< std::vector<byte> >();

    for (int i = 0; i < 3; i++) {
        if (i == 2)
            end = (int) contenido.size();

        std::string cadena = std::string(&contenido[start], &contenido[end]);
        std::vector<byte> parte(cadena.begin(), cadena.end());
        partesTotal.add(parte);

        start = end;
        end += size;

        sf::Packet packet;
        packet << "database";
        packet << opcode;
        packet << cadena;

        Singleton::getDisk()->get(i)->send(packet);
    }

    //Calcula la paridad
    std::vector<byte> paridadVector = FaultTolerance::calcularParidad(partesTotal.get(0),
                                                                partesTotal.get(1),
                                                                partesTotal.get(2));
    //Envia la paridad al cuarto disco.
    sf::Packet paridad;
    paridad << "dataBase";
    paridad << opcode;
    paridad << std::string(paridadVector.begin(), paridadVector.end());
    Singleton::getDisk()->get(3)->send(paridad);
}
