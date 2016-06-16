#include <iostream>
#include <string.h>
#include <termios.h>
#include "ClientSocket.h"
#include "SignalHandler.h"
#include "SIG_Trap.h"
#include "defines.h"

// esto hace que no se impriman por pantalla los caracteres que apretas (como las contrasenias)
// es pq si no esta en modo canonico y apretas flechas o teclas raras imprime basura
void setStdInEcho(bool enable = true) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable) {
        tty.c_lflag &= ~ECHO;
    } else {
        tty.c_lflag |= ECHO;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// esto cambia un flag de stdin que lo saca de 'modo canonico'
// es para que las llamadas a cin retornen despues de cada caracter, en vez de esperar a un newline
void setStdInCanon(bool enable = true) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable) {
        tty.c_lflag &= ~ICANON;
    } else {
        tty.c_lflag |= ICANON;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

int main ( int argc,char *argv[] ) {

	SIG_Trap sigalarm_handler(SIGALRM);
	SIG_Trap sigint_handler(SIGINT);
    SignalHandler::getInstance()->registrarHandler(SIGALRM, &sigalarm_handler);
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

	if ( argc != 2 ) {
		std::cout << "Uso: ./EchoClient direccion_ip_servidor" << std::endl << std::endl;
		return -1 ;
	}

	try {
		ClientSocket socket ( argv[1],SERVER_PORT );
		char bufferRta[BUFFSIZE];
		char entrada[BUFFSIZE];

		std::cout << "ConcuClient: abriendo conexion con el servidor " << argv[1] << std::endl;
		socket.abrirConexion();

        std::string mensaje, rta, username;
		std::cout << "ConcuClient: conexion abierta. Por favor ingrese su nombre de usuario:" << std::endl;
		while (!sigint_handler.signalWasReceived()) {
            // obtengo username de usuario
            std::cin.getline ( entrada,BUFFSIZE );
            username = entrada;

            // chequeo si es alfanumerico
            bool isalpha = true;
            for (int i = 0; i < username.length(); ++i) {
                if (!(isalpha = std::isalpha(username[i]))) {
                    std::cout << "El nombre de usuario solo puede contener caracteres alfanumericos (a-z, 0-9): " << std::endl;
                }
            }
            if (!isalpha) continue;

            // armo mensaje de pedido de username para server
            mensaje = "_client: " + username;
            strcpy(entrada, mensaje.c_str());
            socket.enviar ( static_cast<const void*>(entrada),mensaje.size() );

            // recibo respuesta
            int longRta = socket.recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
            rta = bufferRta;
            rta.resize(longRta);

            // veo resultado
            if (rta == EXIT_MESSAGE) {
                raise(SIGINT);
            } else if (rta == std::string("OK")) {
                std::cout << "ConcuClient: bienvenido a la sala de chat, " << username << "!" << std::endl;
                std::cout << "ConcuClient: ingresar el texto a enviar y presionar [ENTER].  s para salir " << std::endl << std::endl;
                break;
            } else {
                std::cout << "ConcuClient: el nombre de usuario ya fue tomado" << std::endl;
            }
		}

        socket.setNonBlocking();
        mensaje = "";
        setStdInCanon(false);
        setStdInEcho(false);
		while (!sigint_handler.signalWasReceived()) {

            std::cout << username << ": " << mensaje << std::flush;

            // hay que alternar entre chequear input del usuario y chequear si el server mando algo
			alarm(2);
			char c = getchar();
			alarm(0);

            // para que quede lindo lo manejo caracter a caracter
            // cuando el usuario apreto enter el mensaje esta listo para enviarlo
            if (!sigalarm_handler.signalWasReceived() && c == '\n') {
                std::cout << c << std::flush;
                mensaje = username + ": " + mensaje;
                strcpy(entrada, mensaje.c_str());
                socket.enviar ( static_cast<const void*>(entrada),mensaje.size() );
                mensaje = "";
            } else {
                // si el usuario no apreto enter pero no sono la alarma, me fijo que tecla apreto
                if (!sigalarm_handler.signalWasReceived()) {
                    // si apreto una tecla valida, la imprimo y la acumulo al mensaje
                    if (c >= 32 && c <= 126) {
                        std::cout << c << std::flush;
                        mensaje += c;
                    // si apreto backspace borro el ultimo caracter
                    } else if (c == 127) {
                        if (mensaje.size() > 0)
                            mensaje.erase(mensaje.end()-1);
                    }
                // si sono la alarma, la reseteo y reseto el bit de error de cin (pq lo interrumpi)
                } else {
                    sigalarm_handler.reset();
                    std::cin.clear();
                }

                // a cada paso borro la linea de la pantalla por si tengo que imprimir algo encima
                // es para simular la textbox de skype ponele, donde escribis lo que queres mandar
                // y si alguien te escribe antes, no se te borra ni se te mezcla con lo que escribias vos
                std::cout << "\r" << std::flush;
                std::string temp = username + ": " + mensaje;
                for (int i = 0; i < temp.length(); ++i) {
                    std::cout << " " << std::flush;
                }
                if (c == 127) std::cout << " " << std::flush;
                std::cout << "\r" << std::flush;
            }

            while(!sigint_handler.signalWasReceived()){
                // veo si el server me mando algo
                int longRta = socket.recibir ( static_cast<void*>(bufferRta),BUFFSIZE );
                if (longRta == -1){
                    break;
                }

                rta = bufferRta;
                rta.resize(longRta);

                // si era un mensaje de que se cierra el server, me mando un sigint
                if (rta == EXIT_MESSAGE) {
                    std::cout << "sigint" << std::endl;
                    raise(SIGINT);
                } else {
                    std::cout << rta << std::endl;
                }
			}
		}
		// seria como el handshake de tcp para cerrar la conexion
        mensaje = username + ": " + EXIT_MESSAGE;
        strcpy(entrada, mensaje.c_str());
        socket.setBlocking();
        socket.enviar ( static_cast<const void*>(entrada),mensaje.size() );
        int longRta = socket.recibir ( static_cast<void*>(bufferRta),BUFFSIZE );

		std::cout << "EchoClient: cerrando la conexion" << std::endl;
		socket.cerrarConexion ();
		std::cout << "EchoClient: fin del programa" << std::endl;

		setStdInEcho(true);
        setStdInCanon(true);

	} catch ( std::string& mensaje ) {
		std::cout << mensaje << std::endl;
	}

	return 0;
}
