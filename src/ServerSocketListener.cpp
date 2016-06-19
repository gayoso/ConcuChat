#include "ServerSocketListener.h"
#include "defines.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "ServerMensaje.h"
#include "ServerSocketClSender.h"
#include "Logger.h"
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

    Logger::log(nombre + "_L", "Creo cl sender" , DEBUG);
    ServerSocketClSender sender(clientSocket, nombre);
    clientSender = sender.run();

    char buffer[BUFFSIZE];
    std::string peticion;

    Logger::log(nombre + "_L", "Mando CONN START a sender" , DEBUG);
    mensaje recibido;
    recibido.mtype = CONNECTION_START;
    recibido.socket = this->clientSocket;
    peticion = nombre + SEPARATOR + std::to_string(getpid());
    strcpy ( recibido.texto,peticion.c_str() );
    colaDeEnvio.escribir(recibido);

    while(!sigint_handler.signalWasReceived()) {
        int bytesRecibidos = this->recibir ( static_cast<void*>(buffer),BUFFSIZE );
        if (!sigint_handler.signalWasReceived()) {
            peticion = buffer;
            peticion.resize(bytesRecibidos);
            //std::cout << "ServerListener: " << getpid() << ": dato recibido: " << peticion;
            //std::cout << ", de : " << clientSocket << std::endl;
            Logger::log(nombre + "_L", "recibi " + peticion + " de " + std::to_string(clientSocket) , DEBUG);

            recibido.mtype = (peticion == EXIT_MESSAGE? CONNECTION_END : TEXT);
            recibido.socket = this->clientSocket;
            recibido.msize = bytesRecibidos;
            strcpy ( recibido.texto,peticion.c_str() );

            Logger::log(nombre + "_L", "Forwardeo a sender" , DEBUG);
            colaDeEnvio.escribir(recibido);

            if (peticion == EXIT_MESSAGE) {
                raise(SIGINT);
            }
        }
    }

    Logger::log(nombre + "_L", "Cierro todo" , DEBUG);
    kill(clientSender, SIGINT);
    cerrarConexion();
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
