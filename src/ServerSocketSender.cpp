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
            respuesta.resize(recibido.msize);
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

        } else if (recibido.mtype == NICKNAME_REQ) {
            Logger::log("SENDER", "Recibi nickname request de socket: " + std::to_string(recibido.socket) , DEBUG);

            std::string nickname = recibido.texto;
            nickname.resize(recibido.msize);

            //std::cout << "Sender: me piden nickname: " << nickname << std::endl;

            FifoEscritura* fifo = fifosDeEnvio.at(recibido.socket);

            if (nicknames.find(nickname) == nicknames.end()) {
                respuesta = "OK";
                nicknames.insert(nickname);
                nicknamesBySocket.insert(std::pair<int, std::string>(recibido.socket, nickname));
            } else {
                respuesta = "NICKNAME_TAKEN";
            }

            fifo->escribir(respuesta.c_str(), respuesta.size());

        } else if (recibido.mtype == CONNECTION_END) {
            //std::cout << "Sender: era connection end" << std::endl;
            Logger::log("SENDER", "Recibi connection end de socket: " + std::to_string(recibido.socket) , DEBUG);

            std::string nickname = recibido.nickname;
            nickname.resize(recibido.nsize);
            std::string message = recibido.texto;
            message.resize(recibido.msize);

            FifoEscritura* fifo = fifosDeEnvio.at(recibido.socket);
            fifo->cerrar();
            delete fifo;
            fifosDeEnvio.erase(recibido.socket);

            pid_t pid = serverListeners.at(recibido.socket);
            kill(pid, SIGINT);
            serverListeners.erase(recibido.socket);

            std::map<int, std::string>::iterator it = nicknamesBySocket.find(recibido.socket);
            if(it != nicknamesBySocket.end()){
                nicknames.erase(it->second);
                nicknamesBySocket.erase(recibido.socket);
            }

        } else if (recibido.mtype == TEXT) {

            std::string nickname = recibido.nickname;
            nickname.resize(recibido.nsize);
            std::string message = recibido.texto;
            message.resize(recibido.msize);

            respuesta = nickname + ": " + message;
            //respuesta.resize(recibido.msize);
            Logger::log("SENDER", "Recibi texto: " + respuesta + " de socket: " + std::to_string(recibido.socket) , DEBUG);
            //std::cout << "Sender: era texto: " << respuesta;
            //std::cout << ", tengo " << fifosDeEnvio.size() << " clientes" << std::endl;
            //std::cout << "Recibi texto: '" << respuesta << "' de socket: " << std::to_string(recibido.socket) << std::endl;

            for (std::map<int, FifoEscritura*>::iterator it = fifosDeEnvio.begin();
                    it != fifosDeEnvio.end(); ++it) {
                //std::cout << "Escribi a " << it->first << std::endl;
                if(it->first != recibido.socket &&
                    nicknamesBySocket.find(it->first) != nicknamesBySocket.end()) {
                    //std::cout << "Escribi : " << respuesta << " a " << it->first << std::endl;
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
        //std::cout << "Cierro " << it->second << std::endl;

        int status;
        pid_t pid = it->second;
        kill(pid, SIGINT);
        waitpid(pid, &status, 0);
    }

    //std::cout << "cierro cola de recibidos" << std::endl;
    colaDeRecibidos.destruir();
    //std::cout << "holalss" << std::endl;
    unlink(NOM_SERVER_SENDER);
}

void ServerSocketSender::init() {
    close ( open ( NOM_SERVER_SENDER,O_CREAT,0777 ) );

    colaDeRecibidos.crear();
}
