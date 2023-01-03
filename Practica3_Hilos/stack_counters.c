#include "my_lib.c"
#include <stdio.h>
#include <pthread.h>
#define N_ITERACIONES 1000000

char nombreFichero[1024];
pthread_mutex_t semaforo = PTHREAD_MUTEX_INITIALIZER;
void *worker(void *ptr);
struct my_stack* pila;

int main (int argc, char *argv[]){
    if (argc > 1){
        strcpy(nombreFichero, argv[1]);
    }
    else{
        fprintf(stderr, "Error, sintaxis: ./stack_counters <nombre_fichero_pila>\n");
    }

    // Verificar si la pila existe
    pila = my_stack_read(nombreFichero);
    if (!pila){
        pila = my_stack_init(sizeof(int));
    }
    else{

    }
    exit(0);

    pthread_t identificadores[10];  
    for (int i = 0; i < 10; i++){
        pthread_create(&identificadores[i], NULL, worker, NULL);
    }
}

void *worker(void *ptr){
    for (int i = 0; i < N_ITERACIONES; i++){

    }
}