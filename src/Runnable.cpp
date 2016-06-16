#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "Runnable.h"
#include "LockFile.h"

Runnable::Runnable()
{

}

Runnable::~Runnable()
{
    //dtor
}

pid_t Runnable::run(){
    // esto se asegura que al crear un runnable se inicializan sus estructuras
    /// podemos asumir que no existe un archivo con el nombre 'runnableTempLockFile' (sino cambiar, pq al final lo borro)
    LockFile lock("runnableTempLockFile");
    lock.tomarLock();
    this->init();
    lock.liberarLock();
    unlink("runnableTempLockFile");

    pid_t pid = fork();
    if(pid == -1){
        child = false;
        perror("Runnable error: ");
    }

    // en el hijo corro la funcion _run y termino
    if(pid == 0){
        _run();
        exit(0);
    } else {
        return pid;
    }
    // en el padre sigo como si nada
}
