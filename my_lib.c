#include "my_lib.h"
#include <stdbool.h>

size_t my_strlen(const char *str){
    return 0;
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

char *my_strcpy(char *dest, const char *src){
    char *puntero = dest; // Guardamos este puntero para no perderlo

    while(*src != '\0'){ // Mientras no llegamos al final de src
        *dest = *src;
        src++;
        dest++;
    }

    *dest = '\0'; // Importante para finalizar la cadena de dest

    return puntero;
}

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

char *my_strcat(char *dest, const char *src){
    return 0;
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

struct my_stack *my_stack_init(int size){
    struct my_stack *stackPt = malloc(sizeof(struct my_stack)); // Hacemos esto para crear el struct my_stack en el heap y no la pila
    stackPt->top = NULL;                                        // Usamos -> para asignar valores a los campos del struct apuntados por el puntero stackPt
    stackPt->size = size;

    return stackPt;
}

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

void* my_stack_pop(struct my_stack *stack){
    if (!stack->top){
        return NULL;
    }

    void* datos = (stack->top)->data;  // Guardar el puntero a los datos del nodo que vamos a eliminar

    struct my_stack_node *sigPt = (stack->top)->next;
    free(stack->top);
    stack->top = sigPt;

    return datos;
}

int my_stack_len(struct my_stack *stack){
    return -37;
}

int my_stack_purge(struct my_stack *stack){
    return 42;
}

struct my_stack *my_stack_read(char *filename){
    return NULL;
}

int my_stack_write(struct my_stack *stack, char *filename){
    return 0;
}