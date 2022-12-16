/****************************************************************************
*                       PRACTICA 2 SO 22/23                                 *
*                                                                           *
*                           MINISHELL                                       *
*                                                                           *
*****************************************************************************/

/*===========================================================================
=                 VARIABLES, DEFINES Y INCLUDES                             =
============================================================================*/

//DIRECTIVAS PARA EL PREPROCESADOR
#define _POSIX_C_SOURCE 200112L
#define DEBUGN1 1

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
#define GRIS_T "\x1b[94m"

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
#include <sys/types.h> //pid_t
#include <sys/wait.h>  //wait()

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
    printf(AMARILLO_T "%s" BLANCO_T "%c " RESET_FORMATO, getenv("PWD"), PROMPT);

    //forzamos el vaciado del buffer de salida (Es necesario dado que no hemos imprimido \n)
    fflush(stdout);
    return;
}

/*
    Imprime el prompt y lee una linea de consola con la funcion fgets()
    Parametros: line: Puntero (string) donde guardaremos la linea leida
    Devuelve:   El puntero a la linea leida
*/
char *read_line(char *line){
    imprimir_prompt();
    //fgets: Función que se encarga de leer o almacenar una cadena de caracteres introducida mediante el teclado.
    char *ptr = fgets(line,COMMAND_LINE_SIZE,stdin);
    if (ptr) {    // Si fgets no devuelve null (no ha habido error ni end-of-file)
        char *pos = strchr(line, '\n'); // Buscamos la primera ocurrencia de '\n'
        if (pos != NULL){
            *pos = '\0';    // Si ha encontrado '\n' lo sustituye por '\0'
        }
    } else {   // Si fgets devuelve null (hay end-of-file o error de entrada)
        printf("\r");
        if (feof(stdin)) { // Si se ha pulsado Ctrl+D (end-of-file)
            fprintf(stderr,"Hasta la proxima, Adios!\n");
            exit(0);
        }   
    }
    return ptr;
}


/*
    Recibe la linea leida de stdin por parametro
    Devuelve 0 si no ha habido error
*/
int execute_line(char *line){
    char* args[ARGS_SIZE];
    parse_args(args,line);  //obtener la linea fragmentada en tokens
 
    //Si hay algo dentro de args mira si se trata de un comando interno (y lo ejecuta)
    if(args[0]){
        check_internal(args);
        return 0;
    }
    return 1;
}

/*
    Argumentos: line: string de la linea leida de la linea de comandos
                args: Puntero al que asignaremos el array args[]
    Devuelve:   El numero de tokens (sin contar NULL)
    Trocea la linea line en diferentes tokens y los guarda en un array de tokens args
*/
int parse_args(char **args, char *line) {
    int i = 0;

    //Troceamos la línea obtenida en tokens y lo metemos en un vector
    args[i] = strtok(line, " \t\n\r");

    // si args[i]!= NULL && *args[i]!='#' pasamos al siguiente token
    while (args[i] && args[i][0] != '#') {
        #if DEBUGN1
            fprintf(stderr, GRIS_T"[parse_args()→token %d: %s]\n"RESET_FORMATO, i, args[i]);  // Mensaje de debug
        #endif
        i++;
        args[i] = strtok(NULL, " \t\n\r");
    }

    #if DEBUGN1
            fprintf(stderr, GRIS_T"[parse_args()→token %d: %s]\n"RESET_FORMATO, i, args[i]);  // Mensaje de debug
    #endif

    //si el ultimo token no es NULL lo convertimos en NULL
    if (args[i]) {
        args[i] = NULL; // por si el último token es el símbolo comentario
        #if DEBUGN1
            fprintf(stderr, GRIS_T"[parse_args()→token %d corregido: %s]\n"RESET_FORMATO, i, args[i]);  // Mensaje de debug
        #endif
    }

    return i;
}

/*
    Comprobamos si args[] es un comando interno y llamamos a su funcion correspondiente en caso de serlo.
    Parametros: args, el array de punteros a los tokens/argumentos
    Devuelve:   0 si no es un comando interno, 1 si se ha ejecutado un comando interno
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
    #if DEBUGN1 
        fprintf(stderr, GRIS_T"[internal_cd()→ Esta funcion cambiara de directorio]\n"RESET_FORMATO);
    #endif
    return 1;
} 

int internal_export(char **args) {
    #if DEBUGN1 
        fprintf(stderr, GRIS_T"[internal_export()→ Esta funcion asignara valores a variables de entorno]\n"RESET_FORMATO);
    #endif
    return 1;
}

int internal_source(char **args) {
    #if DEBUGN1 
        fprintf(stderr, GRIS_T"[internal_source()→ Esta funcion ejecutara los comandos de un fichero]\n"RESET_FORMATO);
    #endif
    return 1;
}

int internal_jobs(char **args) {
    #if DEBUGN1 
        fprintf(stderr, GRIS_T"[internal_jobs()→ Esta función mostrará el PID de los procesos que no estén en foreground]\n"RESET_FORMATO);
    #endif
    return 1;
}

int internal_fg(char **args) {
    #if DEBUGN1 
        fprintf(stderr, GRIS_T"[internal_fg()→ Esta función enviará un trabajo detenido al foreground reactivando su ejecución, o uno del background al foreground ]\n"RESET_FORMATO);
    #endif
    return 1;
}

int internal_bg(char **args) {
    #if DEBUGN1 
        fprintf(stderr, GRIS_T"[internal_bg()→ Esta función reactivará un proceso detenido para que siga ejecutándose pero en segundo plano]\n"RESET_FORMATO);
    #endif
    return 1;
}

