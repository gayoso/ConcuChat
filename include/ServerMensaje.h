#ifndef SERVERMENSAJE_H_INCLUDED
#define SERVERMENSAJE_H_INCLUDED

#include "defines.h"

#define ANY_MESSAGE 0
#define	TEXT 1
#define CONNECTION_START 2
#define CONNECTION_END 3
//#define	RESPUESTA	2
//#define TEXTO_SIZE	255

typedef struct mensaje {
	long mtype;
	int socket;
	int msize;
	char texto[BUFFSIZE];
} mensaje;

#endif // SERVERMENSAJE_H_INCLUDED
