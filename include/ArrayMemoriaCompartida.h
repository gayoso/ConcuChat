#ifndef ARRAYMEMORIACOMPARTIDA_H_INCLUDED
#define ARRAYMEMORIACOMPARTIDA_H_INCLUDED

#include "MemoriaCompartida.h"

template <class T> class ArrayMemoriaCompartida {

    protected:
        int maxSize;

        int shmId;
        T*	ptrDatos;

        std::string archivo;
        char letra;

        int cantidadProcesosAdosados () const;

    public:
        ArrayMemoriaCompartida (const std::string& archivo,const char letra, int maxSize);
        ~ArrayMemoriaCompartida ();
        int crear ();
        void liberar ();
        void escribir ( const T& dato, int index );
        T leer (int index) const;

        static std::string getName() { return "MEM_COMP"; };
};

template <class T> ArrayMemoriaCompartida<T> :: ArrayMemoriaCompartida(const std::string& archivo,const char letra, int maxSize) :
    maxSize(maxSize) {
}

template <class T> ArrayMemoriaCompartida<T> :: ~ArrayMemoriaCompartida() {
}

template <class T> int ArrayMemoriaCompartida<T> :: crear () {

	// generacion de la clave
	key_t clave = ftok ( archivo.c_str(),letra );
	if ( clave == -1 ){
        Logger::logErrno(getName() + "_crear_ftok");
		return ERROR_FTOK;
	} else {
		// creacion de la memoria compartida
		this->shmId = shmget ( clave,sizeof(T)*maxSize,0644|IPC_CREAT );

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


template <class T> void ArrayMemoriaCompartida<T> :: liberar () {
	// detach del bloque de memoria
	shmdt ( static_cast<void*> (this->ptrDatos) );

	int procAdosados = this->cantidadProcesosAdosados ();

	if ( procAdosados == 0 ) {
		shmctl ( this->shmId,IPC_RMID,NULL );
	}
}

template <class T> void ArrayMemoriaCompartida<T> :: escribir ( const T& dato, int index ) {
	if (index > maxSize-1) {
        return; // index error
	}
	* (this->ptrDatos+index) = dato;
}

template <class T> T ArrayMemoriaCompartida<T> :: leer (int index) const {
	if (index > maxSize-1) {
        return; // index error
	}
	return ( *(this->ptrDatos+index) );
}

template <class T> int ArrayMemoriaCompartida<T> :: cantidadProcesosAdosados () const {
	shmid_ds estado;
	shmctl ( this->shmId,IPC_STAT,&estado );
	return estado.shm_nattch;
}

#endif // ARRAYMEMORIACOMPARTIDA_H_INCLUDED
