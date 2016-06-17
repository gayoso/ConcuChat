#include "ServerSocket.h"
#include "ServerSocketListener.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

ServerSocket :: ServerSocket ( const unsigned int port ) : Socket ( port ) {
	//this->clientSocket = -1;
}

ServerSocket :: ~ServerSocket () {
}

void ServerSocket :: abrirConexion () {

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

	std::cout << "termine listen" << std::endl;
}

void ServerSocket :: aceptarCliente() {

    struct sockaddr_in cli_addr;
    int longCliente = sizeof ( cli_addr );
	std::cout << "empiezo accept" << std::endl;

	int clientSocket = accept ( this->fdSocket,
									(struct sockaddr *)&cli_addr,
									(socklen_t *)&longCliente );

    std::cout << "termine accept" << std::endl;

	if ( clientSocket < 0 ) {
		std::string mensaje = std::string("Error en accept(): ") + std::string(strerror(errno));
		//throw mensaje;
	} else {
        ServerSocketListener listener(clientSocket);
        pid_t pid = listener.run();
        serverListeners.push_back(pid);
	}
}

void ServerSocket :: cerrarConexion () {
	//close ( this->clientSocket );
	int status;
	for(int i = 0; i < serverListeners.size(); ++i) {
        kill(serverListeners[i], SIGINT);
        waitpid(serverListeners[i], &status, 0);
	}
	close ( this->fdSocket );
}
