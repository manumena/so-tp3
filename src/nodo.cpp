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
    MPI_Request req;

    while (true) {
    	MPI_Probe(ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    	if (status.MPI_TAG == QUIT_TAG) {
	    	MPI_Recv(NULL, 0, MPI_CHAR, ROOT, QUIT_TAG, MPI_COMM_WORLD, &status);
	    	break;
    	}

    	if (status.MPI_TAG == LOAD_REQ_TAG) {
    		int filenameSize;
    		MPI_Get_count(&status, MPI_CHAR, &filenameSize);

    		// Recibir nombre de archivo
    		char *filename = (char *) malloc(filenameSize);
    		MPI_Recv(filename, filenameSize, MPI_CHAR, ROOT, LOAD_REQ_TAG, MPI_COMM_WORLD, &status);
			  string fname(filename);

    		// Enviar mensaje de aceptacion de carga
        MPI_Request req;
        trabajarArduamente();
    		MPI_Isend("", 0, MPI_CHAR, ROOT, LOAD_ACCEPT_TAG, MPI_COMM_WORLD, &req);

    		// Esperar a recibir la orden
    		int order;
    		MPI_Recv(&order, 1, MPI_INT, ROOT, LOAD_ORDER_TAG, MPI_COMM_WORLD, &status);

    		if (order == ACCEPTED) {
    			// Cargar el archivo en el hashmap local
    			hashmap.load(fname);
    			// printf("[%d] Loaded %s file \n", rank, filename);
          trabajarArduamente();
    			MPI_Isend("", 0, MPI_CHAR, ROOT, LOAD_COMPLETE_TAG, MPI_COMM_WORLD, &req);
    		}

    		free(filename);
    	}

      if (status.MPI_TAG == MEMBER_REQ_TAG) {
    		int keySize;
    		MPI_Get_count(&status, MPI_CHAR, &keySize);

    		char *key = (char *) malloc(keySize);

    		MPI_Recv(key, keySize, MPI_CHAR, ROOT, MEMBER_REQ_TAG, MPI_COMM_WORLD, &status);

    		string strKey(key);
    		int esta = hashmap.member(strKey);
    		trabajarArduamente();
    		MPI_Isend(&esta, 1, MPI_INT, ROOT, MEMBER_RES_TAG, MPI_COMM_WORLD, &req);

    		free(key);
    	}

      if(status.MPI_TAG == MAXIMUM_MSG_START_TAG){

        MPI_Recv(NULL, 0, MPI_CHAR, ROOT, MAXIMUM_MSG_START_TAG, MPI_COMM_WORLD, &status);
        // printf("[%d] Recibo el tag MAXIMUM_MSG_START_TAG \n", rank );
        trabajarArduamente();
        HashMap::iterator it = hashmap.begin();
        while(it != hashmap.end()){
          string word = *it;

          // Transformar el string en char* para enviar
          char *wordPointer = (char *) malloc(word.size() + 1);
          strcpy(wordPointer, word.c_str());

          MPI_Isend(wordPointer, word.size() + 1, MPI_CHAR, ROOT, MAXIMUM_WORD_TAG, MPI_COMM_WORLD, &req);

          // printf("[%d] Envio la key %s con size %ld al nodo 0 \n", rank, wordPointer,word.size() + 1);
          //free(wordPointer);
          it++;
        }
        if(hashmap.size() > 0)
          trabajarArduamente();
        // printf("[%d] Ya le paso sus keys a el nodo ROOT \n", rank);
        MPI_Isend(NULL, 0, MPI_CHAR, ROOT, MAXIMUM_WORD_TAG, MPI_COMM_WORLD, &req);
      }

      if (status.MPI_TAG == ADD_AND_INC_REQ_TAG) {
        int keySize;
        MPI_Get_count(&status, MPI_CHAR, &keySize);

        char *key = (char *) malloc(keySize);
        MPI_Recv(key, keySize, MPI_CHAR, ROOT, ADD_AND_INC_REQ_TAG, MPI_COMM_WORLD, &status);
        string k(key);
        // Enviar mensaje de aceptacion de carga
        trabajarArduamente();
        MPI_Isend("", 0, MPI_CHAR, ROOT, ADD_AND_INC_ACCEPT_TAG, MPI_COMM_WORLD, &req);
        // Esperar a recibir la orden
        int order;
        MPI_Recv(&order, 1, MPI_INT, ROOT, ADD_AND_INC_ORDER_TAG, MPI_COMM_WORLD, &status);
        if (order == ACCEPTED) {
          // Cargar el archivo en el hashmap local
          hashmap.addAndInc(k);
        }
        free(key);
        trabajarArduamente();
        MPI_Isend("", 0, MPI_CHAR, ROOT, ADD_AND_INC_COMPLETE_TAG, MPI_COMM_WORLD, &req);
      }
    }
    // printf("[%d] Quit \n", rank);
}

void trabajarArduamente() {
    int r = rand() % 2500000 + 500000;
    usleep(r);
}
