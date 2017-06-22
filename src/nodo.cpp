#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include "constants.hpp"

using namespace std;

void nodo(unsigned int rank) {
    // printf("Soy un nodo. Mi rank es %d \n", rank);

    // TODO: Implementar
    // Crear un HashMap local
    MPI_Status status;

    while (true) {
    	MPI_Probe(ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    	if (status.MPI_TAG == QUIT_TAG) {
	    	MPI_Recv(NULL, 0, MPI_CHAR, ROOT, QUIT_TAG, MPI_COMM_WORLD, &status);
	    	printf("Me muero, el tag es %d. Mi rank es %d \n", status.MPI_TAG, rank);
	    	break;
    	}

    	if (status.MPI_TAG == LOAD_REQ_TAG) {
    		int filenameSize;
    		MPI_Get_count(&status, MPI_CHAR, &filenameSize);
    		printf("Mi rank es %d, Tamaño del nombre: %d\n", rank, filenameSize);

    		MPI_Request req;
    		char filename[filenameSize];
    		MPI_Recv(filename, filenameSize, MPI_CHAR, ROOT, LOAD_REQ_TAG, MPI_COMM_WORLD, &status);
    		printf("Mi rank es %d, recibi el archivo: %s\n", rank, filename);
    		MPI_Isend("", 0, MPI_CHAR, rank, LOAD_ACCEPT_TAG, MPI_COMM_WORLD, &req);
    	}
    }
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
