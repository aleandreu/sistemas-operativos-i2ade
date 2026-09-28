// Ejercicio 1. a) malla.c

#include <stdio.h>  // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED







void leerArgumentos(int nArgs, char *args[], int *x, int *y) {
    if (nArgs != 3 || atoi(args[1]) < 1 || atoi(args[2]) < 1) {
        printf("Error. Debes introducir un número válido de argumentos que sean > 0 \n");
        exit(1);
    }
    else {
        *x = atoi(args[1]); // FILAS
        *y = atoi(args[2]); // COLUMNAS
    }
}

void creaHorizontal(int ncolumnas) {
    bool soyHijo = false;
    for (int i=0; i<ncolumnas && !soyHijo; i++) {
        if (fork() == 0) {
            soyHijo = true;
        }
    }

    /*
    fork (0 --> x)
        si es hijo
        si es padre: continue
*/
}

void creaVertical(char *argv[]) {

    if ()

    for (int i=1; i<nfilas; i++) {

    }
    
    /* for (0 --> y) */
};

int main(int argc, char *argv[]) {
    int nfilas, ncolumnas;

    leerArgumentos(argc, argv, &nfilas, &ncolumnas);

    
    creaHorizontal(ncolumnas);

    creaVertical(nfilas);

    return 0;
}