#include "ServerSocketSender.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sys/wait.h>

ServerSocketSender::ServerSocketSender()
    : colaDeRecibidos(NOM_SERVER_SENDER, C_SENDER_RECIBIDOS),
        socketToClose(NOM_SERVER_SOCKETS, C_SERVER_SOCKETS),
        sem_socketToClose(NOM_SERVER_SOCKETS, C_SERVER_SOCKETS, 0)
{

}

void ServerSocketSender::forwardAll(std::string respuesta) {
    forwardAllButSender(respuesta, -1);
}

void ServerSocketSender::forwardAllButSender(std::string respuesta, int sender) {
    addToChatLog(respuesta);
    for (std::map<int, FifoEscritura*>::iterator it = fifosDeEnvio.begin();
                    it != fifosDeEnvio.end(); ++it) {
        if(it->first != sender &&
            nicknamesBySocket.find(it->first) != nicknamesBySocket.end()) {
            FifoEscritura* fifo = it->second;
            fifo->escribir(respuesta.c_str(), respuesta.size());
        }
    }
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

        if (recibido.mtype == CONNECTION_START) {
            Logger::log("SENDER", "Recibi connection start de socket: " + std::to_string(recibido.socket) , DEBUG);

            respuesta = recibido.texto;
            respuesta.resize(recibido.msize);
            int first_sep = respuesta.find_first_of(SEPARATOR, 0);

            std::string nombreFifo = respuesta.substr(0, first_sep);

            std::string pidString = respuesta.substr(first_sep+1, respuesta.size());
            pid_t pidListener = std::stoi(pidString, 0, 10);

            serverListeners.insert(std::pair<int, pid_t>(recibido.socket, pidListener));

            FifoEscritura* fifo = new FifoEscritura(nombreFifo);
            fifo->abrir();
            fifosDeEnvio.insert(std::pair<int, FifoEscritura*>(recibido.socket, fifo));

        } else if (recibido.mtype == NICKNAME_REQ) {
            Logger::log("SENDER", "Recibi nickname request de socket: " + std::to_string(recibido.socket) , DEBUG);

            std::string nickname = recibido.texto;
            nickname.resize(recibido.msize);

            if (nicknames.find(nickname) == nicknames.end()) {
                respuesta = "OK";
                nicknames.insert(nickname);
                nicknamesBySocket.insert(std::pair<int, std::string>(recibido.socket, nickname));

                forwardAllButSender("--- ingreso '" + nickname + "' ---\n", recibido.socket);
            } else {
                respuesta = "NICKNAME_TAKEN";
            }

            FifoEscritura* fifo = fifosDeEnvio.at(recibido.socket);
            fifo->escribir(respuesta.c_str(), respuesta.size());

        } else if (recibido.mtype == GET_LOG) {
            Logger::log("SENDER", "Recibi get log: " + std::to_string(recibido.socket) , DEBUG);

            FifoEscritura* fifo = fifosDeEnvio.at(recibido.socket);
            for(int i = 0; i < historial.size(); ++i) {
                fifo->escribir(historial[i].c_str(), historial[i].size());
            }

        } else if (recibido.mtype == CONNECTION_END) {
            Logger::log("SENDER", "Recibi connection end de socket: " + std::to_string(recibido.socket) , DEBUG);

            FifoEscritura* fifo = fifosDeEnvio.at(recibido.socket);
            fifo->cerrar();
            delete fifo;
            fifosDeEnvio.erase(recibido.socket);

            pid_t pid = serverListeners.at(recibido.socket);
            kill(pid, SIGINT);
            serverListeners.erase(recibido.socket);

            std::map<int, std::string>::iterator it = nicknamesBySocket.find(recibido.socket);
            if(it != nicknamesBySocket.end()){
                forwardAllButSender("--- se fue '" + it->second + "' ---\n", recibido.socket);
                nicknames.erase(it->second);
                nicknamesBySocket.erase(recibido.socket);
            }

            socketToClose.escribir(recibido.socket);
            kill(getppid(), SIGUSR1);
            sem_socketToClose.p();

        } else if (recibido.mtype == TEXT) {

            std::string nickname = recibido.nickname;
            nickname.resize(recibido.nsize);
            std::string message = recibido.texto;
            message.resize(recibido.msize);

            respuesta = nickname + ": " + message + "\n";
            Logger::log("SENDER", "Recibi texto: '" +
                respuesta.substr(0, std::min((int)respuesta.size()-1, 10)) +
                "...' de socket: " + std::to_string(recibido.socket) , DEBUG);

            forwardAllButSender(respuesta, recibido.socket);

        } else {
            Logger::log("SENDER", "Recibi basura" , DEBUG);
        }
    }

    Logger::log("SENDER", "Cierro todo" , DEBUG);
    for (std::map<int, pid_t>::iterator it = serverListeners.begin(); it != serverListeners.end(); ++it) {
        int status;
        pid_t pid = it->second;
        kill(pid, SIGINT);
        //waitpid(pid, &status, 0);
    }

    emptyLogToFile();
    //socketToClose.liberar();
    colaDeRecibidos.destruir();
    unlink(NOM_SERVER_SENDER);
}

void ServerSocketSender::addToChatLog(std::string respuesta) {
    historial.push_back(respuesta);
}

void ServerSocketSender::emptyLogToFile() {
    std::ofstream chat_log;
    chat_log.open("chat_log.txt", std::ofstream::out);
    for(int i = 0; i < historial.size(); ++i) {
        chat_log << historial[i];
    }
    chat_log.close();
}

void ServerSocketSender::init() {
    close ( open ( NOM_SERVER_SENDER,O_CREAT,0777 ) );

    socketToClose.get();
    sem_socketToClose.get();
    colaDeRecibidos.crear();
}
