// Ejercicio 1. b) ejec.c

#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <stdio.h>  // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED

int leerArgumento(int nArgs, char *args[]) {
    int tiempo;

    if (nArgs != 2) {
        printf("Error. Debes introducir argumento de tiempo\n");
        exit(1);
    }
    else {
        tiempo = atoi(args[1]);
    }

    return tiempo;
}

int main(int argc, char *argv[]) {

    int tiempo = leerArgumento(argc,argv);
    
    pid_t pidA;
    pidA = fork();
    if (pidA == -1) {
        perror("Error en fork");
        exit(1);
    }
    if (pidA != 0) {
        // arb
        printf("Soy el proceso ejec: mi pid es %d\n", getpid());
        wait(NULL);
        exit(0);
    }

    pid_t pidB;
    pidB = fork();
    if (pidB == -1) {
        perror("Error en fork");
        exit(1);
    }
    if (pidB != 0) {
        // A
        printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", getpid(), getppid());
        wait(NULL);
        exit(0);
    }

    if (pidB == 0) {
        // B
        printf("Soy el proceso B: mi pid es %d. Mi padre es %d\n", getpid(), getppid());
    }
    
    pid_t pidX;
    pidX = fork();
    if (pidX == -1) {
        perror("Error en fork");
        exit(1);
    }
    if (pidX == 0) {
        // X
        printf("Soy el proceso X: mi pid es %d. Mi padre es %d\n", getpid(), getppid());
        exit(0);
    }

    pid_t pidY;
    pidY = fork();
    if (pidY == -1) {
        perror("Error en fork");
        exit(1);
    }
    if (pidY == 0) {
        // Y
        printf("Soy el proceso Y: mi pid es %d. Mi padre es %d\n", getpid(), getppid());
        exit(0);
    }

    pid_t pidZ;
    pidZ = fork();
    if (pidZ == -1) {
        perror("Error en fork");
        exit(1);
    }
    if (pidZ == 0) {
        // Z
        printf("Soy el proceso Z: mi pid es %d. Mi padre es %d\n", getpid(), getppid());
        exit(0);
    }

    // B espera a X, Y, Z
    wait(NULL);
    wait(NULL);
    wait(NULL);


    printf("Final de la ejecución\n");

    return 0;
}