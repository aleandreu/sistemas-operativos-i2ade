// Ejercicio 1. Servidor.c_extended (parte opcional)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h> // Para pid_t
#include <sys/socket.h> // Para socket -> struct sockaddr
#include <netinet/in.h> // Para bind -> struct in_addr || struct sockaddr_in(campo in_addr)
#include <netdb.h> // Para getaddrinfo || struct addrinfo

#define PUERTO 9999

void comprobar_error(int descriptor, const char *mensaje) {
    if (descriptor < 0) {
        perror(mensaje);
        exit(1);
    }
}

/* Funciones para la parte opcional */
int conectar_google() {
    struct addrinfo hints = {0}, *res;
    int sock;

    hints.ai_family=AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo("www.google.com","80", &hints, &res) != 0) {
        printf("Error en getaddrinfo\n");
        exit(1);
    }

    sock = socket(res->ai_family,res->ai_socktype,res->ai_protocol); // 1. crear canal (socket)
    comprobar_error(sock, "Error en socket");

    // 2. establecer la conexión (connect)
    comprobar_error(connect(sock,res->ai_addr,res->ai_addrlen), "Error en connect");

    return sock;
}

void enviar_peticion(int fd) {
    const char *header = "GET / HTTP/1.0\r\nHost: www.google.com\r\n\r\n";
    send(fd,header,strlen(header),0);
}

void leer_archivo(int fd_origen, int fd_destino) {
    char buffer[4096];
    ssize_t leidos;

    while ((leidos = read(fd_origen,buffer,sizeof(buffer))) > 0) {
        write(fd_destino, buffer, leidos);
    }
    comprobar_error(leidos, "Error en read");
}

void servir_desde_google(int fd_cliente) {
    int fd_google = conectar_google();

    enviar_peticion(fd_google);

    leer_archivo(fd_google, fd_cliente);

    close(fd_google);
    close(fd_cliente);
}

void rellenar_dir_servidor(struct sockaddr_in *dir) {
    dir->sin_family = AF_INET;
    dir->sin_port = htons(PUERTO);
    dir->sin_addr.s_addr = htonl(INADDR_ANY);
}

int crear_socket_escucha() {
    int sock;
    struct sockaddr_in dir = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0); // 1. crear canal (socket): IPv4 (AF_INET), TCP (SOCK_STREAM)
    comprobar_error(sock, "Error en socket");

    rellenar_dir_servidor(&dir);

    // 2. publicar la direccion (bind)
    comprobar_error(bind(sock,(struct sockaddr *)&dir, sizeof(dir)), "Error en bind");

    // 3. ponerse a escuchar (listen)
    comprobar_error(listen(sock,5), "Error en listen");

    return sock;
}

void atender_cliente(int escucha, int conexion) {
    pid_t pid = fork();
    comprobar_error(pid, "Error en fork");
    if (pid == 0) { // HIJO
        close(escucha);
        servir_desde_google(conexion);
        exit(0); // matar al hijo
    }
    else { // PADRE
        close(conexion);
    }
}

int main() {
    int escucha, conexion;

    escucha = crear_socket_escucha();

    while (1) {
        conexion = accept(escucha, NULL, NULL); // 4. bloquea hasta que llegue un cliente (accept)
        comprobar_error(conexion, "Error en accept");

        atender_cliente(escucha, conexion);
    }
}