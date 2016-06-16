#include "Semaforo.h"
#include "LockFile.h"
#include "Logger.h"

Semaforo :: Semaforo ( const std::string& nombre, const int projId, const int valorInicial )
    :valorInicial(valorInicial), nombre(nombre), projId(projId) {
	/*key_t clave = ftok ( nombre.c_str(),'a' );
	this->id = semget ( clave,1,0666 | IPC_CREAT );

	this->inicializar ();*/
}

Semaforo::~Semaforo() {
}

std::string Semaforo :: getName() { return "SEM"; }

int Semaforo :: get() {
    key_t clave = ftok ( nombre.c_str(),projId );
	if(clave == -1) {
        Logger::logErrno(getName() + "_get");
        return -1;
    }

    this->id = semget ( clave,1,0666 );
	if(this->id == -1) {
        Logger::logErrno(getName() + "_get");
        return -1;
    }

	return this->id;
}

int Semaforo :: crear() {
    LockFile lock(this->nombre + std::to_string(projId));
    lock.tomarLock();

    key_t clave = ftok ( nombre.c_str(),projId );
    if(clave == -1) {
        Logger::logErrno(getName() + "_crear");
        return -1;
    }

	this->id = semget ( clave,1,0666 | IPC_CREAT ); // ver return
	if(this->id == -1) {
        Logger::logErrno(getName() + "_crear");
        return -1;
    }

	int res = this->inicializar ();

	lock.liberarLock();
	return res;
}

int Semaforo :: inicializar () const {

	union semnum {
		int val;
		struct semid_ds* buf;
		ushort* array;
	};

	semnum init;
	init.val = this->valorInicial;
	int resultado = semctl ( this->id,0,SETVAL,init );
	if(resultado == -1) {
        Logger::logErrno(getName() + "_init");
    }
	return resultado;
}

int Semaforo :: p () const {

	struct sembuf operacion;

	operacion.sem_num = 0;	// numero de semaforo
	operacion.sem_op  = -1;	// restar 1 al semaforo
	//operacion.sem_flg = SEM_UNDO;
	operacion.sem_flg = 0;

	int resultado = semop ( this->id,&operacion,1 );
	if(resultado == -1) {
        Logger::logErrno(getName() + "_p");
    }
	return resultado;
}

int Semaforo :: v () const {

	struct sembuf operacion;

	operacion.sem_num = 0;	// numero de semaforo
	operacion.sem_op  = 1;	// sumar 1 al semaforo
	//operacion.sem_flg = SEM_UNDO;
	operacion.sem_flg = 0;

	int resultado = semop ( this->id,&operacion,1 );
	if(resultado == -1) {
        Logger::logErrno(getName() + "_v");
    }
	return resultado;
}

void Semaforo :: eliminar () const {
	int resultado = semctl ( this->id,0,IPC_RMID );
	if(resultado == -1) {
        Logger::logErrno(getName() + "_eliminar");
    }
    LockFile lock(this->nombre + std::to_string(projId));
    lock.clean();
}
