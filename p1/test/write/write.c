#include <stdio.h> // printf, perror
#include <stdlib.h> // exit
#include <string.h> // strcpy, strcat, strlen
#include <fcntl.h> // creat, O_* constantes
#include <unistd.h> // write, close

// variables.txt eliminado del repo por confidencialidad
int main(int argc, char *argv[], char *envp[]) {
    int df, cont=0;
    char buffer[1024];
    
    df=creat("variables.txt", 0755);
    
    if (df<0) {
        perror("Error al crear archivo");
        exit(-1);
    }
    while (envp[cont] != NULL) {
        //strcpy(buffer, environ[cont]);
        //strcat(buffer, "\n");
        snprintf(buffer, sizeof(buffer), "%s\n", envp[cont]); // PARA EVITAR SEGMENTATION FAULT

        
        if (write(df, buffer, strlen(buffer)) != strlen(buffer)) {
            perror("Error al escribir");
            break; /* NO SIGUE */
        }
        
        cont++;
    }
    
    close(df);
}