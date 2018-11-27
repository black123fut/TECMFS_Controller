//
// Created by isaac on 22/11/18.
//

#ifndef TECMFS_CONTROLLER_FAULTTOLERANCE_H
#define TECMFS_CONTROLLER_FAULTTOLERANCE_H

#include <iostream>
#include <vector>

typedef unsigned char byte;

class FaultTolerance {
public:
    static std::vector<byte>
    calcularParidad(std::vector<byte> parte1, std::vector<byte> parte2, std::vector<byte> parte3);

    static std::string recuperarArchivo(std::string parte1, std::string parte2, std::string paridad);
};


#endif //TECMFS_CONTROLLER_FAULTTOLERANCE_H
