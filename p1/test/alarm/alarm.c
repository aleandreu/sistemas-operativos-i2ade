#include <signal.h>
// Para evitar warnings:
#include <stdio.h>
#include <unistd.h>

int seguir = 1; /* Variable global */

void fin(int n) {
    seguir = 0;
}

int main() {
    int contador = 0;
    signal(SIGALRM, fin);
    alarm(5);
    
    do {
        printf("Esta es la línea %d\n", contador++);
    } while (seguir);
        printf("TOTAL: %d líneas\n", contador);
}