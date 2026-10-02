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
        int tam = 0;
        char buffer[1];
        int leidos;

        if (fd < 0) {
            perror("Error al abrir el archivo");
            exit(1);
        }

        close(tub[0]); // tub[0] = leer la tubería

        while ( read(fd,buffer,1) > 0 ) {
            write(tub[1],buffer,1); // Mientras va leyendo va escribiendo
        }
    }
    else if (pid == -1) {
        perror("Error en el fork");
        exit(1);
    }
    else {
        // HIJO (ve todo lo que hace el padre)
        close(tub[1]); // tub[1] = escribir en la tubería (EL HIJO NO ESCRIBE)

    
        // for (i --> numero)
            // aqui creo el pipe
            // y hago  fork
            // segundo bucle for (.. -> ) -- controlo que se envie cada 100 bytes 


    return 0;
}