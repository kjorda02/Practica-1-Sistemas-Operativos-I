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
#define SUCCES 0
#define FAILURE -1
#define N_JOBS 64

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
#include <limits.h>


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
struct info_job {
   pid_t pid;
   char status; // ‘N’, ’E’, ‘D’, ‘F’ (‘N’: ninguno, ‘E’: Ejecutándose y ‘D’: Detenido, ‘F’: Finalizado) 
   char cmd[COMMAND_LINE_SIZE]; // línea de comando asociada
};

//Declaración de statics
static struct info_job jobs_list [N_JOBS] ;
static char mi_shell[COMMAND_LINE_SIZE]; 


//Main
int main(int argc, char *argv[]){
    char cwd[COMMAND_LINE_SIZE];
    char x ='\0';
    jobs_list[0].pid=0;
    jobs_list[0].status='N';
    // copiamos el caracter [x] a los primeros "COMMAND_LINE_SIZE" caracteres de cwd
    memset(cwd, x, COMMAND_LINE_SIZE);

    strcpy(mi_shell,argv[0]);   //Obtenemos el comando de ejecución del minishell y lo guardamos en "mi_shell"

        if (getcwd(cwd, COMMAND_LINE_SIZE) != NULL) {
            printf("Directorio actual: %s\n", cwd);
        } else {
            perror("getcwd() error");
            return FAILURE;
        }
            return SUCCES;
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
        // Eliminamos el salto de línea (ASCII 10) sustituyéndolo por el \0
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
    pid_t pid, status;
    char cwd[COMMAND_LINE_SIZE];
    memset(cwd, '\0', sizeof(cwd)); 
    strcpy(cwd, line);

    //obtener la linea fragmentada en tokens
    parse_args(args,line);
 
    //Si hay algo dentro de args mira si se trata de un comando interno
    if(args[0]){
        check_internal(args);

            if(check_internal(args) == 0){
                pid=fork();

                if(pid<0){// si PID <0 entonces ERROR
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if(pid==0){ //proceso hijo
                    if(execvp(args[0],args)<0){//si execvp < 0 entonces ERROR
                        fprintf(stderr,"%s: no se encontró el comando \n",args[0]);
                        exit(EXIT_FAILURE);
                    }
                }
                else{//es el padre
                    //Actualizamos jobs_list
                    jobs_list[0].status="E";
                    strcpy(jobs_list[0].cmd,cwd);
                    //wait
                    waitpid(-1, &status, 0);
                    //reseteamos los datos
                    char cwd[COMMAND_LINE_SIZE];
                    char x ='\0';
                    jobs_list[0].pid=0;
                    jobs_list[0].status='N';
                    // copiamos el caracter [x] a los primeros "COMMAND_LINE_SIZE" caracteres de cwd
                    memset(cwd, x, COMMAND_LINE_SIZE);
                }
             }
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
        printf("Ruta: |%s|\n", ruta);
    }

    // Cambiamos la ruta
    if (chdir(ruta)){  // Llama a chdir() para cambiar de ruta y si no devuelve 0 informa del error
        perror(ROJO_T"Error de chdir() al cambiar de ruta"RESET_FORMATO);
        return FAILURE;
    }

    // Obtener el nuevo 'current working directory' con getcwd()
    char cwd[COMMAND_LINE_SIZE];
    if (getcwd(cwd, COMMAND_LINE_SIZE) == NULL){  // Llama a getcwd() y si devuelve NULL informa del error
        perror(ROJO_T"Error de getcwd()\n"RESET_FORMATO);
        return FAILURE;
    }

    // Actualizamos la variable de entorno PWD
    if (setenv("PWD", cwd, 1)){
        perror(ROJO_T"Error de setenv() al actualizar la variable de entorno PWD"RESET_FORMATO);
        return FAILURE;
    }

    // Mensaje de debug
    #if DEBUGN2
        fprintf(stderr, GRIS_T"[Nuevo directorio actual: %s]\n", cwd);
    #endif
    return 0;
} 


int internal_export(char **args) {
    printf("[internal_export()→ comando interno no implementado]\n");
    return 1;
}

int internal_source(char **args) {
    //Abrimos un fichero indicado por consola en modo lectura 
    FILE *fp;
    char *str[240];
    printf("Introduzca el nombre del archivo:\n");
    fp=fopen(read_line(args),"r");

     if(fp == NULL) {//si el fichero no existe error
      perror("Error opening file");
      return(-1);
    }
    //mientras no final de fichero
    while(fgets(str,240,fp)!=NULL){
        //busacmos saltos de linea y en caso de encontrarlos los cambiamos por "\0"
        char* saltoLinea(str, "\n");
        if (saltoLinea != NULL){
                *saltoLinea = '\0';
                }
        execute_line(str);
        //vaciamos el buffer
        fflush(str);
    }
    //cerramos fichero
    fclose(fp);
    return(0);
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

