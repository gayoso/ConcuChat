#include <iostream>
#include "ServerSocket.h"
#include "ServerSocketSender.h"
#include "defines.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include <vector>
#include <sys/wait.h>

int main () {

    SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    ServerSocket socket ( SERVER_PORT );
    ServerSocketSender sender;
    pid_t pidSender = sender.run();
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

    int status;
    kill(pidSender, SIGINT);
    waitpid(pidSender, &status, 0);
	socket.cerrarConexion();

	return 0;
}
