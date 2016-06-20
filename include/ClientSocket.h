#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Socket.h"

class ClientSocket : public Socket {

	private:
		std::string ipServidor;
		pid_t clientListener;
		std::string nickname;

	public:
		ClientSocket ( const std::string& ipServidor,const unsigned int port, std::string nickname );
		~ClientSocket ();

		void abrirConexion ();
		void registrarNickname();
		void loadChatHistory();
		void handleInput(std::string entrada);
		void startListener();

		int enviar ( const void* buffer,const unsigned int buffSize );
		int recibir ( void* buffer,const unsigned int buffSize );

		void cerrarConexion ();
};

#endif /* CLIENTSOCKET_H_ */
