#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <netdb.h>
#include "mensaje.h"

pthread_mutex_t mutex_pet;
pthread_cond_t cond_pet;

char *recover_filename(int file) {
    // Se obtiene el valor de la ruta
    char file_path[256], *filename;
    sprintf(file_path, "/proc/self/fd/%d", file);
    
    // Se comprueba que no haya error en la asignación de memoria
    if ((filename = malloc(256)) == NULL)
        return "";

    // Se lee el enlace simbólico
    int index;
    if ((index = readlink(file_path, filename, 255)) < 0) {
        // En caso de error se libera la memoria y se devuelve nulo
        free(filename);
        return "";
    }

    // Se establece el valor último como \0
    filename[index] = '\0';
    return filename;
}

char *recover_processname() {
    // Se obtiene la ruta al ejecutable del programa a través del PID
    pid_t pid = getpid();
    char proc_path[256], *processname;
    sprintf(proc_path, "/proc/%d/cmdline", pid);

    // Se abre el archivo de proceso
    FILE *file = fopen(proc_path, "r");
    if (file == NULL)
        return "";

    // Se reserva memoria para coger el proceso
    if ((processname = malloc(256)) == NULL) {
        fclose(file);
        return "";
    }

    // Se lee el nombre del proceso
    int end = fread(processname, sizeof(char), 255, file);
    fclose(file);

    // Se establece \0 al final y se devuelve el nombre
    processname[end] = '\0';
    return processname;
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

int send_info(package *package) {
    struct package pkg = *package;
    // Se obtiene el nombre del proceso
    sprintf(pkg.process, "%s", recover_processname());

    // Se crea el socket de conexión
    int sd;
    // Se conecta
    if (set_socket_and_connect(&sd) == -1){
        return -1;
    }
    
    // Se envia el código de operación
    if (sendMessage(sd, pkg.op, strlen(pkg.op) + 1) == -1){
        perror("ERROR: No se pudo mandar la solicitud de operación\n");
        close(sd);
        return -1;
    }
    // Se envia el momento de la realización de la operación
    if (sendMessage(sd, pkg.clnt_time, strlen(pkg.clnt_time) + 1) == -1){
        perror("ERROR: No se pudo mandar el tiempo de realización\n");
        close(sd);
        return -1;
    }
    // Se envía la ruta del archivo abierto
    if (sendMessage(sd, pkg.path, strlen(pkg.path) + 1) == -1){
        perror("ERROR: No se pudo mandar la ruta del fichero\n");
        close(sd);
        return -1;
    }
    // Se envía el nombre del proceso
    if (sendMessage(sd, pkg.process, strlen(pkg.process) + 1) == -1){
        perror("ERROR: No se pudo mandar la ruta del fichero\n");
        close(sd);
        return -1;
    }

    // En caso de ser open se envían los flags
    if (strcmp(pkg.op, "0") == 0){
        if (sendMessage(sd, pkg.flags, strlen(pkg.flags) + 1) == -1){
            perror("ERROR: No se pudo mandar el valor\n");
            close(sd);
            return -1;
        }
    }

    // En caso de ser open/mkdir se encvían el mode
    if (strcmp(pkg.op, "0") == 0 || strcmp(pkg.op, "8") == 0){
        if (sendMessage(sd, pkg.mode, strlen(pkg.mode) + 1) == -1){
            perror("ERROR: No se pudo mandar el valor\n");
            close(sd);
            return -1;
        }
    }

    // En caso de ser read/write se envían los bytes leídos/escritos y su count
    if (strcmp(pkg.op, "2") == 0 || strcmp(pkg.op, "3") == 0){
        if (sendMessage(sd, pkg.buffer, strlen(pkg.buffer) + 1) == -1){
            perror("ERROR: No se pudo mandar el valor\n");
            close(sd);
            return -1;
        }
        if (sendMessage(sd, pkg.count, strlen(pkg.count) + 1) == -1){
            perror("ERROR: No se pudo mandar el valor\n");
            close(sd);
            return -1;
        }
    }

    // En caso de ser lseek se envia su offset y whence
    if (strcmp(pkg.op, "4") == 0){
        if (sendMessage(sd, pkg.offset, strlen(pkg.offset) + 1) == -1){
            perror("ERROR: No se pudo mandar el valor\n");
            close(sd);
            return -1;
        }
        if (sendMessage(sd, pkg.whence, strlen(pkg.whence) + 1) == -1){
            perror("ERROR: No se pudo mandar el valor\n");
            close(sd);
            return -1;
        }
    }

    // En caso de ser rename se envia la nueva ruta
    if (strcmp(pkg.op, "7") == 0){
        if (sendMessage(sd, pkg.newpath, strlen(pkg.newpath) + 1) == -1){
            perror("ERROR: No se pudo mandar el valor\n");
            close(sd);
            return -1;
        }
    }

    return 0;
}

void *send_info_wrapper(void *arg) {
    package *pkg = (package *)arg;
    send_info(pkg);
    free(pkg);
    return NULL;
}

int open(const char *path, int flags, mode_t mode) {
    // printf("INTERCEPTED OPEN\n");
    // Intercepta el valor de la llamada open POSIX
    int (*posix_open)(const char*, int, mode_t) = dlsym(RTLD_NEXT, "open");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *open_package = malloc(sizeof(package));
    obtain_actualtime(open_package->clnt_time);
    strcpy(open_package->op, "0");   // Código 0 de open
    // Envío de la ruta del archivo y los argumentos de la función
    snprintf(open_package->path, sizeof(open_package->path), "%s", recover_filename(posix_open(path, flags, mode)));
    snprintf(open_package->flags, sizeof(open_package-> flags), "%d", flags);
    snprintf(open_package->mode, sizeof(open_package->mode), "%04o", (unsigned int)mode);
    
    // Se inicializa el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, open_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función open
    return posix_open(path, flags, mode);
}

int close(int fd) {
    // printf("INTERCEPTED CLOSE\n");
    // Intercepta el valor de la llamada close POSIX
    int (*posix_close)(int) = dlsym(RTLD_NEXT, "close");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *close_package = malloc(sizeof(package));
    obtain_actualtime(close_package->clnt_time);
    strcpy(close_package->op, "1");   // Código 1 de close
    // Envío de la ruta del archivo
    snprintf(close_package->path, sizeof(close_package->path), "%s", recover_filename(fd));
    
    // Se inicializa el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, close_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función close
    return posix_close(fd);
}

ssize_t read(int fd, void *buf, size_t count) {
    // printf("INTERCEPTED READ\n");
    // Intercepta el valor de la llamada read POSIX
    ssize_t (*posix_read)(int, void *, size_t) = dlsym(RTLD_NEXT, "read");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *read_package = malloc(sizeof(package));
    obtain_actualtime(read_package->clnt_time);
    strcpy(read_package->op, "2");   // Código 2 de read
    // Envío de la ruta del archivo, de los bytes leídos y del count
    snprintf(read_package->path, sizeof(read_package->path), "%s", recover_filename(fd));
    snprintf(read_package->count, sizeof(read_package->count), "%ld", count);
    // Vemos la cadena de texto leída
    ssize_t bytesRead = posix_read(fd, buf, count);

    if (bytesRead > 0) {
        size_t to_copy = (bytesRead < 512) ? bytesRead : 511;
        memcpy(read_package->buffer, buf, to_copy);
        read_package->buffer[to_copy] = '\0';
    }
    
    // Se inicializa el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, read_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función read
    return bytesRead;
}

ssize_t write(int fd, const void *buf, size_t count) {
    // printf("INTERCEPTED WRITE\n");
    // Intercepta el valor de la llamada write POSIX
    ssize_t (*posix_write)(int, const void *, size_t) = dlsym(RTLD_NEXT, "write");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *write_package = malloc(sizeof(package));
    obtain_actualtime(write_package->clnt_time);
    strcpy(write_package->op, "3");   // Código 3 de write
    // Envío de la ruta del archivo, de los bytes escritos y del count
    snprintf(write_package->path, sizeof(write_package->path), "%s", recover_filename(fd));
    snprintf(write_package->count, sizeof(write_package->count), "%ld", count);
    // Copiamos la cadena de texto a escribir
    strcpy(write_package->buffer, (char *)buf);

    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, write_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función write
    return posix_write(fd, buf, count);
}

off_t lseek(int fd, off_t offset, int whence) {
    // printf("INTERCEPTED LSEEK\n");
    // Intercepta el valor de la llamada lseek POSIX
    ssize_t (*posix_lseek)(int, off_t, int) = dlsym(RTLD_NEXT, "lseek");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *lseek_package = malloc(sizeof(package));
    obtain_actualtime(lseek_package->clnt_time);
    strcpy(lseek_package->op, "4");   // Código 4 de lseek
    // Envío de la ruta del archivo y los argumentos de la función
    snprintf(lseek_package->path, sizeof(lseek_package->path), "%s", recover_filename(fd));
    snprintf(lseek_package->offset, sizeof(lseek_package->offset), "%ld", offset);
    snprintf(lseek_package->whence, sizeof(lseek_package->whence), "%d", whence);

    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, lseek_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función lseek
    return posix_lseek(fd, offset, whence);
}

int unlink(const char *path) {
    // printf("INTERCEPTED UNLINK\n");
    // Intercepta el valor de la llamada unlink POSIX
    int (*posix_unlink)(const char *) = dlsym(RTLD_NEXT, "unlink");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *unlink_package = malloc(sizeof(package));
    obtain_actualtime(unlink_package->clnt_time);
    strcpy(unlink_package->op, "5");   // Código 5 de unlink
    // Envío de la ruta del archivo
    snprintf(unlink_package->path, sizeof(unlink_package->path), "%s", path);
    
    // Se inicializa el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, unlink_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función unlink
    return posix_unlink(path);
}

int remove(const char *path) {
    // printf("INTERCEPTED REMOVE\n");
    // Intercepta el valor de la llamada remove POSIX
    int (*posix_remove)(const char *) = dlsym(RTLD_NEXT, "remove");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *remove_package = malloc(sizeof(package));
    obtain_actualtime(remove_package->clnt_time);
    strcpy(remove_package->op, "6");   // Código 6 de remove
    // Envío de la ruta del archivo
    snprintf(remove_package->path, sizeof(remove_package->path), "%s", path);
    
    // Se inicializa el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, remove_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función remove
    return posix_remove(path);
}

int rename(const char *oldpath, const char *newpath) {
    // printf("INTERCEPTED RENAME\n");
    // Intercepta el valor de la llamada rename POSIX
    int (*posix_rename)(const char *, const char *) = dlsym(RTLD_NEXT, "rename");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *rename_package = malloc(sizeof(package));
    obtain_actualtime(rename_package->clnt_time);
    strcpy(rename_package->op, "7");   // Código 7 de rename
    // Envío de la ruta del archivo y la nueva ruta
    snprintf(rename_package->path, sizeof(rename_package->path), "%s", oldpath);
    snprintf(rename_package->newpath, sizeof(rename_package->newpath), "%s", newpath);
    
    // Se inicializa el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, rename_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función rename
    return posix_rename(oldpath, newpath);
}

int mkdir(const char *path, mode_t mode) {
    // printf("INTERCEPTED MKDIR\n");
    // Intercepta el valor de la llamada mkdir POSIX
    int (*posix_mkdir)(const char*, mode_t) = dlsym(RTLD_NEXT, "mkdir");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *mkdir_package = malloc(sizeof(package));
    obtain_actualtime(mkdir_package->clnt_time);
    strcpy(mkdir_package->op, "8");   // Código 8 de mkdir
    // Envío de la ruta del archivo y los argumentos de la función
    snprintf(mkdir_package->path, sizeof(mkdir_package->path), "%s", path);
    snprintf(mkdir_package->mode, sizeof(mkdir_package->mode), "%04o", (unsigned int)mode);
    
    // Se inicializa el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, mkdir_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función mkdir
    return posix_mkdir(path, mode);
}

int rmdir(const char *path) {
    // printf("INTERCEPTED RMKDIR\n");
    // Intercepta el valor de la llamada rmdir POSIX
    int (*posix_rmdir)(const char *) = dlsym(RTLD_NEXT, "rmdir");

    /*
        Crear estructura para el envío de todos los datos necesarios
    */

    package *rmdir_package = malloc(sizeof(package));
    obtain_actualtime(rmdir_package->clnt_time);
    strcpy(rmdir_package->op, "9");   // Código 9 de rmdir
    // Envío de la ruta del archivo
    snprintf(rmdir_package->path, sizeof(rmdir_package->path), "%s", path);
    
    // Se inicializa el hilo y sus atributos
    pthread_t thread_pet;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    // Se inicializa la petición
    pthread_create(&thread_pet, &attr_thread, send_info_wrapper, rmdir_package);
    pthread_attr_destroy(&attr_thread);

    // Devuelve el valor de la función rmdir
    return posix_rmdir(path);
}
