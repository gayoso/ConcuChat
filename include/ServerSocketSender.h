#ifndef SERVERSOCKETSENDER_H
#define SERVERSOCKETSENDER_H

#include "defines.h"
#include "MemoriaCompartida.h"
#include "Semaforo.h"
#include "Cola.h"
#include "Socket.h"
#include "Runnable.h"
#include "ServerMensaje.h"
#include "FifoEscritura.h"
#include <map>
#include <set>
#include <vector>

class ServerSocketSender : public Runnable
{
    public:

        ServerSocketSender();

    protected:

        //std::vector<int> clientSockets;
        Cola<mensaje> colaDeRecibidos;
        std::map<int, FifoEscritura*> fifosDeEnvio;
        std::map<int, pid_t> serverListeners;
        std::set<std::string> nicknames;
        std::map<int, std::string> nicknamesBySocket;
        std::vector<std::string> historial;
        MemoriaCompartida<int> socketToClose;
        Semaforo sem_socketToClose;

        void _run();
        void init();

        void forwardAll(std::string respuesta);
        void forwardAllButSender(std::string respuesta, int sender);
        void addToChatLog(std::string respuesta);
        void emptyLogToFile();

    private:

};

#endif // SERVERSOCKETSENDER_H
