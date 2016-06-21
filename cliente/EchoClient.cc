#include <iostream>
#include <string.h>
#include "defines.h"
#include "ClientSocket.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "Logger.h"

int main ( int argc,char *argv[] ) {

	if ( argc != 3 ) {
		std::cout << "Uso: ./client 'direccion_ip_servidor' 'nickname'" << std::endl << std::endl;
		return -1 ;
	}

	SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);
    pid_t pid;

    ClientSocket socket ( argv[1],SERVER_PORT, argv[2] );

    Logger::log("MAIN_CL", "Abriendo cliente" , DEBUG);

    socket.abrirConexion();
    if(!sigint_handler.signalWasReceived()){
        socket.registrarNickname();
    }
    if (!sigint_handler.signalWasReceived()) {
        socket.loadChatHistory();
    }
    while (!sigint_handler.signalWasReceived()) {
        char entrada[BUFFSIZE];
        std::cin.getline ( entrada,BUFFSIZE );
        if(!sigint_handler.signalWasReceived())
            socket.handleInput(entrada);
    }
    socket.cerrarConexion();
    Logger::log("MAIN_CL", "Fin del programa" , DEBUG);

	return 0;
}
