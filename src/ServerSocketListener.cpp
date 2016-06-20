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
    recibido.msize = peticion.size();
    colaDeEnvio.escribir(recibido);

    while(!sigint_handler.signalWasReceived()) {
        int bytesRecibidos = this->recibir ( static_cast<void*>(buffer),BUFFSIZE );
        if (!sigint_handler.signalWasReceived()) {
            peticion = buffer;
            peticion.resize(bytesRecibidos);
            std::string nickname = peticion.substr(0, peticion.find_first_of(":"));
            std::string message = peticion.substr(peticion.find_first_of(":")+2, peticion.size());
            //std::cout << "ServerListener: " << getpid() << ": dato recibido: '" << peticion << "'";
            //std::cout << ", de : " << clientSocket << std::endl;
            Logger::log(nombre + "_L", "recibi " + peticion + " de " + std::to_string(clientSocket) , DEBUG);

            if (message == EXIT_MESSAGE) {
                recibido.mtype = CONNECTION_END;
            } else if (nickname == "_client") {
                recibido.mtype = NICKNAME_REQ;
            } else if (message == SEPARATOR){
                recibido.mtype = GET_LOG;
            } else {
                recibido.mtype = TEXT;
            }
            recibido.socket = this->clientSocket;
            recibido.msize = message.size();
            strcpy ( recibido.texto,message.c_str() );
            recibido.nsize = nickname.size();
            strcpy ( recibido.nickname,nickname.c_str() );

            Logger::log(nombre + "_L", "Forwardeo a sender" , DEBUG);
            colaDeEnvio.escribir(recibido);

            if (message == EXIT_MESSAGE) {
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
