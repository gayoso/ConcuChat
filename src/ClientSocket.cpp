#include "ClientSocket.h"
#include "ClientListener.h"
//#include <sys/types.h>
//#include <unistd.h>
#include <sys/wait.h>

ClientSocket :: ClientSocket ( const std::string& ipServidor,const unsigned int port ) : Socket ( port ) {
	this->ipServidor = ipServidor;
}

ClientSocket :: ~ClientSocket () {
}

void ClientSocket :: abrirConexion () {

	struct hostent *server = gethostbyname ( this->ipServidor.c_str() );
	if ( server == NULL ) {
		std::string mensaje = std::string("No se puede localizar el host: ") + std::string(strerror(errno));
		throw mensaje;
	}

    bcopy ( (char *)server->h_addr,
    		(char *)&(this->serv_addr.sin_addr.s_addr),
    		server->h_length );

    int connectOk = connect ( this->fdSocket,
    						(const struct sockaddr *)&(this->serv_addr),
    						sizeof(this->serv_addr) );
    if ( connectOk < 0 ) {
    	std::string mensaje = std::string("Error en connect(): ") + std::string(strerror(errno));
    	throw mensaje;
    }
}

void ClientSocket :: startListener() {
    ClientListener listener(this->fdSocket);
    clientListener = listener.run();
}

int ClientSocket :: enviar ( const void* buffer,const unsigned int buffSize ) {
	int cantBytes = write ( this->fdSocket,buffer,buffSize );
	return cantBytes;
}

int ClientSocket :: recibir ( void* buffer,const unsigned int buffSize ) {
	int cantBytes = read ( this->fdSocket,buffer,buffSize );
	return cantBytes;
}

void ClientSocket :: cerrarConexion () {
	close ( this->fdSocket );
	int status;
	kill(clientListener, SIGINT);
	waitpid(clientListener, &status, 0);
}
