#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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
    
    return 0;
}
