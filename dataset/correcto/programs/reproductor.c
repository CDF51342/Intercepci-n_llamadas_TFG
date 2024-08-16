#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>


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
    close(file_lyrics);

    wait(NULL);
    
    return 0;
}
