#ifndef SERVERSOCKETLISTENER_H
#define SERVERSOCKETLISTENER_H

#include "Runnable.h"
#include "Cola.h"
#include "ServerMensaje.h"

class ServerSocketListener : public Runnable
{
    public:

        ServerSocketListener(int clientSocket, std::string nombre);
        virtual ~ServerSocketListener();

    protected:

        int clientSocket;
        std::string nombre;
        Cola<mensaje> colaDeEnvio;

        void _run();
        void init();
        int recibir ( void* buffer,const unsigned int buffSize );
        void cerrarConexion ();
};

#endif // SERVERSOCKETLISTENER_H
