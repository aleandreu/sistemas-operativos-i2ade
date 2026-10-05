// Ejercicio 2. hacha.c

#include <stdio.h>  // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <fcntl.h> // open, creat, O_RDONLY, O_WRONLY, O_CREAT, O_APPEND
#include <sys/types.h> // pid_t, tipos básicos
#include <sys/stat.h> // stat
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED

void leerArgumentos(int nArgs, char *argv[], char **nombre, int *tam) {
    if (nArgs != 3) {
        printf("Error. Debes introducir <archivo> + <tamaño>\n");
        exit(1);
    }
    if (atoi(argv[2]) <= 0) {
        printf("Error. El tamaño del fragmento debe ser > 0\n");
        exit(1);
    }
    *nombre = argv[1];
    *tam = atoi(argv[2]);
}

int main(int argc, char *argv[]) {

    char *nombre;
    int tamañoFragmento;
    leerArgumentos(argc, argv, &nombre, &tamañoFragmento);

    printf("nombre = %s tamaño = %d\n", nombre, tamañoFragmento);

    pid_t pid = fork();
    if (pid == -1) {
        perror("Error en el fork padre");
        exit(1);
    }
    else if (pid == 0) { // HIJO
        exit(0); // No hago nada
    }
    // PADRE INICAL

    struct stat infoArchivo;
    stat(nombre, &infoArchivo); // Stat (datos) del archivo origen (para saber qué tamaño tiene el archivo)

    long tamArchivo = infoArchivo.st_size; // tamaño del archivo origen

    // Calculo el nº de hijos = nº de archivos destino
    int numHijos = tamArchivo / tamañoFragmento;
    if (tamArchivo % tamañoFragmento != 0) {
        numHijos++; // último hijo con el sobrante
    }

    /* Ahora cuando el padre crea el hijo, el hijo verá todo lo que vea el padre 
    (porque la tubería la he creado antes de crear al hijo, si no, este no vería la información) */

    int fdOrigen = open(nombre, O_RDONLY);
    if (fdOrigen < 0) {
        perror("Error al abrir el archivo");
        exit(1);
    }

    for (int i=0; i<numHijos; i++) {
        int tub[2];
        pipe(tub); // creo la tubería
            // tub[0] = LEER la tubería
            // tub[1] = ESCRIBIR en la tubería

        pid_t pid = fork(); // creo a cada hijo
        if (pid == -1) {
            perror("Error en fork hijo");
            exit(1);
        }
        else if (pid > 0) { // PADRE QUE CREA HIJOS
            close(tub[0]); // cierro la parte de lectura de la tubería (PADRE NO LEE)

            long bytesRestantes = tamArchivo - i * tamañoFragmento;

            int bytesEnviar;
            if (bytesRestantes >= tamañoFragmento) {
                bytesEnviar = tamañoFragmento; // mando al pipe un fragmento
            }
            else {
                bytesEnviar = bytesRestantes; // mando al pipe el restante (que es menor al tamaño de los fragmentos)
            }

            char buffer[1]; // envío bytes uno a uno

            for (int i=0; i<bytesEnviar; i++) {
                read(fdOrigen,buffer,1); // leo lo que tengo que enviar
                write(tub[1],buffer,1); // escribo en la tubería lo que acabo de leer
            }

            close(tub[1]);
        }
        else { // HIJOS
            close(tub[1]); // cierro la parte de escritura de la tubería (HIJO NO ESCRIBE)

            char buffer[1];
            int leidos;

            char nombreDestino[200];
            sprintf(nombreDestino,"%s.h%02d",nombre,i); // %s: inserta una cadena || %02d: inserta un entero con dos dígitos

            int fdDestino = creat(nombreDestino,0666); // 0666: solo lectura-escritura pero no ejecutable

            if (fdDestino < 0) {
                perror("Error al crear el archivo destino");
            }

            
            while (read(tub[0],buffer,1) > 0) { // hijo recibe los bytes del padre (el FRAGMENTO enviado en el pipe)
                write(fdDestino,buffer,1); // Mientras va leyendo va escribiendo en el fichero destino
            }

            close(fdDestino);
            close(tub[0]);
            exit(0); // pasamos al siguiente hijo (fragmento del archivo)
        }   
    }

    while (wait(NULL) > 0); // Para que los hijos mueran antes que el padre
}