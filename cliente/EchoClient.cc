#include <iostream>
#include <string.h>
#include "ClientSocket.h"


int main ( int argc,char *argv[] ) {

	static const unsigned int BUFFSIZE = 255;
	static const unsigned int SERVER_PORT = 16000;

	if ( argc != 2 ) {
		std::cout << "Uso: ./EchoClient direccion_ip_servidor" << std::endl << std::endl;
		return -1 ;
	}

	try {
		ClientSocket socket ( argv[1],SERVER_PORT );
		char bufferRta[BUFFSIZE];
		char entrada[BUFFSIZE];

		std::cout << "EchoClient: abriendo conexion con el servidor " << argv[1] << std::endl;
		std::cout << "EchoClient: conexion abierta.  Ingresar el texto a enviar y presionar [ENTER].  s para salir " << std::endl << std::endl;
		socket.abrirConexion();

		std::string mensaje;
		do {
			std::cin.getline ( entrada,BUFFSIZE );

			std::cout << "EchoClient: enviando dato al servidor: " << entrada << std::endl;
			mensaje = entrada;
			socket.enviar ( static_cast<const void*>(entrada),mensaje.size() );
            std::cout << "ya envie, espero a recibir" << std::endl;
			int longRta = socket.recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
			std::string rta = bufferRta;
			rta.resize(longRta);

			std::cout << "EchoClient: respuesta recibida del servidor: " << rta << std::endl;
		} while ( mensaje != std::string("s") );

		std::cout << "EchoClient: cerrando la conexion" << std::endl;
		socket.cerrarConexion ();
		std::cout << "EchoClient: fin del programa" << std::endl;

	} catch ( std::string& mensaje ) {
		std::cout << mensaje << std::endl;
	}

	return 0;
}
