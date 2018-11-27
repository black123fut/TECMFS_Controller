#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <cstring>
#include "SFML/Graphics.hpp"
#include "DataStructures/LinkedList.h"
#include "Server/Singleton.h"
#include "Controller/DiskManager.h"
#include "Controller/MediaManager.h"
#include "Controller/DataBase.h"

#define PORT 8000

void drawWindow(sf::RenderWindow* window) {
    int unitario = 0;
    int scroll = 0, current = 0, limits = 0;
    window->setActive(true);

    sf::Font font;
    if (!font.loadFromFile("../alien.ttf"))
        cout << "No se encontro la fuente" << endl;

    LinkedList<sf::RectangleShape *> rectangles = LinkedList<sf::RectangleShape *>();
    LinkedList<sf::Text*> labels = LinkedList<sf::Text*>();

    for (int i = 0; i < 4; i++) {
        if (i != 0) {
            unitario += 1;
        }

        for (int j = 0; j < 31; j++) {
            std::string labelString;
            sf::RectangleShape *rect = new sf::RectangleShape(sf::Vector2f(150, 60));

            if (j == 0) {
                rect->setFillColor(sf::Color(164, 186, 150));
                labelString = "Disco " + to_string(i + 1);
            }
            else {
                rect->setFillColor(sf::Color(255, 255, 255));
                labelString = "Bloque " + to_string(j);
            }

            int x = 50 + 150 * i + 33 * unitario, y = 60 * j;
            rect->setPosition(x, y);
            rectangles.add(rect);
            rectangles.getNode(current)->x = x;
            rectangles.getNode(current)->y = y;

            int x1 = 92 + 150 * i + 33 * unitario, y1 = 20 + 60 * j;
            sf::Text *text = new sf::Text(labelString, font, 20);
            text->setFillColor(sf::Color(0, 0, 0));
            text->setPosition(x1, y1);
            labels.add(text);
            labels.getNode(current)->x = x1;
            labels.getNode(current)->y = y1;

            current++;
        }
    }

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent((event))) {
            if (event.type == sf::Event::EventType::Closed)
                window->close();
            else if (event.type == sf::Event::MouseWheelScrolled) {
                scroll = (int) event.mouseWheelScroll.delta;
                if (limits == -1260 && scroll == 1) {
                    scroll = 0;
                }
                else if (limits == 0 && scroll == -1) {
                    scroll = 0;
                }
                else {
                    limits = limits - scroll * 20;
                }
                cout  << "Limites: " << limits << endl;
            }
        }
        if (scroll != 0) {
            for (int i = 0; i < rectangles.length(); ++i) {
                int x1 = rectangles.getNode(i)->x, y1 = rectangles.getNode(i)->y,
                    x2 = labels.getNode(i)->x, y2 = labels.getNode(i)->y;

                rectangles.get(i)->setPosition(x1, y1 - scroll * 20);
                labels.get(i)->setPosition(x2, y2 - scroll * 20);

                rectangles.getNode(i)->y = y1 - scroll * 20;
                labels.getNode(i)->y = y2 - scroll * 20;
            }

        }
        scroll = 0;

        for (int k = 0; k < rectangles.length(); ++k) {
            //Numero de bloque ocupados
            if (DiskManager::bloquesUsados > (k % 31 - 1) && k % 31 != 0) {
                rectangles.get(k)->setFillColor(sf::Color(51, 204, 204));
            }
        }
        for (int i = 0; i < rectangles.length(); ++i) {
            window->draw(*rectangles.get(i));
        }
        for (int j = 0; j < labels.length(); ++j) {
            window->draw(*labels.get(j));
        }

        window->display();
    }
}

int main(){
    sf::RenderWindow window(sf::VideoMode(800, 600), "Estado de los discos");
    window.setActive(false);

    sf::Thread thread(&drawWindow, &window);
    thread.launch();

    bool done = false;
    int counter = 0;
    DiskManager controller;
    sf::TcpListener listener;
    sf::SocketSelector selector;

    listener.listen(PORT);
    selector.add(listener);

    LinkedList< sf::TcpSocket* > *clients = Singleton::getClients();
    LinkedList< sf::TcpSocket* > *disks = Singleton::getDisk();

    while (!done) {
        if (disks->length() == 4 && counter == 0) {
            cout << "Controller listo" << endl;
            DataBase::crearTabla();
//            string tabla = "filename;extension;size;part1;part2;part3;parity/";
//            string tablaDiscos = DataBase::getTabla();
//            if (tablaDiscos == tabla || tablaDiscos.empty()) {
//                DataBase::crearTabla();
//            }
            counter++;
        }
        if (selector.wait()) {
            if (selector.isReady(listener)) {
                sf::TcpSocket * newSocket = new sf::TcpSocket();
                listener.accept(*newSocket);
                sf::Packet packet;
                std::string id;
                if (newSocket->receive(packet) == sf::Socket::Done) {
                    packet >> id;
                }
                if (id == "Disk") {
                    controller.agregarDisco(newSocket);
                    cout << "Numero de discos: " << disks->length() <<endl;
                }
                else {
                    cout << "Se conecto un cliente" << endl;
                    clients->add(newSocket);
                }
                selector.add(*newSocket);
            }
        }
        if (disks->length() == 4) {
            for (int i = 0; i < clients->length(); ++i) {
                if (selector.isReady(*clients->get(i))) {
                    sf::Packet paqueteRecibido;
                    if (clients->get(i)->receive(paqueteRecibido) == sf::Socket::Done) {
                        std::string opcodeString;
                        paqueteRecibido >> opcodeString;
                        char* opcode = (char *) opcodeString.c_str();

                        if (strcmp(opcode, "saveVideo") == 0) {
                            cout << "Guardando Video" << endl;
                            std::string nombreArchivo;
                            paqueteRecibido >> nombreArchivo;
                            std::vector<byte> video = MediaManager::recibirVideo(clients->get(i));
                            cout << "Nombre del video: " << nombreArchivo << endl;
                            controller.enviarPartes(video, nombreArchivo);
                        }
                        else if (strcmp(opcode, "getVideo") == 0) {
                            cout << "Solicitando video" << endl;
                            std::string nombreArchivo;
                            paqueteRecibido >> nombreArchivo;

                            cout << "Nombre Solicitud: " << nombreArchivo << endl;
                            std::string video = controller.getVideo(nombreArchivo);
                            sf::Packet respuesta;
                            cout << "Video bytes: " << video << endl;
                            respuesta << video;
                            //Extension
                            respuesta << DataBase::getVideoEnTabla(nombreArchivo).get(1);
                            clients->get(i)->send(respuesta);
                        }

                        else if (strcmp(opcode, "getTable") == 0) {
                            std::string tabla = DataBase::getTabla();
                            sf::Packet respuesta;
                            respuesta << tabla;
                            clients->get(i)->send(respuesta);
                        }
                    }
                }
            }
        }
    }
}