#ifndef MEMORIACOMPARTIDA_H_
#define MEMORIACOMPARTIDA_H_

#define SHM_OK			 0
#define	ERROR_FTOK		-1
#define ERROR_SHMGET	-2
#define	ERROR_SHMAT		-3

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include "Logger.h"


template <class T> class MemoriaCompartida {

private:
	int shmId;
	T*	ptrDatos;

	std::string archivo;
	char letra;

	int cantidadProcesosAdosados () const;

public:
	MemoriaCompartida (const std::string& archivo,const char letra);
	~MemoriaCompartida ();
	int crear ();
	void liberar ();
	void escribir ( const T& dato );
	T leer () const;

	static std::string getName() { return "MEM_COMP"; };
};

template <class T> MemoriaCompartida<T> :: MemoriaCompartida(const std::string& archivo,const char letra)
    : shmId(0), ptrDatos(NULL), archivo(archivo), letra(letra) {
}

template <class T> MemoriaCompartida<T> :: ~MemoriaCompartida() {
}

template <class T> int MemoriaCompartida<T> :: crear () {

	// generacion de la clave
	key_t clave = ftok ( archivo.c_str(),letra );
	if ( clave == -1 ){
        Logger::logErrno(getName() + "_crear_ftok");
		return ERROR_FTOK;
	} else {
		// creacion de la memoria compartida
		this->shmId = shmget ( clave,sizeof(T),0644|IPC_CREAT );

		if ( this->shmId == -1 ){
            Logger::logErrno(getName() + "_crear_shmget");
			return ERROR_SHMGET;
		} else {
			// attach del bloque de memoria al espacio de direcciones del proceso
			void* ptrTemporal = shmat ( this->shmId,NULL,0 );

			if ( ptrTemporal == (void *) -1 ) {
                Logger::logErrno(getName() + "_crear_shmat");
				return ERROR_SHMAT;
			} else {
				this->ptrDatos = static_cast<T*> (ptrTemporal);
				return SHM_OK;
			}
		}
	}
}


template <class T> void MemoriaCompartida<T> :: liberar () {
	// detach del bloque de memoria
	shmdt ( static_cast<void*> (this->ptrDatos) );

	int procAdosados = this->cantidadProcesosAdosados ();

	if ( procAdosados == 0 ) {
		shmctl ( this->shmId,IPC_RMID,NULL );
	}
}

template <class T> void MemoriaCompartida<T> :: escribir ( const T& dato ) {
	* (this->ptrDatos) = dato;
}

template <class T> T MemoriaCompartida<T> :: leer () const {
	return ( *(this->ptrDatos) );
}

template <class T> int MemoriaCompartida<T> :: cantidadProcesosAdosados () const {
	shmid_ds estado;
	shmctl ( this->shmId,IPC_STAT,&estado );
	return estado.shm_nattch;
}


#endif /* MEMORIACOMPARTIDA_H_ */
