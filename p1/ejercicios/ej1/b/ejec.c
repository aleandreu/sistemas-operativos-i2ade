// Ejercicio 1. b) ejec.c

#include <unistd.h> // fork, getpid, getppid, exec*, read, write, close, pause, sleep
#include <stdio.h> // printf, perror
#include <stdlib.h> // exit, atoi, malloc, free
#include <sys/types.h> // pid_t, tipos para procesos y memoria compartida
#include <signal.h> // signal, kill, SIGUSR1, SIGALRM, alarm
#include <sys/wait.h> // wait, waitpid, macros WEXITSTATUS, WIFEXITED

pid_t pidEjec, pidA, pidB, pidX, pidY, pidZ; // Variables globales para que los manejadores tengan acceso a ellas

void comprobarFork(pid_t pid, const char *proceso) {
    if (pid == -1) {
        perror(proceso);
        exit(1);
    }
}

/* REUTILIZADO DE MALLA.C */
int leerArgumento(int nArgs, char *args[]) {
    int tiempo;

    if (nArgs != 2 || atoi(args[1]) <= 0) {
        printf("Error. Debes introducir argumento de tiempo > 0\n");
        exit(1);
    }
    else {
        tiempo = atoi(args[1]);
    }

    return tiempo;
}

void despertar(int sig) {} // Despertar a los procesos

void lanzarPstree() { // Ejecutar el pstree -c
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error en fork");
        exit(1);
    }
    else if (pid == 0) { // Hijo que ejecuta pstree -c
        char buffer[20];
        sprintf(buffer, "%d", pidEjec);
        execlp("pstree","pstree","-c",buffer,NULL);
        perror("Ejecución pstree");
        exit(1);
    }
    pid_t fin; // Para que A espere a que termine pstree
    do {
        fin = wait(NULL); // wait recoge a CUALQUIER hijo: repito hasta que sea pstree
    } while (fin != pid && fin != -1);
}

/* HANDLERS */
void manejadorEjec(int sig) {
    // Inicia la cascada de destrucción
    kill(pidA, SIGUSR2);
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

void manejadorZ(int sig) { 
    // Necesito manejador para que Z no muera antes de enviar la señal
    kill(pidA, SIGUSR1); // ENVÍO DE SEÑAL A "A" CON LA ALARMA
}

/* PROCESOS */
void procesoEjec() {
    printf("Soy el proceso ejec: mi pid es %d\n", pidEjec);

    signal(SIGUSR2, manejadorEjec);

    wait(NULL);
    printf("Soy ejec(%d) y muero\n", getpid());
    exit(0);
}

void procesoA() {
    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", pidA, pidEjec);
    
    signal(SIGUSR1, despertar); // CAPTURA DE SEÑAL --> Hace "pstree -c" y Avisa a ejec
    pause();
    
    lanzarPstree();

    signal(SIGUSR2, manejadorAB); // Avisa a B
    kill(pidEjec, SIGUSR2); // Avisa a "ejec"
    
    wait(NULL); // Espera a que termine B
    printf("Soy A(%d) y muero\n", getpid());
    exit(0);
}

void procesoX() {
    printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);
    signal(SIGUSR2, despertar);
    pause();

    printf("Soy X(%d) y muero\n", getpid());
    exit(0);
}

void procesoY() {
    printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);
    signal(SIGUSR2, despertar);
    pause();

    printf("Soy Y(%d) y muero\n", getpid());
    exit(0);
}

void procesoZ(int tiempo) {
    printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), pidB, pidA, pidEjec);

    signal(SIGALRM, manejadorZ);
    signal(SIGUSR2, despertar);

    alarm(tiempo);
    pause(); // esperar la alarma
    pause(); // esperar la señal de B para morir
    
    printf("\nSoy Z(%d) y muero\n", getpid());
    exit(0);
}

void procesoB(int tiempo) {
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", pidB, pidA, pidEjec);

    signal(SIGUSR2, manejadorB); // Espera la orden de A
    
    // X
    pidX = fork(); // Creo el proceso X
    comprobarFork(pidX, "Error en fork de X");
    if (pidX == 0) {
        procesoX();
    }
    
    // Y
    pidY = fork(); // Creo el proceso Y
    comprobarFork(pidY, "Error en fork de Y");
    if (pidY == 0) {
        procesoY();
    }
    
    // Z
    pidZ = fork(); // Creo el proceso Z
    comprobarFork(pidZ, "Error en fork de Z");
    if (pidZ == 0) {
        procesoZ(tiempo);
    }
    
    wait(NULL); // B espera a que se mueran los hijos
    printf("Soy B(%d) y muero\n", getpid());
    exit(0);
}

/* MAIN */
int main(int argc, char *argv[]) {

    int tiempo = leerArgumento(argc,argv);

    /* ESTRUCTURA VERTICAL: ejec --> A --> B */
    
    // ejec
    pidEjec = getpid(); // Guardo el PID de Ejec
    pidA = fork(); // Creo el proceso A
    comprobarFork(pidA, "Error en fork de A");
    if (pidA > 0) {
        // ejec
        procesoEjec();
    }
    
    // A
    pidA = getpid(); // Guardo el PID de A
    pidB = fork(); // Creo el proceso B
    comprobarFork(pidB, "Error en fork de B");
    if (pidB > 0) {
        // A
        procesoA();
    }
    
    /* ESTRUCTURA HORIZONTAL: B --> {X, Y, Z} */

    // B
    pidB = getpid(); // Guardo el PID de B
    procesoB(tiempo);
}