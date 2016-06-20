#include "ClientSocket.h"
#include "ClientListener.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "defines.h"
#include "Logger.h"
#include <sys/wait.h>
#include <iostream>

ClientSocket :: ClientSocket ( const std::string& ipServidor,const unsigned int port, std::string nickname )
    : Socket ( port ), nickname(nickname) {
	this->ipServidor = ipServidor;
}

ClientSocket :: ~ClientSocket () {
}

void ClientSocket :: registrarNickname() {
    std::string mensaje;
    std::string nicknamePropuesto = nickname;
    nickname = "_client";

    Logger::log("CLIENT", "Chequeando caracteres de nickname" , DEBUG);
    for(int i = 0; i < nicknamePropuesto.size(); ++i) {
        if(!isalnum(nicknamePropuesto[i])){
            std::cout << "Client: Error al ingresar: el nickname debe ser alfanumerico" << std::endl;
            Logger::log("CLIENT", "Error: el nickname tenia caracteres no alfanumericos" , DEBUG);
            raise(SIGINT);
        }
    }

    Logger::log("CLIENT", "Intentando registrar nickname: " + nickname , DEBUG);
    char bufferRta[BUFFSIZE];
    std::string rta;

    mensaje = nickname + ": " + nicknamePropuesto;
    enviar ( static_cast<const void*>(mensaje.c_str()),mensaje.size() );
    int longRta = recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
    rta = bufferRta;
    rta.resize(longRta);
    if (rta == "OK") {
        Logger::log("CLIENT", "Se ingreso con nickname: " +  nickname, DEBUG);
        nickname = nicknamePropuesto;
    } else {
        std::cout << "Client: Error al ingresar: " << rta << std::endl;
        Logger::log("CLIENT", "Error al registrar nickname: " + rta , DEBUG);
        raise(SIGINT);
    }
}

void ClientSocket :: loadChatHistory() {
    std::string mensaje;

    Logger::log("CLIENT", "Se inicia el listener" , DEBUG);
    startListener();

    Logger::log("CLIENT", "Se carga el historial del chat" , DEBUG);
    mensaje = std::string(SEPARATOR) + ": " + nickname;
    enviar ( static_cast<const void*>(mensaje.c_str()),mensaje.size() );
}

void ClientSocket :: handleInput(std::string entrada) {

    //std::cout << "EchoClient: conexion abierta.  Ingresar el texto a enviar y presionar [ENTER]" << std::endl << std::endl;

    std::string mensaje;

    if(!std::cin.bad()) {
        mensaje = nickname + ": " + entrada;
        enviar ( static_cast<const void*>(mensaje.c_str()),mensaje.size() );
    }
}

void ClientSocket :: abrirConexion () {

	struct hostent *server = gethostbyname ( this->ipServidor.c_str() );
	if ( server == NULL ) {
		std::string mensaje = std::string("No se puede localizar el host: ") + std::string(strerror(errno));
		Logger::log("CLIENT", mensaje, DEBUG);
		raise(SIGINT);
		return;
	}

    bcopy ( (char *)server->h_addr,
    		(char *)&(this->serv_addr.sin_addr.s_addr),
    		server->h_length );

    int connectOk = connect ( this->fdSocket,
    						(const struct sockaddr *)&(this->serv_addr),
    						sizeof(this->serv_addr) );
    if ( connectOk < 0 ) {
    	std::string mensaje = std::string("Error en connect(): ") + std::string(strerror(errno));
    	Logger::log("CLIENT", mensaje, DEBUG);
    	raise(SIGINT);
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
    std::string mensaje;
    Logger::log("CLIENT", "Cerrando cliente" , DEBUG);
    mensaje = std::string(EXIT_MESSAGE) + ": " + nickname;
    enviar(static_cast<const void*>(mensaje.c_str()),mensaje.size());

	close ( this->fdSocket );
	int status;
	kill(clientListener, SIGINT);
	waitpid(clientListener, &status, 0);
}
