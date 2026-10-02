// Ejercicio 3. hijos.c

#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <stdio.h> // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED
#include <sys/types.h> // pid_t, tipos básicos
#include <sys/ipc.h> // IPC_PRIVATE, IPC_CREAT, IPC_RMID
#include <sys/shm.h> // shmget, shmat, shmdt, shmctl

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
        if (fork() == 0) { // HIJO
            break;
        }
    }
}

void creaVertical(int nfilas) { // crear columnas
    for (int i=1; i<nfilas; i++) {
        if (fork() > 0) {
            break; // PADRE
        }
    }
}

int main(int argc, char *argv[]) {

    int x, y;

    leerArgumentos(argc, argv, &x, &y);

    printf("x: %d, y: %d\n", x, y);

}