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

struct my_stack *my_stack_init(int size){
    return NULL;
}

int my_stack_push(struct my_stack *stack, void *data){
    return 3;
}

void* my_stack_pop(struct my_stack *stack){
    return 0;
}

int my_stack_len(struct my_stack *stack){
    return 4;
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