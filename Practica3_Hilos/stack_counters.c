#include "my_lib.c"
#include <stdio.h>
#include <pthread.h>
#define N_ITERACIONES 1000000
#define NUM_THREADS  10

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
         ///if(my_stack_len(pila)<10){
            //Mientras haya menos de 10 nodos
            while(my_stack_len(pila)<NUM_THREADS){
            //numnodos= length de la pila
                numnodos=my_stack_len(pila);
            //creamos nuevo nodo en la pila
                my_stack_push(pila,numnodos);
            }
       // }
    }
    exit(0);

    pthread_t identificadores[NUM_THREADS];  
    for (int i = 0; i < NUM_THREADS; i++){
        pthread_create(&identificadores[i], NULL, worker, NULL);
    }
}

void *worker(void *ptr){

    for (int i = 0; i < N_ITERACIONES; i++){

        pthread_mutex_lock(&semaforo);
        void *data = my_stack_pop(pila);
        *data=*data+1;
        my_stack_push(pila,data)
        pthread_mutex_unlock(&semaforo);
        pthread_exit(NULL);
    }
        pthread_join(identificadores[0],NULL);
        my_stack_write(pila,nombreFichero);
        pthread_exit(NULL);
    
}
