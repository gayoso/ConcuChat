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

class ServerSocketSender : public Socket, public Runnable
{
    public:

        ServerSocketSender();

    protected:

        //std::vector<int> clientSockets;
        Cola<mensaje> colaDeRecibidos;
        std::map<int, FifoEscritura*> fifosDeEnvio;
        std::map<int, pid_t> serverListeners;

        void _run();
        void init();

        int enviar ( const void* buffer,const unsigned int buffSize, int index );

    private:

};

#endif // SERVERSOCKETSENDER_H
