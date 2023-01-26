#include "my_lib.c"
#include <stdio.h>
#include <pthread.h>
#define N_ITERACIONES 1000000
#define NUM_THREADS  10

char nombreFichero[1024];
pthread_mutex_t semaforo = PTHREAD_MUTEX_INITIALIZER;
void *worker(void *ptr);
struct my_stack* pila;
int OriginalLenght=0;

int main (int argc, char *argv[]){
    //si se ha pasado el nombre del fichero por consola lo guardamos en 
    if (argc > 1){
        strcpy(nombreFichero, argv[1]);
    }
    //ERROR
    else{
        fprintf(stderr, "Error, sintaxis: ./stack_counters <nombre_fichero_pila>\n");
    }

    // Verificar si la pila existe
    pila = my_stack_read(argv[1]);
    //si la pila no existe la creamos
    if (!pila){
        pila = my_stack_init(sizeof(int));
    }else{
        //si la pila existe y tiene menos de 10 nodos vamos agregando nuevos
        //nodos con el valor inicializado en 0
        while (my_stack_len(pila)<10){
            my_stack_push(pila,NULL);
            }
        //si la pila existe y tiene mas de 10 nodos vamos quitando nodos
        while(mys_stack_len(pila)>10){
            my_stack_pop(pila);
        }    
        
    }
    exit(0);

    pthread_t identificadores[NUM_THREADS];  
    //realizamos un bucle para crear los hilos
    for (int i = 0; i < NUM_THREADS; i++){
        pthread_create(&identificadores[i], NULL, worker, NULL);
        printf("%d) Thread %ld created\n", i, identificadores[i]);
    }

    printf("\nThreads: %d, Itreations: %d \n", NUM_THREADS, N_ITERACIONES);
    printf("stack->size: %d \n", pila->size);
    printf("original stack lenght: %d \n", OriginalLenght);
    //printf("initial stack content: %s \n\n", my_stack_));
    printf("new stack lenght: %d \n\n", my_stack_len(pila));

        //bloqueamos el hilo principal
        pthread_join(identificadores[0],NULL);
    
        //guardamos la pila en el fichero
        my_stack_write(pila,argv[1]);
        printf("Writen elements form stack to file: %d\n", my_stack_write(pila,argv[1]));
        pthread_exit(0);
        int x =my_stack_purge(pila);
        printf("Released bytes: %d\n", x);
    
}

void *worker(void *ptr){

    for (int i = 0; i < N_ITERACIONES; i++){
        //cogo el primer valor y pongo en espera al reesto de procesos
        //para evitar que accedan a la pila
        pthread_mutex_lock(&semaforo);
        //saco el valor de la pila y lo incremento
        int *data = my_stack_pop(pila);
        *data=*data+1;
        //introduzco el valor ya incrementado
        my_stack_push(pila,data);
        //desbloqueo la pila
        pthread_mutex_unlock(&semaforo);
    }
       pthread_exit(NULL);
    
}
