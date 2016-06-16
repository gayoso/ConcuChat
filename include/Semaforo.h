#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string>

class Semaforo {

private:
	int id;
	int valorInicial;
    std::string nombre;
    int projId;

	int inicializar () const;

public:
	Semaforo ( const std::string& nombre, const int projId, const int valorInicial );
	~Semaforo();

	int p () const; // decrementa
	int v () const; // incrementa

	int crear();
	int get();
	void eliminar () const;

	static std::string getName();
};

#endif /* SEMAFORO_H_ */
