#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <utility>
#include <list>

#include <iostream>
#include <sstream>

#include <string>
#include <cstring>

#include "nodo.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include "constants.hpp"

using namespace std;

void nodo(unsigned int rank) {
    // Crear un HashMap local
    HashMap hashmap;
    MPI_Status status;

    while (true) {
    	MPI_Probe(ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    	if (status.MPI_TAG == QUIT_TAG) {
	    	MPI_Recv(NULL, 0, MPI_CHAR, ROOT, QUIT_TAG, MPI_COMM_WORLD, &status);
	    	break;
    	}

    	if (status.MPI_TAG == LOAD_REQ_TAG) {
    		int filenameSize;
    		MPI_Get_count(&status, MPI_CHAR, &filenameSize);
    		// ESTE ES EL PRINT QUE SALVA EL BUG excepto para nombres de archivo de tamaño menor a 5
    		printf("[%d] Tamaño del nombre: %d\n", rank, filenameSize);

    		char *filename = (char *) malloc(filenameSize);
    		MPI_Recv(filename, filenameSize, MPI_CHAR, ROOT, LOAD_REQ_TAG, MPI_COMM_WORLD, &status);
			string fname(filename);
    		printf("[%d] Recibi el archivo: %s\n", rank, fname.c_str());

    		// Enviar mensaje de aceptacion de carga
            MPI_Request req;
    		MPI_Isend("", 0, MPI_CHAR, ROOT, LOAD_ACCEPT_TAG, MPI_COMM_WORLD, &req);

    		// Esperar a recibir la orden
    		int order;
    		MPI_Recv(&order, 1, MPI_INT, ROOT, LOAD_ORDER_TAG, MPI_COMM_WORLD, &status);

    		if (order == ACCEPTED) {
    			// Cargar el archivo en el hashmap local
    			hashmap.load(fname);
    			// hashmap.printAll();
    		}

    		free(filename);
    	}

    	if (status.MPI_TAG == MEMBER_TAG) {
    		int keySize;
    		MPI_Get_count(&status, MPI_CHAR, &keySize);

    		char *key = (char *) malloc(keySize);

    		MPI_Recv(key, keySize, MPI_CHAR, ROOT, MEMBER_TAG, MPI_COMM_WORLD, &status);
    		printf("[%d] Recibo la clave %s\n", rank, key);

    		

    		free(key);
    	}

        if(status.MPI_TAG == MAXIMUM_MSG_START_TAG){
            int msgsize;
            MPI_Get_count(&status, MPI_CHAR, &msgsize);

            char *msg = (char *) malloc(msgsize);

            MPI_Recv(msg, msgsize, MPI_CHAR, ROOT, MAXIMUM_MSG_START_TAG, MPI_COMM_WORLD, &status);

            printf("[%d] Recibo el mensaje %s\n", rank, msg);


            HashMap::iterator it = hashmap.begin();
            MPI_Request req;
            
            while(it != hashmap.end()){
                string word = *it;

                // Transformar el string en char* para enviar
                char *wordPointer = (char *) malloc(word.size() + 1);
                strcpy(wordPointer, word.c_str());

                MPI_Isend(wordPointer, word.size() + 1, MPI_CHAR, rank, MAXIMUM_WORD_TAG, MPI_COMM_WORLD, &req);

                free(wordPointer);
                it++;
            }
            MPI_Isend(NULL, 0, MPI_CHAR, rank, MAXIMUM_WORD_TAG, MPI_COMM_WORLD, &req);

            trabajarArduamente();
        }
    }

}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
