#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int sendMessage(int socket, char *buffer, int len) {
    int r;
    int l = len;
    do {
        r = send(socket, buffer, l, 0);  // Use send instead of write
        if (r < 0) {
            // Handle error
            perror("send");
            return -1;
        }
        l = l - r;
        buffer = buffer + r;
    } while ((l > 0) && (r >= 0));

    if (r < 0)
        return -1;  /* fail */
    else
        return 0;    /* full length has been sent */
}

ssize_t readLine(int fd, char *buffer, size_t n) {
	int totalReceived = 0;
    int received;

    while (totalReceived < n) {
        received = recv(fd, buffer + totalReceived, n - totalReceived, 0);
        if (received == -1 || received == 0) {
            // Manejar error de recepción o conexión cerrada aquí si es necesario
            return -1;
        }
        totalReceived += received;
    }

    return totalReceived; // Retornar la cantidad total de bytes recibidos
}

int set_socket_and_connect(int *sc) {
    struct sockaddr_in server_addr;
    struct hostent *hp;
    // Se crea el socket
    *sc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*sc < 0){
        perror("ERROR: SOCKET\n");
        return -1;
    }
    // Se obtiene la dirección 
    bzero((char *)&server_addr, sizeof(server_addr));
    // A través de la variable de entorno IP_SERVER se consigue la IP
    char *ip;
    ip = getenv("IP_SERVER");
    if (ip == NULL){  // Si no existe, no se puede conectar al servidor
        perror("ERROR: la variable de entorno IP_SERVER no está inicializada.\n");
        return -1;
    }
    // Se intenta conectar a dicho puerto
    hp = gethostbyname(ip);
    if (hp == NULL) {
        perror("ERROR: No se pudo obtener la IP de IP_SERVER.\n");
        return -1;
    }
    memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
    server_addr.sin_family = AF_INET;
    
    // A través de la variable de entorno PORT_SERVER se consigue el puerto
    char *port;
    port = getenv("PORT_SERVER");
    if (port == NULL){    // Si no existe, no se puede conectar al servidor
        perror("ERROR: la variable de entorno PORT_SERVER no está inicializada.\n");
        return -1;
    }
    int number_port;
    if (sscanf(port, "%d", &number_port) != 1){    // En caso de no ser un número, no se puede conectar al servidor
        perror("ERROR: la variable de entorno PORT_SERVER un número factible de puerto.\n");
        return -1;
    }
    server_addr.sin_port = htons(number_port);
    // Vemos si se puede conectar con el servidor
    if (connect(*sc, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1){
        perror("ERROR: No se pudo conectar con el servidor.\n");
        return -1;
    }
    return 0;
}
