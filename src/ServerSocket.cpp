#include "ServerSocket.h"
#include "defines.h"
#include <iostream>
#include <algorithm>
#include "unistd.h"
#include "fcntl.h"

ServerSocket :: ServerSocket ( const unsigned int port ) : Socket ( port ) {
	//this->nuevoFdSocket = -1;
	// set nonblocking
}

ServerSocket :: ~ServerSocket () {
}

void ServerSocket :: abrirConexion () {

	// el servidor aceptara conexiones de cualquier cliente
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;

	int bindOk = bind ( this->fdSocket,
						(struct sockaddr *)&(this->serv_addr),
						sizeof(this->serv_addr) );
	if ( bindOk < 0 ) {
		std::string mensaje = std::string("Error en bind(): ") + std::string(strerror(errno));
		throw mensaje;
	}

	int listenOk = listen ( this->fdSocket,CONEXIONES_PENDIENTES );
	if ( listenOk < 0 ) {
		std::string mensaje = std::string("Error en listen(): ") + std::string(strerror(errno));
		throw mensaje;
	}

	//std::cout << "termine listen" << std::endl;

	long arg = fcntl(this->fdSocket, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(this->fdSocket, F_SETFL, arg);
}

void ServerSocket :: aceptarCliente() {

    //std::cout << "empiezo accept" << std::endl;

    struct sockaddr_in cli_addr;
    int longCliente = sizeof ( cli_addr );

    int clientSocket = -1;
	clientSocket = accept ( this->fdSocket,
                                (struct sockaddr *)&cli_addr,
                                (socklen_t *)&longCliente );

	if ( clientSocket < 0 ) {
		std::string mensaje = std::string("Error en accept(): ") + std::string(strerror(errno));
		//std::cout << mensaje << std::endl;
		//throw mensaje;
	} else {
        clientSockets.push_back(clientSocket);
        usernames.push_back(CLIENT_ID);
	}

	//std::cout << "termine accept" << std::endl;

	long arg = fcntl(clientSocket, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(clientSocket, F_SETFL, arg);
}

bool ServerSocket :: isValidUsername(std::string username) {
    if(std::find(usernames.begin(), usernames.end(), username) != usernames.end()) {
        return false;
    }
    return true;
}

void ServerSocket :: registerUsername(std::string username, int clientIndex) {
    usernames[clientIndex] = username;
}

bool ServerSocket :: isExitCode(std::string message){
    return getMessage(message) == EXIT_MESSAGE;
}

std::string ServerSocket :: getMessage(std::string peticion) {
    return peticion.substr(peticion.find_first_of(" ")+1, peticion.size());
}

std::string ServerSocket :: getUsername(std::string message){
    return message.substr(0, message.find_first_of(":"));
}

std::string ServerSocket :: getUsername(const void* buffer,const unsigned int buffSize){
    std::string strMessage = (char*)buffer;
    strMessage.resize(buffSize);
    return getUsername(strMessage);
}

int ServerSocket :: enviar ( const void* buffer,const unsigned int buffSize ) {
    std::string username = getUsername(buffer, buffSize);
    //std::cout << username << std::endl;
    int cantBytes;
    for(int i = 0; i < usernames.size(); ++i){
        if (usernames[i] != CLIENT_ID && usernames[i] != username) {
            cantBytes = this->enviar(buffer, buffSize, i); // ver cantBytes si alguno no esta completo o algo
        }
	}
	return cantBytes;
}

int ServerSocket :: enviar ( const void* buffer,const unsigned int buffSize, int clientIndex ) {
	int cantBytes = write ( clientSockets[clientIndex], buffer, buffSize );
	return cantBytes;
}

int ServerSocket :: recibir ( void* buffer,const unsigned int buffSize, int clientIndex ) {
	int cantBytes = read ( clientSockets[clientIndex],buffer,buffSize );
	return cantBytes;
}

void ServerSocket :: cerrarConexion () {

    setBlocking();
    std::string peticion = EXIT_MESSAGE;
    char buffer[BUFFSIZE];
	for(int i = 0; i < clientSockets.size(); ++i){
        // hago blocking el socket que voy a cerrar
        long arg = fcntl(this->clientSockets[i], F_GETFL, NULL);
        arg &= ~O_NONBLOCK;
        fcntl(this->clientSockets[i], F_SETFL, arg);

        enviar( static_cast<const void*>(peticion.c_str()),peticion.size(), i );
        recibir ( static_cast<void*>(buffer), BUFFSIZE, i );
        peticion = "OK";
        enviar( static_cast<const void*>(peticion.c_str()),peticion.size(), i );

        this->cerrarConexion(i);
	}

	close ( this->fdSocket );
}

void ServerSocket :: cerrarConexion (unsigned int clientIndex) {
    close ( this->clientSockets[clientIndex]);
    this->clientSockets.erase(clientSockets.begin() + clientIndex);
    this->usernames.erase(usernames.begin() + clientIndex);
}

void ServerSocket :: handleInput(std::string peticion, int clientIndex) {
    std::string username = getUsername(peticion);
    std::string message = getMessage(peticion);

    if (isExitCode(peticion)) {
        cerrarConexion(clientIndex);
        peticion = "OK";
        enviar ( static_cast<const void*>(peticion.c_str()),peticion.size(), clientIndex );
    } else if (username == CLIENT_ID) {
        if (isValidUsername(message)) {
            registerUsername(message, clientIndex);
            peticion = "OK";
        } else {
            peticion = "USERNAME_TAKEN";
        }
        enviar ( static_cast<const void*>(peticion.c_str()),peticion.size(), clientIndex );
    } else {
        enviar ( static_cast<const void*>(peticion.c_str()),peticion.size() );
    }
}
