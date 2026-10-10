// Ejercicio 2. hacha.c

#include <stdio.h> // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <fcntl.h> // open, creat, O_RDONLY, O_WRONLY, O_CREAT, O_APPEND
#include <sys/types.h> // pid_t, tipos básicos
#include <sys/stat.h> // stat
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED

/* FUNCIONES AUXILIARES */
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

int calculoNhijos(long tamArch, int tamFrag) {
    int numHijos = tamArch / tamFrag;
    if (tamArch % tamFrag != 0) {
        numHijos++; // último hijo con el sobrante
    }
    return numHijos;
}

long calculoBytesEnviar(long tamArch, int tamFrag, int i) {
    long bytesRestantes = tamArch - i*tamFrag;
    long bytesEnviar;
    if (bytesRestantes >= tamFrag) {
        bytesEnviar = tamFrag; // mando al pipe un fragmento
    }
    else {
        bytesEnviar = bytesRestantes; // mando al pipe el restante (que es menor al tamaño de los fragmentos)
    }
    return bytesEnviar;
}

void esperarHijos(int num) {
    for (int i=0; i<num; i++) {
        wait(NULL);
    }
}

/* COMPROBACIÓN DE ERRORES */
void comprobarFork(pid_t pid, const char *proceso) {
    if (pid == -1) {
        perror(proceso);
        exit(1);
    }
}

void comprobarArchivo(int descriptor, const char *mensaje) {
    if (descriptor == -1) {
        perror(mensaje);
        exit(1);
    }
}

void comprobarStat(int resultado) {
    if (resultado == -1) {
        perror("Error en stat");
        exit(1);
    }
}

void comprobarPipe(int resultado) {
    if (resultado == -1) {
        perror("Error en pipe");
        exit(1);
    }
}

/* ARCHIVOS */
long TamArchivoOrigen(const char *nombre) {
    struct stat infoArchivo; // Stat (datos) del archivo origen (para saber qué tamaño tiene el archivo)
    comprobarStat(stat(nombre, &infoArchivo)); // Compruebo que no tenga errores
    return infoArchivo.st_size; // tamaño del archivo origen
}

int abrirOrigen(const char *nombre) {
    int fdOrigen = open(nombre, O_RDONLY);
    comprobarArchivo(fdOrigen, "Error al abrir el archivo origen");
    return fdOrigen;
}

void enviarFragmento(int fdOrigen, int tuberia, long bytesEnviar) {
    char buffer[1]; // envío bytes uno a uno

    for (long i=0; i<bytesEnviar; i++) {
        read(fdOrigen,buffer,1); // leo lo que tengo que enviar
        write(tuberia,buffer,1); // escribo en la tubería lo que acabo de leer
    }

    close(tuberia);
}

void crearArchivosDestino(int tuberia, const char *nombre, int i) {
    char buffer[1];
    
    char nombreDestino[200];
    sprintf(nombreDestino,"%s.h%02d",nombre,i); // %s: inserta una cadena || %02d: inserta un entero con dos dígitos

    int fdDestino = creat(nombreDestino,0666); // 0666: solo lectura-escritura pero no ejecutable
    comprobarArchivo(fdDestino, "Error al crear el archivo destino");

    while (read(tuberia,buffer,1) > 0) { // hijo recibe los bytes del padre (el FRAGMENTO enviado en el pipe)
        write(fdDestino,buffer,1); // Mientras va leyendo va escribiendo en el fichero destino
    }

    close(fdDestino);
    close(tuberia);
    exit(0); // pasamos al siguiente hijo (fragmento del archivo)
}

void dividirArchivo(int fdOrigen, const char *nombre, long tamArchivo, int tamFragmento, int numHijos) {
    for (int i=0; i<numHijos; i++) {
        int tub[2];
        // creo la tubería
        comprobarPipe(pipe(tub)); // Compruebo que no tenga errores
            // tub[0] = LEER la tubería
            // tub[1] = ESCRIBIR en la tubería

        pid_t pid = fork(); // creo a cada hijo
        comprobarFork(pid, "Error en fork");
        if (pid > 0) { // PADRE QUE CREA HIJOS
            close(tub[0]); // cierro la parte de lectura de la tubería (PADRE NO LEE)

            long bytesEnviar = calculoBytesEnviar(tamArchivo, tamFragmento, i);

            enviarFragmento(fdOrigen,tub[1],bytesEnviar);
        }
        else { // HIJOS
            close(tub[1]); // cierro la parte de escritura de la tubería (HIJO NO ESCRIBE)
            close(fdOrigen);  // El hijo no utiliza el archivo original
            
            crearArchivosDestino(tub[0],nombre,i);
        }   
    }
}

int main(int argc, char *argv[]) {

    char *nombre;
    int tamFragmento;

    leerArgumentos(argc, argv, &nombre, &tamFragmento);

    long tamArchivo = TamArchivoOrigen(nombre);

    int fdOrigen = abrirOrigen(nombre);

    int numHijos = calculoNhijos(tamArchivo, tamFragmento); // Calculo el nº de hijos = nº de archivos destino
    
    dividirArchivo(fdOrigen, nombre, tamArchivo, tamFragmento, numHijos);

    close(fdOrigen);
    
    esperarHijos(numHijos);
}