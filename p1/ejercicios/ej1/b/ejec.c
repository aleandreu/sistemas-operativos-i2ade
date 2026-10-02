// Ejercicio 1. b) ejec.c

#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <stdio.h>  // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <sys/types.h>  // pid_t, tipos para procesos y memoria compartida
#include <signal.h>     // signal, kill, SIGUSR1, SIGALRM, alarm
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED

pid_t pidEjec, pidA, pidB, pidX, pidY, pidZ; // Variables globales para que los manejadores tengan acceso a ellas

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

void manejadorEjec(int sig) { // Inicia la cascada de destrucción
    kill(pidA, SIGUSR2);
}

void manejadorA(int sig) {
    if (fork() == 0) { // Para que haga otro hijo y desde ahí se ejecute el "pstree"
        execlp("pstree","pstree","-c",NULL);
        exit(1);
    }
    wait(NULL); // Para que A espere a que termine pstree

    kill(pidEjec, SIGUSR2); // Avisa a "ejec"
}

void manejadorAB(int sig) {
    kill(pidB, SIGUSR2);
    wait(NULL); // A espera a que B termine
}

void manejadorB(int sig) {
    kill(pidZ, SIGUSR2);
    wait(NULL); // Despierta y recoge a Z

    kill(pidY, SIGUSR2); 
    wait(NULL); // Despierta y recoge a Y

    kill(pidX, SIGUSR2); 
    wait(NULL); // Despierta y recoge a X
}

void manejadorZ(int sig) { // Necesito manejador para que Z no muera antes de enviar la señal
    kill(pidA, SIGUSR1); // ENVÍO DE SEÑAL A "A" CON LA ALARMA
}

void manejador(int sig) {
    // Para que los procesos despierten del pause()
}

int main(int argc, char *argv[]) {

    int tiempo = leerArgumento(argc,argv);

    /* ESTRUCTURA VERTICAL */

    pidEjec = getpid(); // Guardo el PID de Ejec en su variable
    
    pidA = fork(); // Creo el proceso A 
    if (pidA == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidA > 0) { // PID > 0: PADRE
        // ejec
        
        printf("Soy el proceso ejec: mi pid es %d\n", pidEjec);

        signal(SIGUSR2, manejadorEjec);

        wait(NULL);
        printf("Soy ejec(%d) y muero\n", getpid());
        exit(0);
    }
    // A
    pidA = getpid(); // Guardo el PID de A en su variable

    pidB = fork(); // Creo el proceso B
    if (pidB == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidB > 0) {
        // A
        printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", pidA, pidEjec);
        
        signal(SIGUSR1, manejadorA); // CAPTURA DE SEÑAL --> Hace "pstree -c" y Avisa a ejec
        signal(SIGUSR2, manejadorAB); // Avisa a B

        wait(NULL); // Espera a que termine B

        printf("Soy A(%d) y muero\n", getpid());
        exit(0);
    }
    // B
    pidB = getpid(); // Guardo el PID de B en su variable
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", pidB, pidA, pidEjec);

    signal(SIGUSR2, manejadorB); // Espera la orden de A
    
    /* ESTRUCTURA HORIZONTAL */

    pidX = fork(); // Creo el proceso X
    if (pidX == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidX == 0) {
        // X
        printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);
        signal(SIGUSR2, manejador);
        pause();

        printf("Soy X(%d) y muero\n", getpid());
        exit(0);
    }
    // B

    pidY = fork();
    if (pidY == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidY == 0) {
        // Y
        printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);
        signal(SIGUSR2, manejador);
        pause();
        
        printf("Soy Y(%d) y muero\n", getpid());
        exit(0);
    }
    // B

    pidZ = fork();
    if (pidZ == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidZ == 0) {
        // Z
        printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);
        signal(SIGALRM, manejadorZ);
        signal(SIGUSR2, manejador);

        alarm(tiempo);
        pause(); // esperar la alarma
        pause(); // esperar la señal de B para morir
        
        printf("\nSoy Z(%d) y muero\n", getpid());
        exit(0);
    }
    // B

    wait(NULL); // Espero a que se mueran los hijos
    printf("Soy B(%d) y muero\n", getpid());
    exit(0);
}