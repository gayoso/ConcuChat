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
    SIG_Trap sigusr1_handler(SIGUSR1);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);
    SignalHandler::getInstance()->registrarHandler(SIGUSR1, &sigusr1_handler);

    ServerSocket socket ( SERVER_PORT );
    socket.abrirConexion();

    Logger::log("MAIN_SV", "Arranco server" , DEBUG);
    //std::cout << "EchoServer: esperando conexiones" << std::endl;
    while (!sigint_handler.signalWasReceived()) {
        socket.aceptarCliente();
        if (sigusr1_handler.signalWasReceived()) {
            //std::cout << "Main: me llego sigusr1" << std::endl;
            socket.cerrarSocketsNoUsados();
            sigusr1_handler.reset();
        }
    }

    //std::cout << "EchoServer: cerrando accepter" << std::endl;
    Logger::log("MAIN_SV", "Cierro server" , DEBUG);
	socket.cerrarConexion();
	Logger::log("MAIN_SV", "Fin del programa" , DEBUG);

	return 0;
}
