#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int df,tam;
    long numero;
    char buffer[10];
    df=open("/dev/tty", O_RDONLY);
    printf("df creado: %d\n",df); // por mera curiosdidad... 
    
    if (df<0) {
        perror("Error al abrir tty");
        exit(-1);
    }
    tam=read(df, buffer, 9); /* COMO MUCHO DE HASTA 9 DIGITOS */
    
    if (tam == -1)
        perror("Error de lectura");
    else {
        buffer[tam]=0; /* PONE EL FINAL DE CADENA */
        numero=atoi(buffer);
        printf("Resultado: %ld\n",numero*2);
    }
    close(df);
}