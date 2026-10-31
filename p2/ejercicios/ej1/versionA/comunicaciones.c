// Ejercicio 1. Version A) comunicaciones.c

#include <sys/types.h>
#include <sys/socket.h> // Para socket -> struct sockaddr
#include <netinet/in.h> // Para bind -> struct in_addr || struct sockaddr_in(campo in_addr)

/* 
Necesario para la practica 
    - Llamadas: socket, bind, listen, accept, connect
    - Funciones: 
        - inet_addr(cadena): char --> dirIP
        - inet_ntoa(in_addr): dirIP --> char
        - htonl, htons, ntohl, ntohs: unsigned long/short (host) <--> red
*/