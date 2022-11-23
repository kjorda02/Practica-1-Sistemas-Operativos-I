/****************************************************************************
*                            FUNCIONS i MÈTODES                             *
*****************************************************************************/

/*********************   FUNCIONS INTERNALS DONADES   ***********************/

void imprimir_prompt();

char *read_line(char *line){
 imprimir_prompt();
 //fgets: Función que se encarga de leer o almacenar una cadena de caracteres introducida mediante el teclado.
 char *pnt = fgets(cadena,NUMERO_CARACTERES,stdin)
  if (ptr) {
        // ELiminamos el salto de línea (ASCII 10) sustituyéndolo por el \0
        char *pos = strchr(line, 10);
        if (pos != NULL){
            *pos = '\0';
        } 
	}  else {   //ptr==NULL por error o eof
        printf("\r");
        if (feof(stdin)) { //se ha pulsado Ctrl+D
            fprintf(stderr,"Bye bye\n");
            exit(0);
        }   
    }
    return ptr;
}

/*
*   Funcio: check_internal
*   -----------------
*   Retorna la funció si es interna
*   
*   char **args: argu
*/
int parse_args(char **args, char *line) {
    int i = 0;

    args[i] = strtok(line, " \t\n\r");
    
    while (args[i] && args[i][0] != '#') { // args[i]!= NULL && *args[i]!='#'
        i++;
        args[i] = strtok(NULL, " \t\n\r");

    }
    //si el ultimo token no es NULL lo convertimos en NULL
    if (args[i]) {
        args[i] = NULL; // por si el último token es el símbolo comentario
    }
    return i;
}
