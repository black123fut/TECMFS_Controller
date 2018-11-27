//
// Created by isaac on 22/11/18.
//

#ifndef TECMFS_CONTROLLER_DATABASE_H
#define TECMFS_CONTROLLER_DATABASE_H


#include <string>
#include "../DataStructures/LinkedList.h"

class DataBase {
public:
    static void crearTabla();

    static void agregarContenido_a_Tabla(std::string contenido);

    static std::string getTabla();

    static LinkedList<std::string> splitString(std::string contenido, char *splitCharacter);

    static LinkedList<std::string> getVideoEnTabla(std::string name);

    static std::string getParte(int diskNum);

private:
    static void enviar(std::string contenido, std::string opcode);
};


#endif //TECMFS_CONTROLLER_DATABASE_H
