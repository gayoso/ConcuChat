#include "ServerSocketClSender.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "defines.h"

ServerSocketClSender::ServerSocketClSender(int clientSocket, std::string nombre) :
    clientSocket(clientSocket), nombre(nombre), fifoEnviar(nombre)
{
    //ctor
}

ServerSocketClSender::~ServerSocketClSender()
{
    //dtor
}

void ServerSocketClSender::_run() {

    SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    fifoEnviar.abrir();

    char buffer[BUFFSIZE];
    std::string respuesta;
    while(!sigint_handler.signalWasReceived()) {
        ssize_t bytesLeidos = fifoEnviar.leer(buffer, BUFFSIZE);
        if (!sigint_handler.signalWasReceived()) {
            respuesta = buffer;
            respuesta.resize(bytesLeidos);
            enviar(respuesta.c_str(), respuesta.size());
            if(respuesta == "CONNETION_END") {
                raise(SIGINT);
            }
        }
    }

    fifoEnviar.cerrar();
    fifoEnviar.eliminar();
    unlink(nombre.c_str());
    close(this->clientSocket);
}

void ServerSocketClSender::init() {
    //close ( open ( nombre.c_str(),O_CREAT,0777 ) );

    fifoEnviar.crear();
}

int ServerSocketClSender::enviar (const void* buffer,const unsigned int buffSize ) {
    int cantBytes = write ( this->clientSocket,buffer,buffSize );
	return cantBytes;
}
