#include "ClientListener.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "defines.h"
#include "Logger.h"
#include <iostream>

ClientListener::ClientListener(int serverSocket) : serverSocket(serverSocket)
{
    //ctor
}

ClientListener::~ClientListener()
{
    //dtor
}

int ClientListener :: recibir ( void* buffer,const unsigned int buffSize ) {
	int cantBytes = read ( this->serverSocket,buffer,buffSize );
	return cantBytes;
}

void ClientListener :: _run() {

    SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    char bufferRta[BUFFSIZE];
    //std::cout << "EchoClient: empiezo a escuchar al servidor" << std::endl;
    Logger::log("C_LISN", "Empiezo a escuchar al servidor" , DEBUG);
    while(!sigint_handler.signalWasReceived()) {
        int longRta = recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
        if (!sigint_handler.signalWasReceived()) {
            std::string rta = bufferRta;
            rta.resize(longRta);
            Logger::log("C_LISN", "Respuesta de servidor: " + rta , DEBUG);
            std::cout << rta /*<< std::endl*/;

            if(rta == EXIT_MESSAGE){
                kill(getppid(), SIGINT);
                raise(SIGINT);
            }
        }
    }
}

void ClientListener :: init() {

}
