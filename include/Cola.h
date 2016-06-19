#ifndef COLA_H_
#define COLA_H_

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <string>

template <class T> class Cola {
	private:
		key_t	clave;
		int		id;

		std::string archivo;
		char letra;

	public:
		Cola ( const std::string& archivo,const char letra );
		~Cola();
		int escribir ( const T& dato ) const;
		int leer ( const int tipo,T* buffer ) const;
		int destruir () const;
		void crear();
		void get();
};

template <class T> Cola<T> :: Cola ( const std::string& archivo,const char letra ) :
    archivo(archivo), letra(letra)
{

}

template <class T> void Cola<T> :: crear() {
    this->clave = ftok ( archivo.c_str(),letra );
	if ( this->clave == -1 )
		perror ( "Error en ftok" );

	this->id = msgget ( this->clave,0777|IPC_CREAT );
	if ( this->id == -1 )
		perror ( "Error en msgget" );
}

template <class T> void Cola<T> :: get() {
    this->clave = ftok ( archivo.c_str(),letra );
	if ( this->clave == -1 )
		perror ( "Error en ftok" );

	this->id = msgget ( this->clave,0777 );
	if ( this->id == -1 )
		perror ( "Error en msgget" );
}

template <class T> Cola<T> :: ~Cola () {
}

template <class T> int Cola<T> :: destruir () const {
	int resultado = msgctl ( this->id,IPC_RMID,NULL );
	return resultado;
}

template <class T> int Cola<T> :: escribir ( const T& dato ) const {
	int resultado = msgsnd ( this->id,static_cast<const void*>(&dato),sizeof(T)-sizeof(long),0 );
	return resultado;
}

template <class T> int Cola<T> :: leer ( const int tipo,T* buffer ) const {
	int resultado = msgrcv ( this->id,static_cast<void *>(buffer),sizeof(T)-sizeof(long),tipo,0 );
	return resultado;
}

#endif /* COLA_H_ */
