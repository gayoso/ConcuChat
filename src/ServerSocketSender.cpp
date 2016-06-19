#include "ServerSocketSender.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

ServerSocketSender::ServerSocketSender() : Socket(SERVER_PORT),
    colaDeRecibidos(NOM_SERVER_SENDER, C_SENDER_RECIBIDOS)
{

}

void ServerSocketSender::_run() {

    SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    std::string respuesta;
    while(!sigint_handler.signalWasReceived()) {
        mensaje recibido;
        this->colaDeRecibidos.leer ( ANY_MESSAGE,&(recibido) );

        if(sigint_handler.signalWasReceived()) {
            continue;
        }

        std::cout << "Sender: lei mensaje de la cola" << std::endl;

        if (recibido.mtype == CONNETION_START) {
            std::cout << "Sender: era connection start con socket: " << recibido.socket << std::endl;

            FifoEscritura* fifo = new FifoEscritura(recibido.texto);
            fifo->abrir();
            fifosDeEnvio.insert(std::pair<int, FifoEscritura*>(recibido.socket, fifo));

        } else if (recibido.mtype == CONNECTION_END) {
            std::cout << "Sender: era connection end" << std::endl;

            FifoEscritura* fifo = fifosDeEnvio.at(recibido.socket);
            respuesta = "CONNETION_END";
            fifo->escribir(respuesta.c_str(), respuesta.size());
            fifo->cerrar();
            delete fifo;
            fifosDeEnvio.erase(recibido.socket);

        } else if (recibido.mtype == TEXT) {
            respuesta = recibido.texto;
            //respuesta.resize(recibido.msize);
            std::cout << "Sender: era texto: " << respuesta;
            std::cout << ", tengo " << fifosDeEnvio.size() << " clientes" << std::endl;

            for (std::map<int, FifoEscritura*>::iterator it = fifosDeEnvio.begin();
                    it != fifosDeEnvio.end(); ++it) {
                std::cout << "Escribi a " << it->first << std::endl;

                FifoEscritura* fifo = it->second;
                fifo->escribir(respuesta.c_str(), respuesta.size());
            }
        } else {
            // error
            std::cout << "Sender: era basura" << std::endl;
        }
    }

    colaDeRecibidos.destruir();
    unlink(NOM_SERVER_SENDER);
}

void ServerSocketSender::init() {
    close ( open ( NOM_SERVER_SENDER,O_CREAT,0777 ) );

    colaDeRecibidos.crear();
}
