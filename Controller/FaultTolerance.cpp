#include "FaultTolerance.h"

std::vector<byte>
FaultTolerance::calcularParidad(std::vector<byte> parte1, std::vector<byte> parte2, std::vector<byte> parte3) {
    std::vector<byte> paridad;

    paridad.reserve(parte3.size());
    for (int i = 0; i < parte3.size(); i++) {
        paridad.push_back(parte1[i] ^ parte2[i] ^ parte3[i]);
    }

    return paridad;
}

std::string FaultTolerance::recuperarArchivo(std::string parte1, std::string parte2, std::string paridad) {
    std::vector<byte> fallo;

    std::vector<byte> partA(parte1.begin(), parte1.end());
    std::vector<byte> partB(parte2.begin(), parte2.end());

    fallo.reserve(paridad.size());
    for (int i = 0; i < paridad.size(); i++) {
        fallo.push_back(partA[i] ^ partB[i] ^ paridad[i]);
    }

    if (partA.size() != partB.size()) {
        for (int i = 0; i < (paridad.size() - partA.size()); i++) {
            fallo.erase(fallo.begin() + (fallo.size() - 1));
        }
    }

    std::string falloString(fallo.begin(), fallo.end());
    return falloString;
}
