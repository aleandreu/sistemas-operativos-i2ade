/* Librerías básicas */
#include <stdlib.h> // exit, NULL
#include <stdio.h> // perror, sprintf
#include <unistd.h> // getpid

/* Nuevas librerías (para Memoria Compartida) */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int shmid;
int *numero=NULL;

int creaComp(void) {
    if((shmid=shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|0666))==-1) {
        perror("Error al crear memoria compartida: ");
        return 1;
    }
    
    /* vinculamos el segmento de memoria compartida al proceso */
    numero=(int *) shmat (shmid,0,0);
    
    /*iniciamos las variables */
    *numero=0;
    
    return 0;
}

void borraComp(void) {
    char error[100];

    if (numero!=NULL) {
        /* desvinculamos del proceso la memoria compartida */
        if (shmdt((char *)numero)<0) {
            sprintf(error,"Pid %d: Error al desligar la memoria compartida:", getpid());
            perror(error);
            exit(3);
        }

        /* borramos la memoria compartida */
        if (shmctl(shmid,IPC_RMID,0)<0) {
            sprintf(error,"Pid %d: Error al borrar memoria compartida:", getpid());
            perror(error);
            exit(4);
        }
    numero=NULL;
    }
}

int main() {

    creaComp();
    printf("Valor inicial: %d\n", *numero);

    *numero = 123; // Escribo en la memoria compartida
    printf("Valor después de escribir: %d\n", *numero);
    
    borraComp();

    return 0;
}