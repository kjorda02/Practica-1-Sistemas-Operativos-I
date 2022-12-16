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
#define DEBUGN6 1

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
#define SUCCESS 0
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
void ctrlc (int signum);
void reaper(int signum);
int is_background(char **args);
int jobs_list_add(pid_t pid, char status, char *cmd);
int jobs_list_find(pid_t pid);
int jobs_list_remove(int pos);
void ctrlz(int signum);

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

char line[COMMAND_LINE_SIZE]; // #define COMMAND_LINE_SIZE 1024
int n_pids = 0;

//Main
int main(int argc, char *argv[]){
    // Inicializa jobs_list[0] con el pid a 0, status a 'N' y el cmd con todos los carateres a '\0'
    jobs_list[0].pid = 0;
    jobs_list[0].status = 'N';
    memset(jobs_list[0].cmd, '\0', COMMAND_LINE_SIZE);
    signal(SIGCHLD, reaper);    // llamada al enterrador de zombies cuando un hijo acaba (señal SIGCHLD)
    signal(SIGINT, ctrlc);      // SIGINT es la señal de interrupcion que produce Ctrl+C
    signal(SIGTSTP, ctrlz);     // SIGTSTP es la señal de interrupcion que produce Ctrl+z

    strcpy(mi_shell, argv[0]);   //Obtenemos el comando de ejecución del minishell y lo guardamos en "mi_shell"

    while (1) {
        if (read_line(line)){
            execute_line(line);
        }
    }
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

    char *args[ARGS_SIZE];
    pid_t pid;
    char copiaLine[COMMAND_LINE_SIZE];  // Guardamos una copia de 'line' ya que parse_args la altera
    strcpy(copiaLine, line);

    //obtener la linea fragmentada en tokens
    parse_args(args,line);
 
    //Si hay algo dentro de args mira si se trata de un comando interno
    if(args[0]){
        int internal = check_internal(args);

        if(!internal){  // Si no es un comando interno (internal == 0)

            int isBackground = is_background(args);

            pid=fork();

            if(pid<0){// si PID < 0 entonces ERROR
                perror(ROJO_T"Error fork"RESET_FORMATO);
                exit(EXIT_FAILURE);
            }
            else if(pid==0){ //proceso hijo

                signal(SIGCHLD, SIG_DFL);   // ASOCIAMOS LA ACCIÓN POR DEFECTO A SIGCHLD 
                signal(SIGINT, SIG_IGN);	// INGNORAR LA SEÑAL SIGINT 
                signal(SIGTSTP, SIG_IGN);	// INGNORAR LA SEÑAL SIGINT 

                if(execvp(args[0],args)<0){// Ejecuta el comando externo (si execvp < 0 entonces ERROR)
                    fprintf(stderr,ROJO_T"%s: no se encontró el comando \n"RESET_FORMATO,args[0]);
                    exit(EXIT_FAILURE);
                }
            }
            else{// Es el padre

                //Asociamos el ctrlc a SIGINT
                signal(SIGINT, ctrlc);
                signal(SIGCHLD, reaper);

                if (!isBackground){  // Se ejecuta en foreground
                    //Actualizamos jobs_list
                    jobs_list[0].status='E';
                    strcpy(jobs_list[0].cmd, copiaLine);
                    jobs_list[0].pid=pid;

                    // Mientras exista proceso en primer plano, espera a una señal 
                    while(jobs_list[0].pid > 0){
                        pause();    // Esperamos mientras exista un proceso en 1er plano
                    }
                }
                else{  // Se ejecuta en background
                    if (!jobs_list_add(pid, 'E', copiaLine)){  // Si no hay error
                        printf("[%d] %d\t%c\t%s\n", n_pids, jobs_list[n_pids].pid, jobs_list[n_pids].status, jobs_list[n_pids].cmd);
                    }
                    else{
                        fprintf(stderr, ROJO_T "execute_line--> No queda suficiente espacio en la tabla de trabajos!\n" RESET_FORMATO);
                    }
                }
            }
        }
    }else {
        return -1;
    }
        return 0;
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
    }else{
        fprintf(stderr, ROJO_T"internal_export()--> No se ha encontrado la variable de entorno %s\n"RESET_FORMATO, nombre);
        return FAILURE;
    }

    setenv(nombre, valor, 1);

    return 1;
}


/*
    Recibe el array de tokens por parametro
    Lee el fichero indicado en args[1] y ejecuta todas sus lineas
    Devuelve 1 (TRUE) para indicar que es un comando interno o -1 si ha habido error
*/
int internal_source(char **args) {
    if (args[1] == NULL){
        perror(ROJO_T"Error de sintaxis. Usar la sintaxis \"source <nombre_fichero>\"");
        return FAILURE;
    }

    FILE *fichero = fopen(args[1], "r"); // Abrimos el fichero especificado en modo lectura 
    char str[COMMAND_LINE_SIZE]; // Guarda la linea leida del fichero para ejecutarla posteriormente 

    if (fichero == NULL) { // Si el fichero no existe error
        perror(ROJO_T"Error opening file"RESET_FORMATO);
        return(-1);
    }

    // Mientras no final de fichero
    while(fgets(str, COMMAND_LINE_SIZE, fichero) != NULL){
        fflush(fichero);  // Vaciamos el buffer
        char* saltoLinea = strchr(str, '\n'); // Busacmos el salto de linea
        if (saltoLinea != NULL){
            *saltoLinea = '\0';  // En caso de encontrarlo lo cambiamos por "\0"
        }
        execute_line(str);
    }

    fclose(fichero);  // Cerramos fichero
    return(1);
}

/*
  Recibe el array de argumentos por parametro
  Muestra una lista de trabajos en segundo plano
  Devuelve 1 para indicar que es un comando interno  
*/
int internal_jobs(char **args) {
    for (int i = 1; i <= n_pids; i++){
        printf("[%d] %d\t%c\t%s\n", i, jobs_list[i].pid, jobs_list[i].status, jobs_list[i].cmd);
    }
    return 1;
}

/*
    Recibe el array de argumentos por parametro
    Pasa el trabajo indicado a primer plano
    Devuelve 1 para indicar que es un comando interno
*/
int internal_fg(char **args) {
    if (!args[1] || args[2]){  // Si el primer argumento no es null o si hay 2 argumentos (no hay segfault porque no comprueba la segunda condicion si la primera es 1)
        fprintf(stderr, ROJO_T "Error de sintaxis. Usa 'fg <Nº trabajo>'\n" RESET_FORMATO);
        return 1;
    }

    int pos = atoi(args[1]);
    if ((pos == 0) || (pos > n_pids)){  // Si el primer argumento no es un numero o si es el numero 0 o si es un numero mayor que la posicion del ultimo trabajo
        fprintf(stderr, ROJO_T "Error: No se ha encontrado el trabajo '%s'\n" RESET_FORMATO, args[1]);
        return 1;
    }

    if (jobs_list[pos].status == 'D'){  // Si el estatus es detenido enviar la señal SIGCONT para que se reanude el proceso hijo
        kill(jobs_list[pos].pid, SIGCONT);
        #if DEBUGN6
            fprintf(stderr, GRIS_T"[internal_fg()--> Enviada la señal SIGCONT al trabajo %d con pid %d (%s)]\n" RESET_FORMATO, pos, jobs_list[pos].pid, jobs_list[pos].cmd);
        #endif
    }

    char* ampersand = strchr(jobs_list[pos].cmd, '&');
    if (ampersand){
        *(ampersand - 1) = '\0'; // Eliminamos la posicon anterior porque siempre habra un espacio antes del &
    }

    // Copiar datos
    jobs_list[0].pid = jobs_list[pos].pid;
    jobs_list[0].status = 'E';
    strcpy(jobs_list[0].cmd, jobs_list[pos].cmd);

    jobs_list_remove(pos);
    
    printf("%s\n", jobs_list[0].cmd);
    

    while (jobs_list[0].pid > 0){   // Esperamos a que el proceso acabe y el reaper resetee jobs_list[0]
        pause();
    }
    return 1;
}

/*
    Recibe el array de argumentos por parametro
    Reanuda el proceso en segundo plano indicado
    Devuelve 1 para indicar que es un comando interno
*/
int internal_bg(char **args) {
    if (!args[1] || args[2]){  // Si el primer argumento no es null o si hay 2 argumentos (no hay segfault porque no comprueba la segunda condicion si la primera es 1)
        fprintf(stderr, ROJO_T "Error de sintaxis. Usa 'bg <Nº trabajo>'\n" RESET_FORMATO);
        return 1;
    }

    int pos = atoi(args[1]);
    if ((pos == 0) || (pos > n_pids)){  // Si el primer argumento no es un numero o si es el numero 0 o si es un numero mayor que la posicion del ultimo trabajo
        fprintf(stderr, ROJO_T "Error: No se ha encontrado el trabajo '%s'\n" RESET_FORMATO, args[1]);
        return 1;
    }

    if (jobs_list[pos].status == 'E'){
        fprintf(stderr, ROJO_T "Error: el trabajo %d ya esta ejecuntandose en 2º plano\n" RESET_FORMATO, pos);
        return 1;
    }

    //Cambiar el status de ese trabajo a 'E' (ejecutandose)
    jobs_list[pos].status = 'E';

    //añadimos con strcat el bloque " &"
    strcat(jobs_list[pos].cmd, " &");  

    //Enviar la señal SIGCONT a jobs_list[pos].pid
    kill(jobs_list[pos].pid,SIGCONT);

    #if DEBUGN6
        fprintf(stderr, GRIS_T"[internal_bg()--> Enviada la señal SIGCONT al trabajo %d con pid %d (%s)]\n" RESET_FORMATO, pos, jobs_list[pos].pid, jobs_list[pos].cmd);
    #endif
    
    printf("[%d] %d\t%c\t%s\n", pos, jobs_list[pos].pid, jobs_list[pos].status, jobs_list[pos].cmd);

    return 1;
}


/*
    Manejador de la señal SIGCHLD, que se da cuando un hijo termina
    En este nivel resetea jobs_list[0] si el hijo que ha terminado era el que habia en primer plano
    recibe signum como parametro
*/
void reaper(int signum){
    signal(SIGCHLD, reaper);    // Volvemos a asociar la señal SIGCHLD al reaper (por si se restaura)
    char printReaper[4096];

    int status;
    int ended;
    ended=waitpid(-1, &status, WNOHANG);  // ended contiene el PID del proceso hijo que ha finalizado
    while (ended > 0) {
        if (ended == jobs_list[0].pid){  // Si el proceso que ha finalizado era el que estaba en primer plano resteamos jobs_list[0]
            jobs_list[0].pid = 0;
            jobs_list[0].status='F';
            memset(jobs_list[0].cmd, '\0', COMMAND_LINE_SIZE);
        }
        else{  // Si el proceso que ha finalizado era de segundo plano / parado reseteamos jobs_list[indice]
            int indice = jobs_list_find(ended);
            
            sprintf(printReaper, "\nTerminado PID %d (%s) en jobs_list[%d] ", jobs_list[indice].pid, jobs_list[indice].cmd, indice);
            write(1, printReaper, strlen(printReaper));
            if (WIFEXITED(status)){
                int estado = WEXITSTATUS(status);
                sprintf(printReaper, "con exit status %d\n"RESET_FORMATO, estado);
            } 
            else if(WIFSIGNALED(status)){
                int signal = WTERMSIG(status);
                printf(printReaper, "por la señal %d\n"RESET_FORMATO, signal);
            }
            write(1, printReaper, strlen(printReaper));

            jobs_list_remove(indice);
        }

        ended=waitpid(-1, &status, WNOHANG);
    }
}


/*
    Recibe signum como parametro de entrada
    Manejador de la señal SIGINT
    Mata al proceso en primer plano si hay uno y no es otro minishell
*/
void ctrlc (int signum){
	signal(SIGINT, ctrlc);  // ASOCIAMOS LA SEÑAL SIGINT A CTRLC (por si se restaura)
    char debugCtrlC[4096];
    
    sprintf(debugCtrlC, "\n");
    write(1, debugCtrlC, strlen(debugCtrlC));

	if(jobs_list[0].pid > 0){   // REVISAMOS SI HAY UN PROCESO EN FOREGROUND
		if(strcmp(mi_shell,jobs_list[0].cmd)!=0){   // Comprobamos que el proceso en foreground NO es el mini shell
			
			if(kill(jobs_list[0].pid,SIGTERM)==0){  // Enviar señal SIGTERM al proceso en foreground
			}
            else{
                sprintf(debugCtrlC, ROJO_T "ctrlc()--> Error al enviar la señal SIGTERM a %d (%s) por %d (%s)\n" RESET_FORMATO, jobs_list[0].pid, jobs_list[0].cmd, getpid(), mi_shell);
                write(2, debugCtrlC, strlen(debugCtrlC));
			}
		}
    }
}

/*
    Recibe el array de tokens por parametro
    Devuelve 1 si el comando se tiene que ejecutaro en background, 0 en otro caso
*/
int is_background(char **args){
    for (int i = 1; args[i] != NULL; i++){
        if (args[i][0] == '&'){
            args[i] = NULL;
            return 1;
        }
    }

    return 0;
}

/*
    Recibe los valores de los 3 campos de struct info_job
    Añade el trabajo al final de la lista
*/
int jobs_list_add(pid_t pid, char status, char* cmd){
    n_pids++;   // Tras incrementarlo, jobs_list[n_pìds] sera una posicion vacia

    if (n_pids < N_JOBS){   // Estrictamente menor dado que hay una posicion reservada para 1er plano
        jobs_list[n_pids].pid = pid;
        jobs_list[n_pids].status = status;
        strcpy(jobs_list[n_pids].cmd, cmd); // Importante hacer strcpy porque es un array type o algo
        return 0;
    }
    else{
        return -1;
    }
}

/*
    Recibe el pid del trabajo a buscar
    Devuelve el indice del trabajo encontrado en la lista
*/
int jobs_list_find(pid_t pid){
    for (int i = 0; i <= n_pids; i++){
        if (jobs_list[i].pid == pid){
            return i;
        }
    }
    return -1;
}

/*
    Recibe como parámetro la posición del trabajo que hay que eliminar
    Mueve el registro de la ultima posicion a la posicion que eliminamos y lo sobreescribe
*/
int  jobs_list_remove(int pos){
    if (pos <= n_pids){
        jobs_list[pos].pid = jobs_list[n_pids].pid;
        jobs_list[pos].status = jobs_list[n_pids].status;
        strcpy(jobs_list[pos].cmd, jobs_list[n_pids].cmd);
        n_pids--;
        return 0;
    }
    return -1;
}


void ctrlz(int signum){
    signal(SIGTSTP, ctrlz);  // ASOCIAMOS LA SEÑAL SIGINT A CTRLC (por si se restaura)
    char debugCtrlZ[4096];

    sprintf(debugCtrlZ, "\n");
    write(1, debugCtrlZ, strlen(debugCtrlZ));

    if (jobs_list[0].pid > 0){  // Si hay un proceso en foreground
        if (strcmp(jobs_list[0].cmd, mi_shell) != 0){  // Si no es otro minishell
            if(kill(jobs_list[0].pid, SIGSTOP) == 0){  // Enviar señal SIGSTOP al proceso en foreground
                jobs_list[0].status = 'D';  // Cambiamos el status a detenido
                jobs_list_add(jobs_list[0].pid, jobs_list[0].status, jobs_list[0].cmd);  // Añadimos el proceso a la tabla por el final (ya no puede estar en la posicion 0)

                // Reseteamos los datos de jobs_list[0]
                jobs_list[0].pid = 0;
                jobs_list[0].status = 'N';
                memset(jobs_list[0].cmd, '\0', COMMAND_LINE_SIZE);

                sprintf(debugCtrlZ, "[%d] %d\t%c\t%s\n", n_pids, jobs_list[n_pids].pid, jobs_list[n_pids].status, jobs_list[n_pids].cmd);
                write(1, debugCtrlZ, strlen(debugCtrlZ));
			}
            else{  // Si se ha producido un error al enviar la señal
                sprintf(debugCtrlZ, ROJO_T "ctrlz()--> Error al enviar la señal SIGSTOP a %d (%s) por %d (%s)\n" RESET_FORMATO, jobs_list[0].pid, jobs_list[0].cmd, getpid(), mi_shell);
                write(2, debugCtrlZ, strlen(debugCtrlZ));
            }
        }
    }
}