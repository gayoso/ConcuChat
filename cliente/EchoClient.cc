#include <iostream>
#include <string.h>
#include "defines.h"
#include "ClientSocket.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"

int main ( int argc,char *argv[] ) {

	if ( argc != 2 ) {
		std::cout << "Uso: ./EchoClient direccion_ip_servidor" << std::endl << std::endl;
		return -1 ;
	}

	SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

	try {
		ClientSocket socket ( argv[1],SERVER_PORT );
		char bufferRta[BUFFSIZE];
		char entrada[BUFFSIZE];

		std::cout << "EchoClient: abriendo conexion con el servidor " << argv[1] << std::endl;
		std::cout << "EchoClient: conexion abierta.  Ingresar el texto a enviar y presionar [ENTER].  s para salir " << std::endl << std::endl;
		socket.abrirConexion();

		pid_t pid = fork();
		if(pid == 0) {
            std::cout << "EchoClient: empiezo a escuchar al servidor" << std::endl;
            while(!sigint_handler.signalWasReceived()) {
                int longRta = socket.recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
                if (!sigint_handler.signalWasReceived()) {
                    std::string rta = bufferRta;
                    rta.resize(longRta);
                    std::cout << "EchoClient: respuesta recibida del servidor: " << rta << std::endl;
                }
			}
			exit(0);
		}

		std::string mensaje;
		while(!sigint_handler.signalWasReceived()) {
			std::cin.getline ( entrada,BUFFSIZE );

            if(!sigint_handler.signalWasReceived()){
                std::cout << "EchoClient: enviando dato al servidor: " << entrada << std::endl;
                mensaje = entrada;
                socket.enviar ( static_cast<const void*>(entrada),mensaje.size() );
                std::cout << "ya envie" << std::endl;
            }
		}

		std::cout << "EchoClient: cerrando la conexion" << std::endl;
		kill(pid, SIGINT);
		mensaje = EXIT_MESSAGE;
		socket.enviar(static_cast<const void*>(mensaje.c_str()),mensaje.size());
		int longRta = socket.recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
		socket.cerrarConexion ();
		std::cout << "EchoClient: fin del programa" << std::endl;

	} catch ( std::string& mensaje ) {
		std::cout << mensaje << std::endl;
	}

	return 0;
}
