#include <iostream>
#include <string.h>
#include "defines.h"
#include "ClientSocket.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "Logger.h"

int main ( int argc,char *argv[] ) {

	if ( argc != 2 ) {
		std::cout << "Uso: ./EchoClient direccion_ip_servidor" << std::endl << std::endl;
		return -1 ;
	}

	SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

	try {
		ClientSocket socket ( argv[1],SERVER_PORT );

		char entrada[BUFFSIZE];

		Logger::log("MAIN_CL", "Abriendo cliente" , DEBUG);

		std::cout << "EchoClient: abriendo conexion con el servidor " << argv[1] << std::endl;
		std::cout << "EchoClient: conexion abierta.  Ingresar el texto a enviar y presionar [ENTER]" << std::endl << std::endl;
		socket.abrirConexion();

		std::string mensaje;
		while(!sigint_handler.signalWasReceived()) {
			std::cin.getline ( entrada,BUFFSIZE );

            if(!sigint_handler.signalWasReceived()){
                //std::cout << "EchoClient: enviando dato al servidor: " << entrada << std::endl;
                mensaje = entrada;
                socket.enviar ( static_cast<const void*>(entrada),mensaje.size() );
                //std::cout << "ya envie" << std::endl;
            }
		}

		//std::cout << "EchoClient: cerrando la conexion" << std::endl;
		Logger::log("MAIN_CL", "Cerrando cliente" , DEBUG);
		//kill(pid, SIGINT);
		mensaje = EXIT_MESSAGE;
		socket.enviar(static_cast<const void*>(mensaje.c_str()),mensaje.size());
		//int longRta = socket.recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
		socket.cerrarConexion ();
		//std::cout << "EchoClient: fin del programa" << std::endl;
		Logger::log("MAIN_CL", "Fin del programa" , DEBUG);

	} catch ( std::string& mensaje ) {
		std::cout << mensaje << std::endl;
	}

	return 0;
}
