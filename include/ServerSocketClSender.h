#ifndef SERVERSOCKETCLSENDER_H
#define SERVERSOCKETCLSENDER_H

#include "Runnable.h"
#include "FifoLectura.h"
#include <string>

class ServerSocketClSender : public Runnable
{
    public:
        ServerSocketClSender(int clientSocket, std::string nombre);
        virtual ~ServerSocketClSender();
    protected:

        int clientSocket;
        std::string nombre;
        FifoLectura fifoEnviar;

        void _run();
        void init();
        int enviar ( const void* buffer,const unsigned int buffSize );
};

#endif // SERVERSOCKETCLSENDER_H
