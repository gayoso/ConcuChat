#ifndef CLIENTLISTENER_H
#define CLIENTLISTENER_H

#include "Runnable.h"

class ClientListener : public Runnable
{
    public:

        ClientListener(int serverSocket);
        virtual ~ClientListener();

    protected:

        void _run();
        void init();
        int recibir ( void* buffer,const unsigned int buffSize );

        int serverSocket;
};

#endif // CLIENTLISTENER_H
