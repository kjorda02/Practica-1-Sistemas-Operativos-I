// Jose Luque Mut, Krishna Jorda Jimenez, Valentino Coppola Ferrari
#include "my_lib.h"
#include <stdbool.h>

size_t my_strlen(const char *str) {
    size_t sum=0;
    char aux=*str; //Tengo el elemento 0 del array

    while (aux !='\0') {
        sum++;
        str++;
        aux=*str;
    }
    return sum;
}

int my_strcmp ( const char *ch1, const char *ch2){

    while(*ch1!='\0' && *ch2!='\0'){    // Mientras la posicion considerada no sea null en ninguna de las 2 palabras
        if(*ch1 != *ch2){               // Si la letra es diferente en una palabra de la otra
            return *ch1-*ch2;           // Devolver la diferencia entre las 2
        } else{
            ch1++;                      // Si son iguales pasar a la siguiente posicion
            ch2++;
        }

    }
    return 0;
}

/*
    my_strcpy: Asigna todos los caracteres de una string pasada por argumento a otra string pasada por argumento

    dest:   Puntero a el array destino (donde se va a copiar la string fuente)
    src:    String a copiar (puntero a el primer caracter)
    salida: Puntero a el array destino (devuelve dest)
*/
char *my_strcpy(char *dest, const char *src){
    char *puntero = dest; // Guardamos dest para no perderlo

    while(*src != '\0'){    // Mientras no llegamos al final de src
        *dest = *src;       // Asignamos el contenido de el caracter apuntado por src a la posicion de memoria apuntada por dest
        src++;              // Pasamos a el siguiente caracter (en fuente)
        dest++;             // Pasamos a el siguiente caracter (en destino)
    }

    *dest = '\0';           // Importante para finalizar la cadena de dest

    return puntero;         // Devolvemos dest
}


/*
    my_strncpy: Asigna los primeros n caracteres de una string pasada por argumento a otra string pasada por argumento
    ademas, si se queda sin caracteres en src pero no ha copiado los n caracteres aun, copiara bytes nulos ('\0')

    dest:   Puntero a el array destino (donde se va a copiar la string fuente)
    src:    String a copiar (puntero a el primer caracter)
    n:      Numero de caracteres a copiar
    salida: Puntero a el array destino (devuelve dest)
*/
char *my_strncpy(char *dest, const char *src, size_t n){
    bool fin = false;
    for (int i = 0; i < n; i++){ // Iteramos el numero n veces ya que hay que poner \0 si se acaba src
        if (!fin){              // Si no hemos llegado al final de src copiamos
            dest[i] = src[i];
            if (!src[i]){       // Si estamos copiando \0 indicamos que hemos llegado al final de src
                fin = true;
            }
        }else{
            dest[i] = '\0';     // Si hemos llegado al final de src metemos \0s en dest hasta acabar los n caracteres
        }
        
    }
    return dest;
}

char *my_strcat(char *dest, const char *src) {
    char aux;
    int n=my_strlen(dest); //obtenemos la longitud de dest
    char *ptraux=dest; //Guardamos el valor a devolver

    dest=dest+(n); //nos situamos al final de la cadena apuntada por dest
    aux=*src;

    //copiamos la cadena apuntada por src 
    while(aux!='\0') { 
        *dest=aux;
        src++;
        dest++;
        aux=*src;

    }
    return ptraux;
}

char *my_strchr(const char *s, int caracter){
    int encontrado=0;
    while (*s!='\0' && encontrado==0){  //mientras el elemento no sea un espacio y no hayamaos
                                        //encontrado el caracter

        if (*s==caracter){              // si el contenido del apuntador es el que estamos
                                        // buscando, encontrado =1
            encontrado =1;
        }else{                          // si el caracter de la cadena no es el que estamos 
                                        //buscando pasamos al siguiente    
            s++;
        }
    }                                   //Si salimos del while quiere decir que hemos encontrado
                                        //el caracter o que hemos recorrido toda la cadena sin haber lo
                                         //encontrado

    if (encontrado==0){                 //si no hemos encontrado la palabra devolvemos NULL
        return NULL;
    } else                              //en otro caso es que lo hemos encontrado y devolvemos el valor
                                        //del apuntador
                            
    return (char *)s;
}

/*  ========================================================================================================================================================================
                                                                                | PARTE 2 |
    ========================================================================================================================================================================
*/

/*
    my_stack_init: Reserva espacio para una variable de tipo struct my_stack, y inicializa la misma con top = NULL y size con el valor que nos pasan por parametro

    size: Argumento de entrada que indica el tamaño de los datos que contendra la pila
    salida: Puntero a la pila recien creada
*/
struct my_stack *my_stack_init(int size){
    struct my_stack *stackPt = malloc(sizeof(struct my_stack)); // Hacemos esto para crear el struct my_stack en el heap y no en la pila
    stackPt->top = NULL;                                        // Usamos -> para asignar valores a los campos del struct apuntados por el puntero stackPt
    stackPt->size = size;

    return stackPt;
}

/*
    my_stack_push: Añade un nuevo nodo a la pila.

    stack:  Puntero a un struct my_stack (la pila en si a la que queremos añadir el nodo)
    data: Puntero a los datos que contendra el nuevo nodo de la pila

    salida: 0 si no hay error, -1 si hay error
*/
int my_stack_push(struct my_stack *stack, void *data){
    if (!(stack) || (stack->size < 1)){     // Si el puntero a la pila es NULL o si la longitud de los datos no es >0, devolver error (-1)
        return -1;
    }

    struct my_stack_node *prev_top = stack->top;    // Guardamos el puntero al nodo superior

    struct my_stack_node *stackNodePt = malloc(sizeof(struct my_stack_node));   // Creamos un nuevo nodo en el heap
    stackNodePt->next = prev_top;   // El puntero next del nuevo nodo apunta a donde apuntaba el puntero top de la pila
    stackNodePt->data = data;       // El puntero data apuntara a el puntero a los datos que nos pasan por parametro

    stack->top = stackNodePt;       // El puntero top ahora apuntara al nuevo nodo
    return 0;
}


/*
    my_stack_pop: Elimina un nodo de la pila (el ultimo que se habia añadido obviamente)

    stack:  Puntero a un struct my_stack (la pila en si de la cual queremos eliminar un nodo)

    salida: Puntero a los datos del nodo elimindado
*/
void* my_stack_pop(struct my_stack *stack){
    if (!stack->top){                   // Si no hay elementos en la pila, devolver NULL directamente
        return NULL;
    }

    void* datos = (stack->top)->data;   // Guardar el puntero a los datos del nodo que vamos a eliminar

    struct my_stack_node *sigPt = (stack->top)->next;   // Guardamos el puntero al nodo al que apunta el nodo que vamos a eliminar (el nuevo nodo superior)
    free(stack->top);                   // Liberamos la memoria del nodo superior (lo eliminamos)
    stack->top = sigPt;                 // El segundo nodo superior gurdado previamente se convierte en el nuevo nodo superior

    return datos;
}

int my_stack_len(struct my_stack *stack) {
    int len=0;
    struct my_stack_node *aux =stack->top;

    while(aux!=NULL){
        len++;
        aux=aux->next;


    }
    return len;
}

/*

*/
int my_stack_purge(struct my_stack *stack) {
    int bytes=0;
    struct my_stack_node *aux=stack->top;

    while(aux!=NULL){
        struct my_stack_node *aux2=aux;
        aux=aux2->next;
        free(aux2->data);
        bytes+=stack->size; 
        free(aux2);
        bytes+=sizeof(struct my_stack_node);


    }
    free(stack);
    bytes+=sizeof(struct my_stack);
    return bytes;

}

/*
struct my_stack *my_stack_read(char *filename){
    return NULL;
}*/

struct my_stack *my_stack_read(char *filename){
    struct my_stack *s;
    int size;
    int fd2=open(filename,O_RDONLY);
    if (fd2 == -1){
        return NULL;
    }

    //leemos size
    read(fd2,&size,sizeof(int));
    s=my_stack_init(size);

    void *data;
    data=malloc(size); //Reservamos espacio para data
    while(read(fd2,data,size)>0){
        //leemos data y lo escribmos en la nueva pila
        my_stack_push(s,data);

        data=malloc(size);

    }
    close(fd2);
    return s;
}

/*int my_stack_write(struct my_stack *stack, char *filename){
    return 0;
}*/

int my_stack_write(struct my_stack *stack, char * filename) {

  int fichero = open(filename, O_WRONLY | O_CREAT | 
  O_TRUNC, S_IRUSR | S_IWUSR);

  // Escrivimos el tamaño de los datos del fichero
  int TamañoFichero = write(fichero, & (stack -> size), sizeof(stack -> size));
  
  if (TamañoFichero == -1) {
    return -1;
  }
  int TamañoDatos = stack -> size;

  // Creamos una pila auxiliar del mismo tamaño que la pila original
  struct my_stack *pila_aux = my_stack_init(TamañoDatos);
  void *data_original_stack;

  // vamos copiando los nodos de la pila original a la auxiliar, uno a uno
  struct my_stack_node *ApuntadorNodo = stack -> top;
  int indice = 0;
  while (ApuntadorNodo != NULL) {
    data_original_stack = ApuntadorNodo -> data;
    my_stack_push(pila_aux, data_original_stack);
    ApuntadorNodo = ApuntadorNodo -> next;
  }

  // Vamos haciendo pops i escribiendo los datos de los nodos de la pila auxiliar
  // al fitchero, a cada iteració augmentamos un contador d'elements
  void *AgruparDatos = my_stack_pop(pila_aux);
  while (AgruparDatos != NULL) {
    if (write(fichero, AgruparDatos, TamañoDatos) == TamañoDatos) {
      indice++;
    }
    AgruparDatos = my_stack_pop(pila_aux);
  }

  //cerramos el fichero
  if (close(fichero) == -1) {
    return -1;
  }

  return indice;
}