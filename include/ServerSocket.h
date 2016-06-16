#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <vector>

#include "Socket.h"

class ServerSocket : public Socket {

	private:
		std::vector<int> clientSockets;
		std::vector<std::string> usernames;

		static const int CONEXIONES_PENDIENTES = 20;

	public:
		ServerSocket ( const unsigned int port );
		~ServerSocket ();

		void abrirConexion ();
		void aceptarCliente();

		bool isValidUsername(std::string username);
		void registerUsername(std::string username, int clientIndex);

		std::string getUsername(std::string message);
        std::string getUsername(const void* buffer,const unsigned int buffSize);
        std::string getMessage(std::string peticion);

        bool isExitCode(std::string message);

		int enviar ( const void* buffer,const unsigned int buffSize );
		int enviar ( const void* buffer,const unsigned int buffSize, int clientIndex );
		int recibir ( void* buffer,const unsigned int buffSize, int clientIndex );
		void handleInput(std::string peticion, int clientIndex);

		void cerrarConexion ();
		void cerrarConexion ( unsigned int clientIndex );

		int getCantClientes() { return clientSockets.size(); }
};

#endif /* SERVERSOCKET_H_ */
