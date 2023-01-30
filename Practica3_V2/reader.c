
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "my_lib.h"

int main(int argc, char **argv)
{
    // Miramos si la sintaxis es correcta
    if (argc != 2)
    {
        fprintf(stderr, "Sintaxis incorrecta: reader nombre_fichero\n");
        return EXIT_FAILURE;
    }
    struct my_stack *pila;
    pila = my_stack_read(argv[1]);

    // Si el archivo no existe
    if (!pila)
    {
        fprintf(stderr, "No existe el fichero: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    int length = 10; 
    int suma = 0;
    int minimo = INT_MAX;
    int maximo = INT_MIN;
    int media = 0;
    
    int *data;
    //reservamos espacio
    data = malloc(sizeof(int));
    //mientras queden elementos dentro de la pila
    while(my_stack_len(pila)>0){
        data = my_stack_pop(pila);

        suma=suma + *(data);

        if (*data <minimo ){
            minimo = *(data);
        }
          if (*data >maximo ){
            maximo = *(data);
        }
        printf("%d\n",*(data));
        
    }
    media = suma/10;
    printf("Sum: %d Min: %d Max: %d Average: %d\n",suma, minimo, maximo, media);
    my_stack_purge(pila);
}
