#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "mensaje.h"
#include "funciones.h"
#include "utils.h"

pthread_mutex_t mutex_pet;
pthread_cond_t cond_pet;
int recibido = 0;

void tratar_peticion(petition *petition){
    // Se coge la petición
    struct petition pet = *petition;
    // Respuesta a la petición
    int res=0;
    char res_string[256];
    // Se ve la operación mandada
    values values;
    if (readLine(pet.socket, values.cod_op, 2) == -1){
        perror("ERROR: No se pudo recibir correctamente el código de operación\n");
        close(pet.socket);
        pthread_exit(0);    // Se cierra el hilo
    }
    int operation;
    sscanf(values.cod_op, "%d", &operation);
    // Se copia la ip del cliente
    strcpy(values.ip, pet.ip);
    
    pthread_mutex_lock(&mutex_pet);
    recibido = 1;   // Se ha copiado ya la petición
	// Desbloqueamos el mutex y la condición
    pthread_cond_signal(&cond_pet);
	pthread_mutex_unlock(&mutex_pet);
    
    // Vemos la operación pedida
    switch(operation){
        default:    // Si pide una función inexistente
            res = -1;
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;
        
        case 0: // OPEN
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta, el proceso, las flags y el mode
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.flags, 11) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.mode, 12) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_open(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;
        
        case 1: // CLOSE
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el código de operación\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_close(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;

        case 2: // READ
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta, el buffer leído y el count
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.buffer, 512) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.count, 11) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_read(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;

        case 3: // WRITE
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta, el buffer escrito y el count
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.buffer, 512) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.count, 11) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_write(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;

        case 4: // LSEEK
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta, el offset y el whence
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.offset, 11) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.whence, 11) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_lseek(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;

        case 5: // UNLINK
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_unlink(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;

        case 6: // REMOVE
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_remove(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;
        
        case 7: // RENAME
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta y la nueva ruta
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.newpath, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_rename(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;

        case 8: // MKDIR
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta, el proceso y el mode
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.mode, 12) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_mkdir(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;

        case 9: // RMDIR
            // Se recoge el tiempo en el que la solicitó el cliente
            if (readLine(pet.socket, values.clnt_time, 51) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            // Se recopilan los datos de la ruta, el proceso y el mode
            if (readLine(pet.socket, values.path, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            if (readLine(pet.socket, values.process, 256) == -1){
                perror("ERROR: No se pudo recibir correctamente el valor\n");
                close(pet.socket);
                pthread_exit(0);    // Se cierra el hilo
            }
            res = server_rmdir(values);
            // Se envia la respuesta
            sprintf(res_string, "%d", res);
            if (sendMessage(pet.socket, res_string, strlen(res_string) + 1) == -1){
                perror("ERROR: No se pudo enviar la respuesta\n");
            }
            break;
    }
    close(pet.socket);  // Se cierra el socket
    pthread_exit(0);    // Se cierra el hilo
}

int main(int argc, char **argv){
    // Miramos que se hayan pasado los argumentos
    if (argc != 3){
        perror("El servidor se ejecuta de manera: ./server -p <port>\n");
        return -1;
    }
    
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    struct ifreq ifr;
    petition pet;
    int sd, val, err;

    // Creamos el socket
    sd =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        perror("ERROR: No se pudo inicializar el socket.\n");
        exit(1);
    }

    val = 1;
    err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
    if (err < 0) {
        perror("ERROR: No se pudo poner bien los parámetros al socket.\n");
        exit(1);
    }

    // Indicamos la IP
    ifr.ifr_addr.sa_family = AF_INET;
    strcpy(ifr.ifr_name, "eth0");

    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
        return -1;

    // Creamos la dirección del servidor creado
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // Damos el valor del puerto dado
    int port;
    if (sscanf(argv[2], "%d", &port) != 1){   // En caso de no ser un número
        printf("ERROR: Como segundo argumento debe de introducir un número factible de puerto.\n");
        return -1;
    }
    server_addr.sin_port = htons(port);

    if (bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("ERROR: No se pudo asignar correctamente la dirección al socket.\n");
		return -1;
    }
    if (listen(sd, SOMAXCONN) == -1){
        perror("ERROR: No se pudo habilitar al socket para recibir conexiones.\n");
		return -1;
    }
    size = sizeof(client_addr);

    // Se indica que se ha conectado con su IP y su puerto
    printf("Operating in >>> %s:%d\n", inet_ntoa((*(struct in_addr *) &ifr.ifr_addr.sa_data[2])), port);
    
    // Inicializamos el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_mutex_init(&mutex_pet, NULL);
	pthread_cond_init(&cond_pet, NULL);
    init_mutex();
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    
    while(1) {
        // PARTE CONCURRENTE (HILOS) DE TRAMITACIÓN DE SOLICITUDES
        pet.socket = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
        if (pet.socket == -1) {
            perror("ERROR: No se pudo aceptar la conexión con cliente\n");
            continue;
		}

        // Vemos la IP del cliente
        struct in_addr clientIp = client_addr.sin_addr;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientIp, ip, INET_ADDRSTRLEN);
        strcpy(pet.ip, ip);

        if(pthread_create(&thread_pet, &attr_thread, (void *)tratar_peticion, &pet) == 0){
            //Bloqueamos el hilo para que puedan acceder correctamente a las operaciones
            pthread_mutex_lock(&mutex_pet);
            while (recibido == 0){
                pthread_cond_wait(&cond_pet, &mutex_pet);
            }
            recibido = 0;
            pthread_mutex_unlock(&mutex_pet);
            pthread_join(thread_pet, NULL);
        }
    }
    close(sd);
}
