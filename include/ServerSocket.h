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

	protected:
		//int clientSocket;
		static const int CONEXIONES_PENDIENTES = 20;
		unsigned int clientCount;
        pid_t serverSender;

	public:
		ServerSocket ( const unsigned int port );
		~ServerSocket ();

		//int clientSocket;

		void abrirConexion ();
		void aceptarCliente();

		//int enviar ( const void* buffer,const unsigned int buffSize );
		//int recibir ( void* buffer,const unsigned int buffSize );

		void cerrarConexion ();
};

#endif /* SERVERSOCKET_H_ */
