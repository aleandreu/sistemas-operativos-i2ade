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

    int fd, tub[2];

    pipe(tub); // creo la tubería

    pid_t pid = fork();
    /* Ahora cuando el padre crea el hijo, el hijo verá todo lo que vea el padre 
    (porque la tubería la he creado antes de crear al hijo, si no, este no vería la información) */
    if (pid > 0) {
        // PADRE
        int fd = open(nombre, O_RDONLY);

        if (fd < 0) {
            perror("Error al abrir el archivo");
            exit(1);
        }

        close(tub[0]); // tub[0] = leer la tubería

        char buffer[1];
        int leidos;

        while ( read(fd,buffer,1) > 0 ) {
            write(tub[1],buffer,1); // Mientras va leyendo va escribiendo
        }
    }


    return 0;
}