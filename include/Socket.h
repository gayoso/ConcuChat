#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>

class Socket {

	protected:
		int fdSocket;
		struct sockaddr_in serv_addr;

	public:
		Socket ( const unsigned int port );
		virtual ~Socket ();

		virtual void abrirConexion () = 0;

		virtual int enviar ( const void* buffer,const unsigned int buffSize ) = 0;
		//virtual int recibir ( void* buffer,const unsigned int buffSize ) = 0;

		virtual void cerrarConexion () = 0;

		void setNonBlocking();
		void setBlocking();
};

#endif /* SOCKET_H_ */
