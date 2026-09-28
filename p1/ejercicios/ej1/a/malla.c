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
    for (int i=0; i<ncolumnas; i++) {
        if (fork() == 0) { // HIJO
            break;
        }
    }
}

void creaVertical(int nfilas) {
    for (int i=1; i<nfilas; i++) {
        if (fork() > 0) {
            break; // PADRE
        }
    }
}

int main(int argc, char *argv[]) {
    int nfilas, ncolumnas;

    leerArgumentos(argc, argv, &nfilas, &ncolumnas);

    pid_t pidMalla = getpid();
    
    creaHorizontal(ncolumnas); // La malla crea la primera fila (hijos)

    if (getpid() != pidMalla) {
        creaVertical(nfilas); // Cada hijo crea su columna
    }

    sleep(30); //  Para poder ver pstree -c
    
    while (wait(NULL) > 0); // Para que los hijos mueran primero

    return 0;
}