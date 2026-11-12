// Ejercicio 1. Cliente.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h> // Para socket -> struct sockaddr
#include <netinet/in.h> // Para bind -> struct in_addr || struct sockaddr_in(campo in_addr)

#include <arpa/inet.h>   // Para inet_addr (cadena)

#define PUERTO 9999

void comprobar_args(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <IP del servidor>\n", argv[0]);
        exit(1);
    }
}

void comprobar_error(int descriptor, const char *mensaje) {
    if (descriptor < 0) {
        perror(mensaje);
        exit(1);
    }
}

void rellenar_dir_cliente(struct sockaddr_in *dir, const char *ip) {
    dir->sin_family = AF_INET;
    dir->sin_port = htons(PUERTO);
    dir->sin_addr.s_addr = inet_addr(ip); // cadena char --> dir IP
}

int conectar_a_servidor(const char *ip) {
    int sock;
    struct sockaddr_in dir = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0); // 1. crear canal (socket)
    comprobar_error(sock, "Error en socket");

    rellenar_dir_cliente(&dir, ip);

    // 2. establecer la conexión (connect)
    comprobar_error(connect(sock, (struct sockaddr *)&dir, sizeof(dir)), "Error en connect");

    return sock;
}

void recibir_mostrar(int fd) {
    char buffer[4096];
    ssize_t leidos;

    while ((leidos = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, leidos);
    }
    comprobar_error(leidos, "Error en read");
}

int main(int argc, char *argv[]) {
    int fd;

    comprobar_args(argc, argv);

    fd = conectar_a_servidor(argv[1]);
    recibir_mostrar(fd);
    close(fd);

    return 0;
}