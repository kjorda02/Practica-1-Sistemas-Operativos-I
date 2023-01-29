#define NUM_THREADS 10
#define NODES 10
#define N_ITERACIONES 1000000

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_lib.h"

void *worker();
struct my_stack *init_stack(char *file);

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static struct my_stack *pila;


int main(int argc, char **argv)
{
    // Mirar si la sintaxis es correcta
    if (argc != 2)
    {
        fprintf(stderr, "Error de sintaxis: ./Practica3 nombre archivo\n");
        return EXIT_FAILURE;
    }
    // Mostramos la información básica
    printf("Threads: %d, Iterations: %d\n", NUM_THREADS, N_ITERACIONES);
    

    //Creamos la pila i el array de los identificadores
    pila = init_stack(argv[1]);
    pthread_t identificadores[NUM_THREADS];
    printf("stack->size: %d \n", pila->size);
    
    // Creamos los hilos
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&identificadores[i], NULL, worker, NULL);
        printf("%d) Thread %ld created\n", i, identificadores[i]);
    }
    
    // Esperamos a que finalizen el resto de hilos.
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(identificadores[i], NULL);
    }

    // Guardamos la pila en el fichero y vaciamos la pila
    int p = my_stack_write(pila, argv[1]);
    printf("Writen elements form stack to file: %d\n", p);
    p = my_stack_purge(pila);
    printf("Released bytes: %d\n", p);
    return EXIT_SUCCESS;
}


void *worker()
{
    for(int i=0;i < N_ITERACIONES;i++)
    {
        // leemos el valor de la pila
        pthread_mutex_lock(&mutex);
#ifdef PRINT
        printf("Soy el hilo %ld ejecutando pop\n", pthread_self());
#endif
        int *val = my_stack_pop(pila);
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
#ifdef PRINT
        printf("Soy el hilo %ld ejecutando push\n", pthread_self());
#endif
        //incrementamos en 1 el valor del hilo y lo devolvemos a la pila
        *val = *val + 1;
        my_stack_push(pila, val);

        pthread_mutex_unlock(&mutex);
    }
   
    pthread_exit(0);
}

//Método que inicializa la pila
struct my_stack *init_stack(char *file)
{
    
    struct my_stack *pila;
    pila = my_stack_read(file);

    // Si la pila existe entonces mira si cumple los requisitos 
    if (pila)
    {
        //Mostramos el numero de hilos inicial
        printf("initial stack length: %d\n", my_stack_len(pila));
       
        // Si hay mas o menos de 10 hilos
        if (my_stack_len(pila) != NODES)
        {
            // Si hay menos añadimos nuevos inicializados a 0
            if (my_stack_len(pila) < NODES)
            {
                while (my_stack_len(pila) != NODES)
                {
                    int *data = malloc(sizeof(int));
                    *data = 0;
                    my_stack_push(pila, data);
                }
            }
            // Si hay más quitamos nodos
            else
            {
                while (my_stack_len(pila) != NODES)
                {
                    my_stack_pop(pila);
                }
            }
        }
    }
    // Si no existe la pila la creamos
    else
    {
        pila = my_stack_init(sizeof(int));
        //Mostramos el numero de hilos inicial
        printf("initial stack length: %d\n", my_stack_len(pila));
    
        // Añadimos los hilos a la nueva pila
        while (my_stack_len(pila) != NODES)
        {
            int *data = malloc(sizeof(int));
            *data = 0;
            my_stack_push(pila, data);
        }
    }
    // mostramos la cantidad de hilos y devolvemos la pila
    printf("final stack length: %d\n", my_stack_len(pila));
    return pila;
    

}
