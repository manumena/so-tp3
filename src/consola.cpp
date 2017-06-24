#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "consola.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include "constants.hpp"

using namespace std;

#define CMD_LOAD    "load"
#define CMD_ADD     "addAndInc"
#define CMD_MEMBER  "member"
#define CMD_MAXIMUM "maximum"
#define CMD_QUIT    "quit"
#define CMD_SQUIT   "q"

static unsigned int np;

// Crea un ConcurrentHashMap distribuido
static void load(list<string> params) {
    MPI_Request req;

    for (list<string>::iterator it=params.begin(); it != params.end(); ++it) {
        string filename = *it;

        // Transformar el string en char* para enviar
        char *filenamePointer = (char *) malloc(filename.size() + 1);
        strcpy(filenamePointer, filename.c_str());

        // Mandar mensaje a todos
        for (unsigned int i = 0; i < np; i++) {
            MPI_Isend(filenamePointer, filename.size() + 1, MPI_CHAR, i, LOAD_REQ_TAG, MPI_COMM_WORLD, &req);
        }

        // Esperar a que uno me indique que lo lee
        MPI_Status status;
        MPI_Recv(NULL, 0, MPI_CHAR, MPI_ANY_SOURCE, LOAD_ACCEPT_TAG, MPI_COMM_WORLD, &status);
        unsigned int reader = status.MPI_SOURCE;
        // printf("Respondio el nodo %d\n", reader);

        // Avisarle a ese que lo lea y al resto que no
        int accepted = ACCEPTED;
        int rejected = REJECTED;
        for (unsigned int i = 0; i < np; i++) {
            if (i == reader)
                MPI_Isend(&accepted, 1, MPI_INT, i, LOAD_ORDER_TAG, MPI_COMM_WORLD, &req);
            else
                MPI_Isend(&rejected, 1, MPI_INT, i, LOAD_ORDER_TAG, MPI_COMM_WORLD, &req);
        }

        free(filenamePointer);
    }

    cout << "La listá esta procesada" << endl;
}

// Esta función debe avisar a todos los nodos que deben terminar
static void quit() {
    MPI_Request req;
    for (unsigned int i = 1; i < np; ++i){
        MPI_Isend(NULL, 0, MPI_CHAR, i, QUIT_TAG, MPI_COMM_WORLD, &req);
    }
}

// Esta función calcula el máximo con todos los nodos
static void maximum() {
    pair<string, unsigned int> result;

    // TODO: Implementar
    string str("a");
    result = make_pair(str,10);

    cout << "El máximo es <" << result.first <<"," << result.second << ">" << endl;
}

// Esta función busca la existencia de *key* en algún nodo
static void member(string key) {
    bool esta = false;
    MPI_Request req;

    // Transformar el string en char* para enviar
    char *keyPointer = (char *) malloc(key.size() + 1);
    strcpy(keyPointer, key.c_str());

    // Mandar mensaje a todos
    for (unsigned int i = 0; i < np; i++) {
        MPI_Isend(keyPointer, key.size() + 1, MPI_CHAR, i, MEMBER_TAG, MPI_COMM_WORLD, &req);
    }

    free(keyPointer);

    cout << "El string <" << key << (esta ? ">" : "> no") << " está" << endl;
}


// Esta función suma uno a *key* en algún nodo
static void addAndInc(string key) {

    // TODO: Implementar

    cout << "Agregado: " << key << endl;
}


/* static int procesar_comandos()
La función toma comandos por consola e invoca a las funciones correspondientes
Si devuelve true, significa que el proceso consola debe terminar
Si devuelve false, significa que debe seguir recibiendo un nuevo comando
*/

static bool procesar_comandos() {

    char buffer[BUFFER_SIZE];
    size_t buffer_length;
    char *res, *first_param, *second_param;

    // Mi mamá no me deja usar gets :(
    res = fgets(buffer, sizeof(buffer), stdin);

    // Permitimos salir con EOF
    if (res==NULL)
        return true;

    buffer_length = strlen(buffer);
    // Si es un ENTER, continuamos
    if (buffer_length<=1)
        return false;

    // Sacamos último carácter
    buffer[buffer_length-1] = '\0';

    // Obtenemos el primer parámetro
    first_param = strtok(buffer, " ");

    if (strncmp(first_param, CMD_QUIT, sizeof(CMD_QUIT))==0 ||
        strncmp(first_param, CMD_SQUIT, sizeof(CMD_SQUIT))==0) {

        quit();
        return true;
    }

    if (strncmp(first_param, CMD_MAXIMUM, sizeof(CMD_MAXIMUM))==0) {
        maximum();
        return false;
    }

    // Obtenemos el segundo parámetro
    second_param = strtok(NULL, " ");
    if (strncmp(first_param, CMD_MEMBER, sizeof(CMD_MEMBER))==0) {
        if (second_param != NULL) {
            string s(second_param);
            member(s);
        }
        else {
            printf("Falta un parámetro\n");
        }
        return false;
    }

    if (strncmp(first_param, CMD_ADD, sizeof(CMD_ADD))==0) {
        if (second_param != NULL) {
            string s(second_param);
            addAndInc(s);
        }
        else {
            printf("Falta un parámetro\n");
        }
        return false;
    }

    if (strncmp(first_param, CMD_LOAD, sizeof(CMD_LOAD))==0) {
        list<string> params;
        while (second_param != NULL)
        {
            string s(second_param);
            params.push_back(s);
            second_param = strtok(NULL, " ");
        }

        load(params);
        return false;
    }

    printf("Comando no reconocido");
    return false;
}

void consola(unsigned int np_param) {
    np = np_param;
    printf("Comandos disponibles:\n");
    printf("  "CMD_LOAD" <arch_1> <arch_2> ... <arch_n>\n");
    printf("  "CMD_ADD" <string>\n");
    printf("  "CMD_MEMBER" <string>\n");
    printf("  "CMD_MAXIMUM"\n");
    printf("  "CMD_SQUIT"|"CMD_QUIT"\n");

    bool fin = false;
    while (!fin) {
        printf("> ");
        fflush(stdout);
        fin = procesar_comandos();
    }
}
