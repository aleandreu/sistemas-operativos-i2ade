// Ejercicio 2. hacha.c

#include <stdio.h>  // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <fcntl.h> // open, creat, O_RDONLY, O_WRONLY, O_CREAT, O_APPEND
#include <sys/types.h> // pid_t, tipos básicos

void leerArgumentos(int nArgs, char *argv[], char **nombre, int *tam) {
    if (nArgs != 3) {
        printf("Error. Debes introducir <archivo> + <tamaño>\n");
        exit(1);
    }
    else {
        *nombre = argv[1];
        *tam = atoi(argv[2]);
    }
}
int main(int argc, char *argv[]) {

    char *nombre;
    int tamaño;
    leerArgumentos(argc, argv, &nombre, &tamaño);

    printf("nombre = %s tamaño = %d\n", nombre, tamaño);


    return 0;
}