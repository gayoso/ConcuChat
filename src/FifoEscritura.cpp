#include "FifoEscritura.h"
#include "Logger.h"
#include <stdio.h>

FifoEscritura::FifoEscritura(const std::string nombre) : Fifo(nombre) {
}

FifoEscritura::~FifoEscritura() {
}

void FifoEscritura::abrir() {
	fd = open ( nombre.c_str(),O_WRONLY );
	if(fd == -1){
        //perror("FifoEscritura abrir error: ");
        Logger::logErrno("FifoEscritura abrir error: ");
        // que onda? se cierra el programa?
    }
}

ssize_t FifoEscritura::escribir(const void* buffer,const ssize_t buffsize) const {
	ssize_t bytes_written = write ( fd,buffer,buffsize );
	if(bytes_written == -1){
        //perror("FifoEscritura escribir error: ");
        Logger::logErrno("FifoEscritura escribir error: ");
	}
}
