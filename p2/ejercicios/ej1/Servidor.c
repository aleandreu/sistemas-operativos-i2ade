// Ejercicio 1. Version A) Servidor.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h> // Para open
#include <sys/types.h> // Para pid_t

#include <sys/socket.h> // Para socket -> struct sockaddr
#include <netinet/in.h> // Para bind -> struct in_addr || struct sockaddr_in(campo in_addr)

#define PUERTO 9999

void comprobar_error(int descriptor, const char *mensaje) {
    if (descriptor < 0) {
        perror(mensaje);
        exit(1);
    }
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

int abrir_archivo(const char* nombre) {
    int fd = open(nombre, O_RDONLY);
    comprobar_error(fd, "Error en open");
    return fd;
}

void leer_archivo(int fd_origen, int fd_destino) {
    char buffer[4096];
    ssize_t leidos;

    while ((leidos = read(fd_origen,buffer,sizeof(buffer))) > 0) {
        write(fd_destino, buffer, leidos);
    }
    comprobar_error(leidos, "Error en read");
}

void servir_desde_archivo(int fd_cliente, const char* nombre) {
    int fd_archivo = abrir_archivo(nombre);

    leer_archivo(fd_archivo, fd_cliente);

    close(fd_archivo);
    close(fd_cliente);
}

void atender_cliente(int escucha, int conexion, const char *nombre) {
    pid_t pid = fork();
    comprobar_error(pid, "Error en fork");
    if (pid == 0) { // HIJO
        close(escucha);
        servir_desde_archivo(conexion, nombre);
        exit(0); // matar al hijo
    }
    else { // PADRE
        close(conexion);
    }
}

int main() {
    int escucha, conexion;
    const char *nombre = "Google.html";

    escucha = crear_socket_escucha();

    while (1) {
        conexion = accept(escucha, NULL, NULL); // 4. bloquea hasta que llegue un cliente (accept)
        comprobar_error(conexion, "Error en accept");

        atender_cliente(escucha, conexion, nombre);
    }
}