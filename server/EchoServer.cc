#include <iostream>
#include "ServerSocket.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "defines.h"

int main () {

	SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

	try {
		ServerSocket socket ( SERVER_PORT );
		char buffer[BUFFSIZE];

		std::cout << "ConcuChatServer: esperando conexiones" << std::endl << std::endl;
		socket.abrirConexion();

		std::string peticion;
		int bytesRecibidos;

		while (!sigint_handler.signalWasReceived()) {
            // veo si alguien se quiere conectar
            socket.aceptarCliente();

			bytesRecibidos = -1;
			for(int i = 0; i < socket.getCantClientes(); ++i){

                // veo si alguien mando algo
                bytesRecibidos = socket.recibir ( static_cast<void*>(buffer),BUFFSIZE,i );
                // si no mandaron nada, sigo
                if (bytesRecibidos == -1) {
                    continue;
                }

                peticion = buffer;
                peticion.resize(bytesRecibidos);
                std::cout << "ConcuChatServer: dato recibido: '" << peticion <<
                                "' de cliente: '" << std::to_string(i) << "'" << std::endl;
                // si mandaron algo, veo que hago
                socket.handleInput(peticion, i);
            }
		}

		std::cout << "ConcuChatServer: cerrando conexion" << std::endl;
		socket.cerrarConexion ();

	} catch ( std::string& mensaje ) {
		std::cout << mensaje << std::endl;
	}

	return 0;
}

