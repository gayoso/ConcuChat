#include "ServerSocketListener.h"
#include "defines.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "ServerMensaje.h"
#include <iostream>

ServerSocketListener::ServerSocketListener(int clientSocket, std::string nombre) :
    colaDeEnvio(NOM_SERVER_SENDER, C_SENDER_RECIBIDOS), nombre(nombre)
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

    colaDeEnvio.get();

    char buffer[BUFFSIZE];
    std::string peticion;
    mensaje recibido;
    recibido.mtype = CONNETION_START;
    recibido.socket = this->clientSocket;
    strcpy ( recibido.texto,nombre.c_str() );
    colaDeEnvio.escribir(recibido);

    while(!sigint_handler.signalWasReceived()) {
        int bytesRecibidos = this->recibir ( static_cast<void*>(buffer),BUFFSIZE );
        if (!sigint_handler.signalWasReceived()) {
            peticion = buffer;
            peticion.resize(bytesRecibidos);
            std::cout << "ServerListener: " << getpid() << ": dato recibido: " << peticion;
            std::cout << ", de : " << clientSocket << std::endl;

            if (peticion == EXIT_MESSAGE) {
                recibido.mtype = CONNECTION_END;
            } else {
                recibido.mtype = TEXT;
            }
            recibido.socket = this->clientSocket;
            //recibido.msize = bytesRecibidos;
            strcpy ( recibido.texto,peticion.c_str() );

            colaDeEnvio.escribir(recibido);

            if (peticion == EXIT_MESSAGE) {
                raise(SIGINT);
            }
            //std::cout << "EchoServer: " << getpid() << ": enviando respuesta . . ." << std::endl;
            //this->enviar ( static_cast<const void*>(peticion.c_str()),peticion.size() );
        }
    }
    //cerrarConexion();
}

void ServerSocketListener::init() {

}

int ServerSocketListener :: recibir ( void* buffer,const unsigned int buffSize ) {
	int cantBytes = read ( this->clientSocket,buffer,buffSize );
	return cantBytes;
}

void ServerSocketListener :: cerrarConexion () {
	//close ( this->clientSocket );
	close ( this->clientSocket );
}
