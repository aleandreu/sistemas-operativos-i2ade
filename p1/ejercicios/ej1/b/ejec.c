// Ejercicio 1. b) ejec.c

#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <stdio.h>  // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <sys/types.h>  // pid_t, tipos para procesos y memoria compartida
#include <signal.h>     // signal, kill, SIGUSR1, SIGALRM, alarm
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED
// PREGUNTAR A ALEJANDRO PAUSE() EN X,Y
pid_t pidA; // Variable global para que tenga acceso a ella el manejadorZ

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

void manejadorA(int sig) {
    if (fork() == 0) { // Para que haga otro hijo y desde ahí se ejecute el "pstree"
        execlp("pstree","pstree",NULL);
        exit(1);
    }
}

void manejadorZ(int sig) { // Necesito manejador para que Z no muera antes de enviar la señal
    kill(pidA, SIGUSR1); // ENVÍO DE SEÑAL
}

int main(int argc, char *argv[]) {

    int tiempo = leerArgumento(argc,argv);
    

    /* ESTRUCTURA VERTICAL */

    pid_t pidEjec = getpid(); // Guardo el PID de Ejec en su variable
    pidA = fork(); // Creo el proceso A 
    if (pidA == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidA > 0) { // PID > 0: PADRE
        // ejec
        
        printf("Soy el proceso ejec: mi pid es %d\n", pidEjec);
        
        wait(NULL);

        printf("Soy ejec(%d) y muero\n", getpid());
        exit(0);
    }
    pidA = getpid(); // Guardo el PID de A en su variable

    pid_t pidB; 
    pidB = fork(); // Creo el proceso B
    if (pidB == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidB > 0) {
        // A
        printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", pidA, pidEjec);
        
        signal(SIGUSR1, manejadorA); // CAPTURA DE SEÑAL
        pause(); // Esperar señal de Z

        printf("Soy A(%d) y muero\n", getpid());
        exit(0);
    }
    pidB = getpid(); // Guardo el PID de B en su variable
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", pidB, pidA, pidEjec);
    
    
    /* ESTRUCTURA HORIZONTAL */

    pid_t pidX;
    pidX = fork(); // Creo el proceso X
    if (pidX == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidX == 0) {
        // X
        printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);

        sleep(tiempo + 2);  // Para que muera después de Z e Y

        printf("Soy X(%d) y muero\n", getpid());
        exit(0);
    }

    pid_t pidY;
    pidY = fork();
    if (pidY == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidY == 0) {
        // Y
        printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);
        
        sleep(tiempo + 1);  // Para que muera después de Z
        
        printf("Soy Y(%d) y muero\n", getpid());
        exit(0);
    }

    pid_t pidZ;
    pidZ = fork();
    if (pidZ == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pidZ == 0) {
        // Z

        signal(SIGALRM, manejadorZ);
        alarm(tiempo);
        pause();
        printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);

        printf("\nSoy Z(%d) y muero\n", getpid());
        exit(0);
    }

    // B espera a X, Y, Z
    while(wait(NULL) > 0);

    printf("Soy B(%d) y muero\n", getpid());
    exit(0);

    printf("Final de la ejecución\n");

    return 0;
}