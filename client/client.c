#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    int f;
    f = open("text/mytext.txt", O_WRONLY | O_APPEND, 0644);
    off_t offset = lseek(f, 0, SEEK_END);
    lseek(f, offset, SEEK_CUR);
    const char *text = "Esto es un texto escrito con la función write.\nSe escribe todo bien.\n";
    write(f, text, strlen(text));
    close(f);
    f = open("text/anothertext.txt", O_RDWR, 0644);
    char buffer[256];
    read(f, buffer, 256);
    printf("Se ha leído del fichero -> %s\n", buffer);
    close(f);
    unlink("no_existe.txt");
    remove("no_existe.txt");
    rename("text/anothertext.txt", "text/changedtext.txt");
    rename("text/changedtext.txt", "text/anothertext.txt");
    mkdir("new_directory", 0644);
    rmdir("new_directory");
    return 0;
}
