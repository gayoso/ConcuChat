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

	try {
		ClientSocket socket ( argv[1],SERVER_PORT );

		Logger::log("MAIN_CL", "Abriendo cliente" , DEBUG);

		std::cout << "Client: abriendo conexion con el servidor " << argv[1] << std::endl;
		//std::cout << "EchoClient: conexion abierta.  Ingresar el texto a enviar y presionar [ENTER]" << std::endl << std::endl;
		socket.abrirConexion();

		std::string mensaje;
        std::string nickname = "_client";
        std::string nicknamePropuesto = argv[2];

        for(int i = 0; i < nicknamePropuesto.size(); ++i) {
            if(!isalnum(nicknamePropuesto[i])){
                std::cout << "Client: Error al ingresar: el nickname debe ser alfanumerico" << std::endl;
                raise(SIGINT);
            }
        }

        char bufferRta[BUFFSIZE];
        std::string rta;
        if (!sigint_handler.signalWasReceived()){
            mensaje = nickname + ": " + nicknamePropuesto;
            socket.enviar ( static_cast<const void*>(mensaje.c_str()),mensaje.size() );
            int longRta = socket.recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
            rta = bufferRta;
            rta.resize(longRta);
            if (rta == "OK") {
                nickname = nicknamePropuesto;
            } else {
                std::cout << "Client: Error al ingresar: " << rta << std::endl;
                raise(SIGINT);
            }
        }

        socket.startListener();
        char entrada[BUFFSIZE];
		while(!sigint_handler.signalWasReceived()) {
			std::cin.getline ( entrada,BUFFSIZE );

            if(!sigint_handler.signalWasReceived()) {
                //std::cout << "Client: enviando dato al servidor: " << entrada << std::endl;
                mensaje = nickname + ": " + entrada;
                socket.enviar ( static_cast<const void*>(mensaje.c_str()),mensaje.size() );
                //std::cout << "ya envie" << std::endl;
            }
		}

		//std::cout << "EchoClient: cerrando la conexion" << std::endl;
		Logger::log("MAIN_CL", "Cerrando cliente" , DEBUG);
		//kill(pid, SIGINT);
		mensaje = nickname + ": " + EXIT_MESSAGE;
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
