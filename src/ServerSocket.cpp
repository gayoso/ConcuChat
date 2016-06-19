#include "ServerSocket.h"
#include "ServerSocketListener.h"
#include "ServerSocketClSender.h"
#include "ServerSocketSender.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

ServerSocket :: ServerSocket ( const unsigned int port ) : Socket ( port ), clientCount(0) {
    Logger::log("SERVER", "Creo sender", DEBUG);
	ServerSocketSender sender;
	serverSender = sender.run();
}

ServerSocket :: ~ServerSocket () {
}

void ServerSocket :: abrirConexion () {

    Logger::log("SERVER", "Empiezo listen", DEBUG);

	// el servidor aceptara conexiones de cualquier cliente
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;

	int bindOk = bind ( this->fdSocket,
						(struct sockaddr *)&(this->serv_addr),
						sizeof(this->serv_addr) );
	if ( bindOk < 0 ) {
		std::string mensaje = std::string("Error en bind(): ") + std::string(strerror(errno));
		throw mensaje;
	}

	int listenOk = listen ( this->fdSocket,CONEXIONES_PENDIENTES );
	if ( listenOk < 0 ) {
		std::string mensaje = std::string("Error en listen(): ") + std::string(strerror(errno));
		throw mensaje;
	}

	//std::cout << "termine listen" << std::endl;
	Logger::log("SERVER", "Termine listen", DEBUG);
}

void ServerSocket :: aceptarCliente() {

    struct sockaddr_in cli_addr;
    int longCliente = sizeof ( cli_addr );
	//std::cout << "empiezo accept" << std::endl;
	Logger::log("SERVER", "Empiezo accept", DEBUG);

	int clientSocket = accept ( this->fdSocket,
									(struct sockaddr *)&cli_addr,
									(socklen_t *)&longCliente );

    //std::cout << "termine accept" << std::endl;
    Logger::log("SERVER", "Termine accept", DEBUG);

	if ( clientSocket < 0 ) {
		std::string mensaje = std::string("Error en accept(): ") + std::string(strerror(errno));
		//throw mensaje;
		Logger::log("SERVER", "Accept interrumpido", DEBUG);
	} else {
        //std::cout << "acepto en socket " << clientSocket << std::endl;
        Logger::log("SERVER", "Accepto en socket " + std::to_string(clientSocket), DEBUG);

        ServerSocketListener listener(clientSocket, NOM_SERVER_SENDER + std::to_string(++clientCount));
        pid_t pid = listener.run();

        //close(clientSocket);
	}
}

void ServerSocket :: cerrarConexion () {
    Logger::log("SERVER", "Cierro sender", DEBUG);
	int status;
	kill(serverSender, SIGINT);
	waitpid(serverSender, &status, 0);
	close ( this->fdSocket );
}
