#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include "funciones.h"
#include "mensaje.h"
#include "utils.h"

int server_open(values values) {
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Nombre y Tipo del archivo
    char *filename, *extension;
    obtain_filename(values.path, &filename, &extension);
    printf("Nombre del archivo: %s\n", filename);
    printf("Extensión del archivo: %s\n", extension);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"open\", \"type_op\": \"file\", \"path\": \"%s\", \"filename\": \"%s\", \"extension\": \"%s\", \"flags\": \"%s\", \"mode\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, filename, extension, values.flags, values.mode);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_close(values values) {
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Nombre y Tipo del archivo
    char *filename, *extension;
    obtain_filename(values.path, &filename, &extension);
    printf("Nombre del archivo: %s\n", filename);
    printf("Extensión del archivo: %s\n", extension);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"close\", \"type_op\": \"file\", \"path\": \"%s\", \"filename\": \"%s\", \"extension\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, filename, extension);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_read(values values){
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Nombre y Tipo del archivo
    char *filename, *extension;
    obtain_filename(values.path, &filename, &extension);
    printf("Nombre del archivo: %s\n", filename);
    printf("Extensión del archivo: %s\n", extension);

    // Se formatea los bytes leídos
    format_bytes_inside(values.buffer);
    printf("Formateado bytes: %s\n", values.buffer);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"read\", \"type_op\": \"file\", \"path\": \"%s\", \"filename\": \"%s\", \"extension\": \"%s\", \"bytes_read\": \"%s\", \"count_read\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, filename, extension, values.buffer, values.count);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_write(values values){
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Nombre y Tipo del archivo
    char *filename, *extension;
    obtain_filename(values.path, &filename, &extension);
    printf("Nombre del archivo: %s\n", filename);
    printf("Extensión del archivo: %s\n", extension);

    // Se formatea los bytes leídos
    format_bytes_inside(values.buffer);
    printf("Formateado bytes: %s\n", values.buffer);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"write\", \"type_op\": \"file\", \"path\": \"%s\", \"filename\": \"%s\", \"extension\": \"%s\", \"bytes_write\": \"%s\", \"count_write\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, filename, extension, values.buffer, values.count);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_lseek(values values) {
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Nombre y Tipo del archivo
    char *filename, *extension;
    obtain_filename(values.path, &filename, &extension);
    printf("Nombre del archivo: %s\n", filename);
    printf("Extensión del archivo: %s\n", extension);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"lseek\", \"type_op\": \"file\", \"path\": \"%s\", \"filename\": \"%s\", \"extension\": \"%s\", \"offset\": \"%s\", \"whence\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, filename, extension, values.offset, values.whence);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_unlink(values values) {
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Nombre y Tipo del archivo
    char *filename, *extension;
    obtain_filename(values.path, &filename, &extension);
    printf("Nombre del archivo: %s\n", filename);
    printf("Extensión del archivo: %s\n", extension);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"unlink\", \"type_op\": \"dir\", \"path\": \"%s\", \"filename\": \"%s\", \"extension\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, filename, extension);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_remove(values values) {
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Nombre y Tipo del archivo
    char *filename, *extension;
    obtain_filename(values.path, &filename, &extension);
    printf("Nombre del archivo: %s\n", filename);
    printf("Extensión del archivo: %s\n", extension);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"remove\", \"type_op\": \"dir\", \"path\": \"%s\", \"filename\": \"%s\", \"extension\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, filename, extension);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_rename(values values) {
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Nombre y Tipo del archivo
    char *filename, *extension;
    obtain_filename(values.path, &filename, &extension);
    printf("Nombre del archivo: %s\n", filename);
    printf("Extensión del archivo: %s\n", extension);

    // Nombre y Tipo de la nueva ruta del archivo
    char *newfilename, *newextension;
    obtain_filename(values.newpath, &newfilename, &newextension);
    printf("Nuevo nombre del archivo: %s\n", newfilename);
    printf("Nueva extensión del archivo: %s\n", newextension);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"rename\", \"type_op\": \"dir\", \"path\": \"%s\", \"filename\": \"%s\", \"extension\": \"%s\", \"newpath\": \"%s\", \"newfilename\": \"%s\", \"newextension\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, filename, extension, values.newpath, newfilename, newextension);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_mkdir(values values) {
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"mkdir\", \"type_op\": \"dir\", \"path\": \"%s\", \"mode\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path, values.mode);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}

int server_rmdir(values values) {
    printf("----------------------------------------\n");
    // Obtener Timestamp actual
    char timestamp_str[50];
    obtain_actualtime(timestamp_str);

    // Ruta del archivo
    char path[256];
    strcpy(path, values.path);
    printf("La ruta obtenida es: %s\n", path);

    // Preparación del formato json
    char jsonline[2048];
    sprintf(jsonline, "{ \"timestamp_server\": \"%s\", \"timestamp_client\": \"%s\", \"IP\": \"%s\", \"process\": \"%s\", \"operation\": \"rmdir\", \"type_op\": \"dir\", \"path\": \"%s\" }\n", timestamp_str, values.clnt_time, values.ip, values.process, path);
    
    // Se escribe en el log
    insert_datalog(jsonline);
    printf("Se inserta al archivo prueba_log.log:\n%s", jsonline);
    printf("----------------------------------------\n");
    return 0;
}
