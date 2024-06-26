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
#define DEBUGN2 1
#define FAILURE -1
#define SUCCESS 0

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
int internal_cd(char** args);



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

char* buscarComilla(char* token){
    char* comilla = strchr(token, '"');

    while(comilla && (comilla != token) && (*(comilla - 1) == '\\')){
        *(comilla - 1) = '\0';
        char aux[COMMAND_LINE_SIZE];
        strcpy(aux, comilla);
        strcat(token, aux);
        comilla = strchr(comilla, '"');
    }

    return comilla;
}

int buscarBackslash(char* token){  // Comprueba si el ultimo caracter del token es '\', si hay 2 backslash se cancelan
    char* fin = strchr(token, '\0');

    // Eliminar secuencias de "\\" (2 backslash seguidos), quitando el primer '\'
    for (char* i = (fin - 1); i > token; i--){  // Recorremos el token desde el utimo caracter hacia el principio
        if (*i == '\\' && *(i - 1) == '\\'){  // Si encuentra un '\' precedido de otro '\', elimina el primer '\'
            *(i - 1) = '\0';
            char aux[COMMAND_LINE_SIZE];
            strcpy(aux, i);
            strcat(token, aux);
            i--; // Hay que decrementar el contador ya que hemos eliminado un '\'
        }
    }

    fin = strchr(token, '\0');
    if ((fin > token) && *(fin - 1) == '\\'){
        *(fin - 1) = '\0';
        return 1;
    }
    else{
        return 0;
    }
}

/*
    Recibe el array de tokens por argumento
    Cambia el directorio y la variable de entorno PWD dependiendo en los tokens
    Devuelve 1 (TRUE) si ha ido bien y -1 si ha habido error
*/
int internal_cd(char **args) {
    // Obtenemos la ruta a la que cambiar a partir de los argumentos/tokens
    char* ruta;
    if (args[1] == NULL){  // Si no tiene ningun argumento (solo 'cd')
        ruta = getenv("HOME");  // Guarda la ruta a 'home'
    }else{
        char str[COMMAND_LINE_SIZE];
        str[0] = '\0';
        // Parse comillas
        if (args[1][0] == '"'){
            strcat(str, args[1]+1);  // Añade el primer argumento sin la comilla inicial

            for (int i = 2; args[i] != NULL; i++){
                strcat(str, " ");
                char* comillas = strchr(args[i],'"');  // Busca el caracter '"' en el token
                if (comillas != NULL){
                    *comillas = '\0';
                    args[i+1] = NULL;  // Si ha encontrado comillas hemos acabado y podemos hacer esto porque el ultimo puntero de args siempre es NULL
                }
                strcat(str, args[i]);
            }
        }else{
            // Parse no comillas
            for (int i = 1; args[i] != NULL; i++){
                int seguirLeyendo = 0;
                char* backslash = strchr(args[i], '\\');  // Buscamos el caracter '\' en el token

                while (backslash != NULL){
                    if (*(backslash+1) != '\\'){  // Si el siguiente caracter no es '\'
                        *backslash = '\0';
                        if (*(backslash+1) != '\0'){ // Si el caracter '\' no es el ultimo lo eliminamos  (obtenemos las 2 strings y las juntamos sin el)
                            char parte2[strlen(args[i])];
                            strcpy(parte2, (backslash+1)); // No podemos usar (backslash+1) directamente en strcat porque C hace cosas raras
                            strcat(args[i], parte2);  // Eliminamos
                        }
                        else{  // Si es el ultimo
                            seguirLeyendo = 1;
                        }
                    }
                    else{  // Si el siguiente caracter es '\'
                        backslash++;
                    }
                    
                    backslash = strchr(backslash, '\\');  // Buscamos el siguiente '\' (empezando en la siguiente posicion del ultimo '\')
                }

                if (!seguirLeyendo){  // Si no ha encontrado un '\' que viniera seguido de un '\0'
                    args[i+1] = NULL;
                }

                strcat(str, args[i]);
                if (args[i+1] != NULL){
                    strcat(str, " ");
                }
            }
        }
        ruta = str;
    }

    // Cambiamos la ruta
    if (chdir(ruta)){  // Llama a chdir() para cambiar de ruta y si no devuelve 0 informa del error
        perror(ROJO_T"internal_cd()--> Error de chdir() al cambiar de ruta"RESET_FORMATO);
        return FAILURE;
    }

    // Obtener el nuevo 'current working directory' con getcwd()
    char cwd[COMMAND_LINE_SIZE];
    if (getcwd(cwd, COMMAND_LINE_SIZE) == NULL){  // Llama a getcwd() y si devuelve NULL informa del error
        perror(ROJO_T"internal_cd()--> Error de getcwd()\n"RESET_FORMATO);
        return FAILURE;
    }

    // Actualizamos la variable de entorno PWD
    if (setenv("PWD", cwd, 1)){
        perror(ROJO_T"internal_cd()--> Error de setenv() al actualizar la variable de entorno PWD"RESET_FORMATO);
        return FAILURE;
    }

    // Mensaje de debug
    #if DEBUGN2
        fprintf(stderr, GRIS_T"[internal_cd()--> Nuevo directorio actual: %s]\n", cwd);
    #endif
    return 1;
} 

/*
    Recibe el array de tokens por parametro
    Actualiza la variable y el valor especificados por args[1]
    Devuelve 1 (TRUE) para indicar que es un comando interno o -1 si ha habido error
*/
int internal_export(char **args) {
    if (args[1] == NULL || !strchr(args[1], '=')){
        fprintf(stderr, ROJO_T"internal_export()--> Error de sintaxis. Usar export Nombre=Valor\n"RESET_FORMATO);
        return FAILURE;
    }

    char* nombre = strtok(args[1], "=");
    char* valor = strtok(NULL, "=");

    char* valorInicial = getenv(nombre);
    if (valorInicial){
        #if DEBUGN2
        printf(GRIS_T"internal_export()--> Valor inicial de la variable de entorno %s: %s\n"RESET_FORMATO, nombre, valorInicial);
        #endif
    }else{
        fprintf(stderr, ROJO_T"internal_export()--> No se ha encontrado la variable de entorno %s\n"RESET_FORMATO, nombre);
        return FAILURE;
    }

    setenv(nombre, valor, 1);

    #if DEBUGN2
        printf(GRIS_T"internal_export()--> Nuevo valor de la variable de entorno %s: %s\n"RESET_FORMATO, nombre, getenv(nombre));
    #endif

    return 1;
}

int internal_source(char **args) {
    #if DEBUGN2
        fprintf(stderr, GRIS_T"[internal_source()→ comando interno no implementado]\n"RESET_FORMATO);
    #endif
    return 1;
}

int internal_jobs(char **args) {
    #if DEBUGN2
        fprintf(stderr, GRIS_T"[internal_jobs()→ Esta función mostrará el PID de los procesos que no estén en foreground]\n"RESET_FORMATO);
    #endif
    return 1;
}

int internal_fg(char **args) {
    #if DEBUGN2
        fprintf(stderr, GRIS_T"[internal_fg()→ Esta función enviará un trabajo detenido al foreground reactivando su ejecución, o uno del background al foreground ]\n"RESET_FORMATO);
    #endif
    return 1;
}

int internal_bg(char **args) {
    #if DEBUGN2
        fprintf(stderr, GRIS_T"[internal_bg()→ Esta función reactivará un proceso detenido para que siga ejecutándose pero en segundo plano]\n"RESET_FORMATO);
    #endif
    return 1;
}

