/****************************************************************************
*                       PRACTICA 2 SO 22/23                                 *
*                                                                           *
*                           MINISHELL                                       *
*                                                                           *
*****************************************************************************/

/*===========================================================================
=                 VARIABLES, DEFINES Y INCLUDES                             =
============================================================================*/

//DIRECTIVA DEL PROCESADOR
#define _POSIX_C_SOURCE 200112L

#define RESET_FORMATO "\x1b[0m"
#define NEGRO_T "\x1b[30m"
#define NEGRO_F "\x1b[40m"
#define ROJO_T "\x1b[31m"
#define VERDE_T "\x1b[32m"
#define AMARILLO_T "\x1b[33m"
#define AZUL_T "\x1b[34m"
#define MAGENTA_T "\x1b[35m"
#define CYAN_T "\x1b[36m"
#define BLANCO_T "\x1b[37m"
#define NEGRITA "\x1b[1m"

//TAMAÑOS
#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64

char const PROMPT = '$';

//INCLUDES
#include <errno.h>  //errno
#include <stdio.h>  //printf(), fflush(), fgets(), stdout, stdin, stderr...
#include <stdlib.h> //setenv(), getenv()
#include <string.h> //strcmp(), strtok(), strerror()
#include <unistd.h> //NULL, getcwd(), chdir()
#include <signal.h>
#include <sys/file.h> // file library, open()
#include <sys/types.h> //pid_t
#include <sys/wait.h>  //wait()
#include <sys/stat.h>

char *read_line(char *line); //HECHO
int parse_args(char **args, char *line); //HECHO
int execute_line(char *line);
int internal_fg(char **args);
int internal_bg(char **args);
int check_internal(char **args);
int internal_cd(char **args);   
int internal_export(char **args); 
int internal_source(char **args); 
int internal_jobs();




/*============================================================================
=                            FUNCIONES Y MÉTODOS                             =
=============================================================================*/
char line[COMMAND_LINE_SIZE]; // #define COMMAND_LINE_SIZE 1024
int main(){

while (1) {
   if (read_line(line)){
       execute_line(line);
   }
}
return 0;
}


void imprimir_prompt() {
    //"%s" = cadena de caracteres terminada con "\0"
    printf(NEGRITA ROJO_T "%s" BLANCO_T ":", getenv("USER"));
    //"%c"= imprime el caracter ASCII correspondiente
    printf(AMARILLO_T "MINISHELL" BLANCO_T "%c " RESET_FORMATO, PROMPT);

    printf("%c ", PROMPT);

    //forzamos el vaciado del buffer de salida
    fflush(stdout);
    return;
}

/*
* Funcion: read_line()
*------------------------
*Devolverá un puntero a la línea leída
*
*/

char *read_line(char *line){

 imprimir_prompt();
 //fgets: Función que se encarga de leer o almacenar una cadena de caracteres introducida mediante el teclado.
 char *ptr = fgets(line,COMMAND_LINE_SIZE,stdin);
  if (ptr) {
        // ELiminamos el salto de línea (ASCII 10) sustituyéndolo por el \0
        char *pos = strchr(line, 10);
        if (pos != NULL){
            *pos = '\0';
        } 
	}  else {   //ptr==NULL 
        printf("\r");
        if (feof(stdin)) { //se ha pulsado Ctrl+D
            fprintf(stderr,"Hasta la proxima, Adios!\n");
            exit(0);
        }   
    }
    return ptr;
}
/*
* Funcion: execute_line()
*------------------------
* 
*/
int execute_line(char *line){

 char *args[ARGS_SIZE];
//obtener la linea fragmentada en tokens
 parse_args(args,line);
 
//Si hay algo dentro de args mira si se trata de un comando interno
 if(args[0]){
    check_internal(args);
 }else {
    return -1;
 }
    return 0;
}
/*
* Funcion: parse_args()
*------------------------
* Muestra por pantalla el número de tokens y su valor para comprobar su correcto funcionamiento
*
*/
int parse_args(char **args, char *line) {
    int i = 0;

    //Troceamos la línea obtenida en tokens y lo metemos en un vector
    args[i] = strtok(line, " \t\n\r");

    // si args[i]!= NULL && *args[i]!='#' pasamos al siguiente token
    while (args[i] && args[i][0] != '#') { 
        i++;
        args[i] = strtok(NULL, " \t\n\r");

    }
    //si el ultimo token no es NULL lo convertimos en NULL
    if (args[i]) {
        args[i] = NULL; // por si el último token es el símbolo comentario
    }
    return i;
}

/*
*   Función: check_internal
*   -----------------
*   Comprobamos si args[] es un comando interno y llamamos a su respectivo en caso
*   de serlo.
*   
*/
int check_internal(char **args) {
    if (strcmp(args[0], "cd")==0){
        return internal_cd(args);
}
    if (strcmp(args[0], "export")==0){
        return internal_export(args);
    }
    if (strcmp(args[0], "source")==0){
        return internal_source(args);
        }
    if (strcmp(args[0], "jobs")==0){
        return internal_jobs(args);
        }
    if (strcmp(args[0], "fg")==0){
        return internal_fg(args);
    }
    if (strcmp(args[0], "bg")==0){
        return internal_bg(args);
    }
    if (strcmp(args[0], "exit")==0){
        exit(0);
    }
    return 0; // no es un comando interno
}


int internal_cd(char **args) {
    printf("[internal_cd()→ comando interno no implementado]\n");
    return 1;
} 

int internal_export(char **args) {
    printf("[internal_export()→ comando interno no implementado]\n");
    return 1;
}

int internal_source(char **args) {
    printf("[internal_source()→ comando interno no implementado]\n");
    return 1;
}

int internal_jobs(char **args) {
    #if DEBUGN1 
        printf("[internal_jobs()→ Esta función mostrará el PID de los procesos que no estén en foreground]\n");
    #endif
    return 1;
}

int internal_fg(char **args) {
    #if DEBUGN1 
        printf("[internal_fg()→ Esta función enviará un trabajo detenido al foreground reactivando su ejecución, o uno del background al foreground ]\n");
    #endif
    return 1;
}

int internal_bg(char **args) {
    #if DEBUGN1 
        printf("[internal_bg()→ Esta función reactivará un proceso detenido para que siga ejecutándose pero en segundo plano]\n");
    #endif
    return 1;
}

