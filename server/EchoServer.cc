#include <iostream>
#include "ServerSocket.h"
#include "ServerSocketSender.h"
#include "defines.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "Logger.h"
#include <vector>
#include <sys/wait.h>

int main () {

    SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    ServerSocket socket ( SERVER_PORT );
    socket.abrirConexion();

    Logger::log("MAIN_SV", "Arranco server" , DEBUG);
    //std::cout << "EchoServer: esperando conexiones" << std::endl;
    try {
        while (!sigint_handler.signalWasReceived()) {
            socket.aceptarCliente();
        }
    } catch ( std::string& mensaje ) {
		std::cout << mensaje << std::endl;
	}

    //std::cout << "EchoServer: cerrando accepter" << std::endl;
    Logger::log("MAIN_SV", "Cierro server" , DEBUG);
	socket.cerrarConexion();
	Logger::log("MAIN_SV", "Fin del programa" , DEBUG);

	return 0;
}
