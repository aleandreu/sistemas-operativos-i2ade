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

void creaHorizontal(int ncolumnas) { // crear filas
    for (int i=0; i<ncolumnas; i++) {
        pid_t pid = fork();
        if (pid == 0) { // HIJO
            break;
        }
        else if (pid == -1) { // ERROR
            perror("Error en fork");
            exit(1);
        }
    }
}

void creaVertical(int nfilas) { // crear columnas
    for (int i=1; i<nfilas; i++) {
        pid_t pid = fork();
        if (pid > 0) {
            wait(NULL); // Espero a que mis hijos mueran
            break; // PADRE
        }
        else if (pid == -1) { // ERROR
            perror("Error en fork");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    int nfilas, ncolumnas;

    leerArgumentos(argc, argv, &nfilas, &ncolumnas);

    pid_t pidMalla = getpid();
    
    creaHorizontal(ncolumnas); // La malla crea la primera fila (hijos)
    
    if (getppid() == pidMalla) {
        creaVertical(nfilas); // Cada hijo crea su columna
    }

    sleep(10); //  Para poder ver pstree -c

    if (getpid() == pidMalla) { // Super padre espera a la fila 1
        while (wait(NULL) > 0);
    }
}