// Ejercicio 3. hijos.c

#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <stdio.h> // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <signal.h> // signal, kill, SIGUSR1, SIGALRM, alarm
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED
#include <sys/types.h> // pid_t, tipos básicos
#include <sys/ipc.h> // IPC_PRIVATE, IPC_CREAT, IPC_RMID
#include <sys/shm.h> // shmget, shmat, shmdt, shmctl

// USAR WRITE? Vaciado de buffers al hacer kill...

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

void creaHorizontal(int ncolumnas, int nfilas, pid_t hijos[], pid_t *mem) { // crear filas
    
    for (int i=0; i<ncolumnas; i++) {
        pid_t pid = fork();
        if (pid == 0) { // HIJO
            printf("Soy el subhijo %d, mis padres son: ", getpid());
            for (int j=0; j<nfilas; j++) {
                printf("%d ", mem[j]);
            }
            printf("\n");

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

void creaArbol(int nfilas, int ncolumnas, pid_t *mem) { // crear columna
    pid_t pidPadre = getpid();

    for (int i=0; i<nfilas; i++) {
        pid_t pid = fork();

        if (pid > 0) { // PADRE
            wait(NULL);
            if (getpid() != pidPadre) {  // solo los padres intermedios mueren
                exit(0);
            }
            else {
                break;
            }
        }
        else if (pid == 0) { // HIJO
            mem[i] = getpid();

            if (i == nfilas-1) {
                pid_t hijos[ncolumnas];
                creaHorizontal(ncolumnas,nfilas,hijos,mem);

                sleep(5); // Para ver el pstree -c

                matarHijos(ncolumnas, hijos);
                for (int j=0; j< ncolumnas; j++) {
                    mem[nfilas + j] = hijos[j];
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

    int shmid;
    pid_t *mem; // Memoria compartida para guardar PIDs

    if ((shmid = shmget(IPC_PRIVATE,sizeof(pid_t)*(x+y), IPC_CREAT|0666)) == -1) {
        perror("Error al crear memoria compartida");
        exit(1);
    }
    
    // Vinculo el segmento de memoria compartida al proceso
    mem = (pid_t *) shmat(shmid,0,0);
    if (mem == (void *) -1) {
        perror("Error en shmat");
        exit(1);
    }

    creaArbol(x,y,mem);

    wait(NULL); // El super padre solo tiene que esperar al proceso de abajo

    printf("Soy el superpadre %d, mis hijos finales son: ", getpid());

    for (int k=0; k<y; k++) {
        printf("%d ", mem[x+k]);
    }
    printf("\n");

    shmdt(mem);
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        printf("Error al borrar memoria compartida\n");
    }

    exit(0);
}