#include "my_lib.c"
#include <limits.h>

struct  my_stack *pila;
int main(int argc, char *argv[]){
    //cargamos la pila que guardamos anteriormente
   pila = my_stack_read(argv[1]);

    if (!pila){
        fprintf(stderr, "Error, file not found: %s\n", argv[1]);

    }
    int length = my_stack_len(pila);
    int suma = 0;
    int minimo = 0;
    int maximo = 0;
    int media = 0;
    //mientras queden elementos dentro de la pila
    while(my_stack_len(pila)>0){
        int *data = my_stack_pop(pila);

        suma=suma + *data;

        if (*data <minimo ){
            minimo = *data;
        }
          if (*data >maximo ){
            maximo = *data;
        }
        
    }
    media = suma/length;
    printf("Sum: %d Min: %d Max: %d Average: %d\n",suma, minimo, maximo, media);
    my_stack_purge(pila);

}    
