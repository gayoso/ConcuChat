#include "ServerSocketSender.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sys/wait.h>

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

        //Logger::log("SENDER", "Recibi mensaje en la cola" , DEBUG);
        //std::cout << "Sender: lei mensaje de la cola" << std::endl;

        if (recibido.mtype == CONNECTION_START) {
            Logger::log("SENDER", "Recibi connection start de socket: " + std::to_string(recibido.socket) , DEBUG);
            //std::cout << "Sender: era connection start con socket: " << recibido.socket << std::endl;

            respuesta = recibido.texto;
            int first_sep = respuesta.find_first_of(SEPARATOR, 0);
            std::string nombreFifo = respuesta.substr(0, first_sep);
            //std::cout << "Sender: nombre: " << nombreFifo << std::endl;
            std::string pidString = respuesta.substr(first_sep+1, respuesta.size());
            //std::cout << "Sender: pid: " << pidString << std::endl;
            pid_t pidListener = std::stoi(pidString, 0, 10);

            serverListeners.insert(std::pair<int, pid_t>(recibido.socket, pidListener));

            FifoEscritura* fifo = new FifoEscritura(nombreFifo);
            fifo->abrir();
            fifosDeEnvio.insert(std::pair<int, FifoEscritura*>(recibido.socket, fifo));

        } else if (recibido.mtype == CONNECTION_END) {
            //std::cout << "Sender: era connection end" << std::endl;
            Logger::log("SENDER", "Recibi connection end de socket: " + std::to_string(recibido.socket) , DEBUG);

            FifoEscritura* fifo = fifosDeEnvio.at(recibido.socket);
            fifo->cerrar();
            delete fifo;
            fifosDeEnvio.erase(recibido.socket);

            pid_t pid = serverListeners.at(recibido.socket);
            kill(pid, SIGINT);
            serverListeners.erase(recibido.socket);

        } else if (recibido.mtype == TEXT) {
            respuesta = recibido.texto;
            respuesta.resize(recibido.msize);
            Logger::log("SENDER", "Recibi texto: " + respuesta + " de socket: " + std::to_string(recibido.socket) , DEBUG);
            //std::cout << "Sender: era texto: " << respuesta;
            //std::cout << ", tengo " << fifosDeEnvio.size() << " clientes" << std::endl;

            for (std::map<int, FifoEscritura*>::iterator it = fifosDeEnvio.begin();
                    it != fifosDeEnvio.end(); ++it) {
                //std::cout << "Escribi a " << it->first << std::endl;
                if(it->first != recibido.socket) {
                    FifoEscritura* fifo = it->second;
                    fifo->escribir(respuesta.c_str(), respuesta.size());
                }
            }
        } else {
            // error
            //std::cout << "Sender: era basura" << std::endl;
            Logger::log("SENDER", "Recibi basura" , DEBUG);
        }
    }

    Logger::log("SENDER", "Cierro todo" , DEBUG);
    for (std::map<int, pid_t>::iterator it = serverListeners.begin();
            it != serverListeners.end(); ++it) {
        //std::cout << "Escribi a " << it->first << std::endl;

        int status;
        pid_t pid = it->second;
        kill(pid, SIGINT);
        waitpid(pid, &status, 0);
    }

    colaDeRecibidos.destruir();
    unlink(NOM_SERVER_SENDER);
}

void ServerSocketSender::init() {
    close ( open ( NOM_SERVER_SENDER,O_CREAT,0777 ) );

    colaDeRecibidos.crear();
}
