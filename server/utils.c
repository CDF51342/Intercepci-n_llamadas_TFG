#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include "mensaje.h"
#define MAX 256

pthread_mutex_t mutex_write;

void init_mutex() { // Inicializa el mutex
    pthread_mutex_init(&mutex_write, NULL);
}

void lock_mutex() {     // Bloquea el mutex
    pthread_mutex_lock(&mutex_write);
}

void unlock_mutex() {   // Desbloquea el mutex
    pthread_mutex_unlock(&mutex_write);
}

void destroy_mutex() {  // Destruye el mutex
    pthread_mutex_destroy(&mutex_write);
}

void obtain_actualtime(char *timestamp_str) {
    // Se obtiene el tiempo actual
    struct timeval tv;
    struct tm *time_info;
    gettimeofday(&tv, NULL);
    time_info = localtime(&tv.tv_sec);
    // Se establece en función al formato mes día, año @ hh:mm:ss.ms
    strftime(timestamp_str, 25, "%b %d, %Y @ %H:%M:%S", time_info);
    sprintf(timestamp_str + strlen(timestamp_str), ".%03ld", tv.tv_usec / 1000);
}

void obtain_filename(char *path, char **filename, char **extension) {
    // Nombre del archivo
    char *splitted = strtok(path, "/");
    while (splitted != NULL) {
        *filename = splitted;
        splitted = strtok(NULL, "/");
    }

    // Tipo del archivo
    splitted = strtok(*filename, ".");
    while (splitted != NULL) {
        *extension = splitted;
        splitted = strtok(NULL, ".");
    }
}

void format_bytes_inside(char *string) {
    // Se establece memoria
    char *formatted_string = (char *) malloc((strlen(string) * 2 + 1) * sizeof(char));
    int len = 0;
    // Se realiza un bucle para leer cada carácter de la cadena
    for (int i = 0; i < strlen(string); i++) {
        // En caso de encontrarse con un salto de línea se formatea
        if (string[i + 1] == '\n') {
            formatted_string[len++] = '\\';
            formatted_string[len++] = 'n';
            i++;
        }
        else if (string[i + 1] == '{') {
            formatted_string[len++] = string[i];
            formatted_string[len++] = '\\';
            formatted_string[len++] = '{';
            i++;
        }
        else if (string[i + 1] == '}') {
            formatted_string[len++] = string[i];
            formatted_string[len++] = '\\';
            formatted_string[len++] = '}';
            i++;
        }
        else
            formatted_string[len++] = string[i];
    }
    formatted_string[len] = '\0';
    strcpy(string, formatted_string);
    free(formatted_string);
}

int get_path_logstash(char **path) {
    // A través de la variable de entorno PATH_LOGSTASH se consigue la ruta al fichero log
    *path = getenv("PATH_LOGSTASH");     //"../ELK Stack/logstash-8.11.3/prueba_log.log"
    if (*path == NULL){
        perror("ERROR: la variable de entorno PATH_LOGSTASH no está inicializada.\n");
        return -1;
    }
    return 0;
}

void insert_datalog(char *jsonline) {
    // Se bloquea mutex para la escritura del fichero log
    lock_mutex();
    FILE *f;
    // Se obtiene la ruta al fichero log
    char *path;
    if (get_path_logstash(&path) == -1)
        exit(0);

    // Se abre el fichero
    f = fopen(path, "a");
    if (f == NULL)
        exit(0);
    // Se escribe en el fichero el jsonline dado
    fwrite(jsonline, 1, strlen(jsonline), f);
    fclose(f);
    // Se desbloquea el mutex
    unlock_mutex();
}
