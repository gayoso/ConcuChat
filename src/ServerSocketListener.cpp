#include "ServerSocketListener.h"
#include "defines.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include <iostream>

ServerSocketListener::ServerSocketListener(int clientSocket) : Socket(SERVER_PORT)
{
    this->clientSocket = clientSocket;
}

ServerSocketListener::~ServerSocketListener()
{
    //dtor
}

void ServerSocketListener::_run() {

    SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    char buffer[BUFFSIZE];
    std::string peticion;
    while(!sigint_handler.signalWasReceived()) {
        int bytesRecibidos = this->recibir ( static_cast<void*>(buffer),BUFFSIZE );
        if (!sigint_handler.signalWasReceived()) {
            peticion = buffer;
            peticion.resize(bytesRecibidos);
            std::cout << "EchoServer: " << getpid() << ": dato recibido: " << peticion << std::endl;

            std::cout << "EchoServer: " << getpid() << ": enviando respuesta . . ." << std::endl;
            this->enviar ( static_cast<const void*>(peticion.c_str()),peticion.size() );
        }
    }
    cerrarConexion();
}

void ServerSocketListener::init() {

}

int ServerSocketListener :: recibir ( void* buffer,const unsigned int buffSize ) {
	int cantBytes = read ( this->clientSocket,buffer,buffSize );
	return cantBytes;
}

int ServerSocketListener :: enviar ( const void* buffer,const unsigned int buffSize ) {
	int cantBytes = write ( this->clientSocket,buffer,buffSize );
	return cantBytes;
}

void ServerSocketListener :: cerrarConexion () {
	//close ( this->clientSocket );
	close ( this->clientSocket );
}
