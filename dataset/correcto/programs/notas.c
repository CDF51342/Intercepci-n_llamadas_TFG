#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv) {
    
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
