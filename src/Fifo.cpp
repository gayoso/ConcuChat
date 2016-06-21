#include "Fifo.h"
#include "Logger.h"
#include <stdio.h>

Fifo::Fifo(const std::string nombre) : nombre(nombre), fd(-1) {
}

void Fifo::crear(){
    if(mknod ( static_cast<const char*>(nombre.c_str()),S_IFIFO|0666,0 ) == -1){
        //perror("Fifo crear error: ");
        Logger::logErrno("Fifo crear error: ");
	}
}

Fifo::~Fifo() {
}

void Fifo::cerrar() {
    if(fd != -1){
        if(close ( fd ) == -1){
            perror("Fifo cerrar error: ");
            Logger::logErrno("Fifo cerrar error: ");
        }
        fd = -1;
    }
}

void Fifo::eliminar() const {
	if(unlink ( nombre.c_str() ) == -1){
        //perror("Fifo eliminar error: ");
        Logger::logErrno("Fifo eliminar error: ");
	}
}
