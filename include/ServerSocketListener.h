#ifndef SERVERSOCKETLISTENER_H
#define SERVERSOCKETLISTENER_H

#include "ServerSocket.h"
#include "Runnable.h"

class ServerSocketListener : public Socket, public Runnable
{
    public:

        ServerSocketListener(int clientSocket);
        virtual ~ServerSocketListener();

    protected:

        int clientSocket;

        void _run();
        void init();
        int recibir ( void* buffer,const unsigned int buffSize );
        int enviar ( const void* buffer,const unsigned int buffSize );
        void cerrarConexion ();
};

#endif // SERVERSOCKETLISTENER_H
