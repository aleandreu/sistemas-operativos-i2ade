// Ejercicio 3. hijos.c

#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <stdio.h> // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <signal.h> // signal, kill, SIGUSR1, SIGALRM, alarm
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

void matarHijos(int nhijos, pid_t hijos[]) {
    for (int i=0; i<nhijos; i++) {
        kill(hijos[i], SIGTERM);
    }
}
void creaHorizontal(int ncolumnas, pid_t hijos[]) { // crear filas
    
    for (int i=0; i<ncolumnas; i++) {
        pid_t pid = fork();
        if (pid == 0) { // HIJO
            printf("Soy el subhijo %d\n", getpid());
            pause(); // espero señal del padre
            exit(0);
        }
        else if (pid > 0) { // PADRE
            hijos[i] = pid;
        }
        else {
            perror("Error en el fork");
            exit(1);
        }
    }
}

void creaArbol(int nfilas, int ncolumnas) { // crear columna
    for (int i=0; i<nfilas; i++) {
        pid_t pid = fork();

        if (pid > 0) { // PADRE
            wait(NULL);
            exit(0);
        }
        else if (pid == 0) { // HIJO
            if (i == nfilas-1) {
                pid_t hijos[ncolumnas];
                creaHorizontal(ncolumnas,hijos);

                sleep(20); // Para ver el pstree -c

                matarHijos(ncolumnas, hijos);
                for (int j=0; j< ncolumnas; j++) {
                    wait(NULL);
                }
                exit(0);
            }
        }
        else { // ERROR
            perror("Error en el fork");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {

    int x, y;

    leerArgumentos(argc, argv, &x, &y);

    printf("x: %d, y: %d\n", x, y);

    creaArbol(x,y);

    


    wait(NULL); // El super padre solo tiene que esperar al proceso de abajo
    exit(0);
}