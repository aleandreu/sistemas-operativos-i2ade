// Ejercicio 1. a) malla.c // REVISAR CASO ./MALLA 1 5

#include <stdio.h>  // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <sys/types.h> // pid_t, tipos para procesos y memoria compartida
#include <signal.h> // signal, kill, SIGUSR1, SIGALRM, alarm
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED

pid_t pidMalla;

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

void despertar(int sig) {}; // Para despertar a los procesos tras el pause();

void manejadorAlarm(int sig) { // Para avisar al padre de que empiece a matar
    kill(pidMalla,SIGUSR1);
}

void matarProcesos(int ncolumnas, pid_t pids[]) {
    for (int i=0; i<ncolumnas; i++) {
        kill(pids[i], SIGUSR1);
    }
    for (int i=0; i<ncolumnas; i++) {
        wait(NULL);
    }
}

void lanzarPstree() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pid == 0) { // Hijo que ejecuta pstree -c
        char buffer[20];
        sprintf(buffer, "%d", pidMalla);
        execlp("pstree","pstree","-c",buffer,NULL);
        perror("Ejecución pstree");
        exit(1);
    }
    pid_t fin;
    do {
        fin = wait(NULL); // wait recoge a CUALQUIER hijo: repito hasta que sea pstree
    } while (fin != pid && fin != -1);
}

void creaVertical(int nfilas, int esUltimaColumna) { // crear columnas
    signal(SIGUSR1, despertar);

    for (int i=1; i<nfilas; i++) {
        pid_t pid = fork();
        if (pid > 0) { // PADRE
            pause();
            kill(pid, SIGUSR1); // Reenvío señal hacia abajo
            wait(NULL); // Espero a que mis hijos mueran
            exit(0); 
        }
        else if (pid == -1) { // ERROR
            perror("Error en fork");
            exit(1);
        }
        signal(SIGUSR1, despertar);
    }

    if (esUltimaColumna) { // Último proceso de toda la malla (uso ultimaColumna como si fuera un "bool")
        signal(SIGALRM, manejadorAlarm);
        alarm(1); // tras 1s envía SIGUSR1 al superpadre
    }

    pause(); // Espero señal del padre vertical
    exit(0);
}

void creaHorizontal(int ncolumnas, int nfilas, pid_t pids[]) { // crear filas
    for (int i=0; i<ncolumnas; i++) {
        pid_t pid = fork();
        if (pid == 0) { // HIJO
            int ultima = 0;
            if (i == ncolumnas-1) {
                ultima = 1;
            }
            creaVertical(nfilas,ultima);
        }
        else if (pid == -1) { // ERROR
            perror("Error en fork");
            exit(1);
        }
        pids[i] = pid;
    }
}

int main(int argc, char *argv[]) {
    int nfilas, ncolumnas;
    leerArgumentos(argc, argv, &nfilas, &ncolumnas);

    pidMalla = getpid();

    signal(SIGUSR1, despertar);
    
    pid_t pids[ncolumnas];
    creaHorizontal(ncolumnas, nfilas, pids); // La malla crea la primera fila (hijos)

    pause(); // super padre esperaS
    lanzarPstree();

    matarProcesos(ncolumnas, pids);
}