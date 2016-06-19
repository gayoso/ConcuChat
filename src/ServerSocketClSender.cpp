#include "ServerSocketClSender.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "defines.h"
#include "Logger.h"
#include <iostream>

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

    Logger::log(nombre, "Empiezo cl sender", DEBUG);

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
            Logger::log(nombre, "Envio " + respuesta, DEBUG);
            //std::cout << "ServerClSender: envio " << respuesta << std::endl;
            enviar(respuesta.c_str(), respuesta.size());
        }
    }

    // envio exit message para que se desconecte el cliente
    respuesta = EXIT_MESSAGE;
    Logger::log(nombre, "Envio exit message" , DEBUG);
    enviar(respuesta.c_str(), respuesta.size());

    // cierro estructuras
    Logger::log(nombre, "Cierro estructuras temporales" , DEBUG);
    fifoEnviar.cerrar();
    fifoEnviar.eliminar();
    unlink(nombre.c_str());
    close(this->clientSocket);
}

void ServerSocketClSender::init() {
    Logger::log(nombre, "Creo estructuras temporales" , DEBUG);
    fifoEnviar.crear();
}

int ServerSocketClSender::enviar (const void* buffer,const unsigned int buffSize ) {
    int cantBytes = write ( this->clientSocket,buffer,buffSize );
	return cantBytes;
}
