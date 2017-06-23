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
	    	printf("[%d] Me muero, el tag es %d\n", rank, status.MPI_TAG);
	    	break;
    	}

    	if (status.MPI_TAG == LOAD_REQ_TAG) {
    		int filenameSize;
    		MPI_Get_count(&status, MPI_CHAR, &filenameSize);
    		printf("[%d] Tamaño del nombre: %d\n", rank, filenameSize);

    		MPI_Request req;
    		char filename[filenameSize];
    		MPI_Recv(filename, filenameSize, MPI_CHAR, ROOT, LOAD_REQ_TAG, MPI_COMM_WORLD, &status);
    		printf("[%d] Recibi el archivo: %s\n", rank, filename);

    		// Enviar mensaje de aceptacion de carga
    		MPI_Isend("", 0, MPI_CHAR, ROOT, LOAD_ACCEPT_TAG, MPI_COMM_WORLD, &req);

    		// Esperar a recibir la orden
    		int order;
    		MPI_Recv(&order, 1, MPI_INT, ROOT, LOAD_ORDER_TAG, MPI_COMM_WORLD, &status);

    		if (order == ACCEPTED) {
    			// Cargar el archivo en el hashmap local
    			printf("[%d] Cargo %s\n", rank, filename);
    		} else
    			printf("[%d] No lo cargo\n", rank);
    	}
    }
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
