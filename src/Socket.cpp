#include "Socket.h"
#include "unistd.h"
#include "fcntl.h"

Socket :: Socket ( const unsigned int port ) {

	this->fdSocket = socket ( AF_INET,SOCK_STREAM,0 );
	if ( this->fdSocket < 0 )
		throw "Error al crear el socket";

	// se inicializa la estructura de la direccion
	bzero ( (char *)&(this->serv_addr),sizeof(this->serv_addr) );
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_port = htons ( port );
}

Socket::~Socket () {
}

void Socket::setNonBlocking() {
    long arg = fcntl(this->fdSocket, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(this->fdSocket, F_SETFL, arg);
}

void Socket::setBlocking() {
    long arg = fcntl(this->fdSocket, F_GETFL, NULL);
    arg &= ~O_NONBLOCK;
    fcntl(this->fdSocket, F_SETFL, arg);
}
