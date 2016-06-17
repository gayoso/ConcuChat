#include <iostream>
#include "ServerSocket.h"
#include "defines.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include <vector>

int main () {

    SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    ServerSocket socket ( SERVER_PORT );
    socket.abrirConexion();

    //std::vector<pid_t> serverListeners;

    //char buffer[BUFFSIZE];
    std::cout << "EchoServer: esperando conexiones" << std::endl;
    try {
        while (!sigint_handler.signalWasReceived()) {
            socket.aceptarCliente();
            //ServerSocketListener listener(socket.client_socket);
            //serverListeners.push_back(listener.run());
        }
    } catch ( std::string& mensaje ) {
		std::cout << mensaje << std::endl;
	}

	socket.cerrarConexion();

	/*try {
		ServerSocket socket ( SERVER_PORT );
		char buffer[BUFFSIZE];

		std::cout << "EchoServer: esperando conexiones" << std::endl;
		std::cout << "EchoServer: enviar la cadena 's' desde el cliente para terminar" << std::endl << std::endl;
		socket.abrirConexion();
		socket.aceptarCliente();

		std::string peticion;

		do {
			int bytesRecibidos = socket.recibir ( static_cast<void*>(buffer),BUFFSIZE );
			peticion = buffer;
			peticion.resize(bytesRecibidos);
			std::cout << "EchoServer: dato recibido: " << peticion << std::endl;

			std::cout << "EchoServer: enviando respuesta . . ." << std::endl;
			socket.enviar ( static_cast<const void*>(peticion.c_str()),peticion.size() );
		} while ( peticion != std::string("s") );

		std::cout << "EchoServer: cerrando conexion" << std::endl;
		socket.cerrarConexion ();

	} catch ( std::string& mensaje ) {
		std::cout << mensaje << std::endl;
	}*/

	return 0;
}
