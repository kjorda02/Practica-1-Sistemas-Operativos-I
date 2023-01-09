#include "my_lib.c"
#include <limits.h>

char nombreFichero[1024];
struct  my_stack *pila;
int main(int argc, char *argv[]){
  if (argc > 1){
      //guardamos el nombre del fichero 
       strcpy(nombreFichero, argv[1]);
    } else{
        fprintf(stderr, "Error, sintaxis: ./stack_counters <nombre_fichero_pila>\n");
    }
  //inicializamos el puntero de la pila
   pila = my_stack_read(argv[1]);
  //miramos si el fichero existe
    if (!pila){
        fprintf(stderr, "Error, file not found: %s\n", argv[1]);

    }
    //definimos e inicializamos las variables
    int length = my_stack_len(pila);
    int suma = 0;
    int minimo = 0;
    int maximo = 0;
    int media = 0;
    //mientras queden elementos dentro de la pila
    while(my_stack_len(pila)>0){
    //leemos un elemento de la pila
        int *data = my_stack_pop(pila);
    //sumamos el contenido
        suma=suma + *data;
    //si el contenido es menor que le minimo, este es el nuvo minimo
        if (*data <minimo ){
            minimo = *data;
        }
          if (*data >maximo ){
            maximo = *data;
        }
        
    }
    //la media será la suma total divido la cantidad de elementos
    media = suma/length;
    //printeamos el resultado
    printf("Items: %d Sum: %d Min: %d Max: %d Average: %d\n",suma, minimo, maximo, media);
    //liberamos la memoria 
    my_stack_purge(pila);
    
}    
