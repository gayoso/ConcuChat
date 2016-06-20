#ifndef SERVERMENSAJE_H_INCLUDED
#define SERVERMENSAJE_H_INCLUDED

#include "defines.h"

#define ANY_MESSAGE 0
#define	TEXT 1
#define CONNECTION_START 2
#define CONNECTION_END 3
#define NICKNAME_REQ 4
#define GET_LOG 5

typedef struct mensaje {
	long mtype;
	int socket;
	int nsize;
	char nickname[NICKSIZE];
	int msize;
	char texto[BUFFSIZE];
} mensaje;

#endif // SERVERMENSAJE_H_INCLUDED
