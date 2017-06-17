#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;

#define ROOT 0

#define QUIT_TAG 0

void nodo(unsigned int rank) {
    printf("Soy un nodo. Mi rank es %d \n", rank);

    // TODO: Implementar
    // Creo un HashMap local
    MPI_Status status;

    while (true) {
    	// MPI_Probe(ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    	MPI_Recv(NULL, 0, MPI_CHAR, ROOT, QUIT_TAG, MPI_COMM_WORLD, &status);
    	printf("Me muero. Mi rank es %d \n", rank);
    	break;
    }
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
