// Ejercicio 3. hijos.c

#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <stdio.h> // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <signal.h> // signal, kill, SIGUSR1, SIGALRM, alarm
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED
#include <sys/types.h> // pid_t, tipos básicos
#include <sys/ipc.h> // IPC_PRIVATE, IPC_CREAT, IPC_RMID
#include <sys/shm.h> // shmget, shmat, shmdt, shmctl

pid_t pidPadre; // Variable global del PID del super padre

/* COMPROBACIÓN DE ERRORES */
void comprobarFork(pid_t pid) {
    if (pid == -1) {
        perror("Error en el fork");
        exit(1);
    }
}

void comprobarMemoria(int shmid) {
    if (shmid == -1) {
        perror("Error al crear memoria compartida");
        exit(1);
    }
}

void comprobarShmat(void *memoria) {
    if (memoria == (void *) -1) {
        perror("Error en shmat");
        exit(1);
    }
}

/* FUNCIONES AUXILIARES */
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

void despertar(int sig) {}

void manejadorAlarm(int sig) { // Para avisar al padre de que empiece a matar
    kill(pidPadre,SIGUSR1);
}

void matarHijos(int ncolumnas, int nfilas, pid_t *mem) {
    signal(SIGUSR1, despertar);
    pause(); // espera la orden de su padre

    for (int i=0; i<ncolumnas; i++) {
        kill(mem[nfilas + i], SIGUSR1);
    }
    for (int i=0; i<ncolumnas; i++) {
        wait(NULL);
    }

    exit(0);
}

pid_t *crearMemoria(int x, int y, int *shmid) {
    *shmid = shmget(IPC_PRIVATE,sizeof(pid_t)*(x+y), IPC_CREAT|0666);
    comprobarMemoria(*shmid);

    pid_t *mem = (pid_t *) shmat(*shmid,0,0); // Vinculo el segmento de memoria compartida al proceso
    comprobarShmat(mem);

    return mem;
}

void imprimirLista(pid_t lista[], int n) {
    for (int i=0; i<n; i++) {
        if (i > 0) {
            printf(",");
        }
        printf(" %d", lista[i]);
    }
    printf("\n"); 
}

void subhijo(int i, int ultimo, int nfilas, pid_t *mem) {
    mem[nfilas + i] = getpid();
    printf("Soy el subhijo %d, mis padres son: ", getpid());
    imprimirLista(mem, nfilas);

    if (ultimo) {
        signal(SIGALRM, manejadorAlarm);
        alarm(1); // tras 1 s avisa al superpadre (como en malla)
        pause();
        pause(); // espera la orden de su padre
        exit(0);
    }

    pause(); // espera la orden de su padre
    exit(0);    
}

void creaHorizontal(int ncolumnas, int nfilas, pid_t *mem) {
    for (int i=0; i<ncolumnas; i++) {
        pid_t pid = fork();
        comprobarFork(pid);
        
        if (pid == 0) { // HIJO
            int esUltimo = 0;
            if (i == ncolumnas-1) {
                esUltimo=1;
            }
            subhijo(i, esUltimo, nfilas, mem);
        }

        mem[nfilas + i] = pid; // padre apunta el PID de cada subhijo
    }
}

void creaArbol(int nfilas, int ncolumnas, pid_t *mem) {
    signal(SIGUSR1, despertar);
    mem[0] = getpid();

    for (int i=1; i<nfilas; i++) {
        pid_t pid = fork();
        comprobarFork(pid);
        if (pid > 0) { // PADRE
            pause(); // espera la orden
            kill(pid,SIGUSR1); // se la manda a su hijo
            wait(NULL); // espera a que muera
            exit(0);
        }
        // HIJO
        mem[i] = getpid();
    }
    
    creaHorizontal(ncolumnas,nfilas,mem);
    matarHijos(ncolumnas, nfilas, mem);
}

void liberarMemoria(pid_t *mem, int shmid) {
    shmdt(mem);
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        printf("Error al borrar memoria compartida\n");
    }
    exit(0);
}

pid_t iniciarProcesos(int x, int y, pid_t *mem) {
    pid_t primerProceso = fork(); // super padre crea un hijo
    comprobarFork(primerProceso);
    if (primerProceso == 0) {
        creaArbol(x, y, mem);
    }
    return primerProceso;
}

int main(int argc, char *argv[]) {
    int x, y, shmid;
    pid_t *mem, primerProceso;
    
    leerArgumentos(argc, argv, &x, &y);

    pidPadre = getpid();
    signal(SIGUSR1, despertar);

    // Memoria compartida para guardar PIDs
    mem = crearMemoria(x, y, &shmid);
    primerProceso = iniciarProcesos(x, y, mem);

    pause(); // Espero el aviso del último subhijo

    printf("Soy el superpadre (%d) : mis hijos finales son: ", getpid());
    imprimirLista(&mem[x], y);

    kill(primerProceso, SIGUSR1); // inicio los kills hacia abajo en cascada
    wait(NULL);

    liberarMemoria(mem, shmid);
}