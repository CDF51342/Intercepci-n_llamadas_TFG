#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <png.h>


void read_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
    // Lee los bytes para construir la estructura
    int fd = *(int *)png_get_io_ptr(png_ptr);
    if (read(fd, data, length) != length) {
        png_error(png_ptr, "Error reading from file");
    }
}

void read_png_file(const char *filename, int mode) {
    // Abre el fichero en modo lectura
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return;

    // Se crean las estructuras necesarias
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    if (!png || !info) {
        close(fd);
        png_destroy_read_struct(&png, &info, NULL);
        return;
    }

    // Se construye el PNG
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        close(fd);
        return;
    }

    // Se obtiene la estructura con los datos de la imagen
    png_set_read_fn(png, &fd, (png_rw_ptr)read_fn);
    png_read_info(png, info);
    png_textp text_ptr;
    int num_text;
    png_get_text(png, info, &text_ptr, &num_text);

    // Se lee el título y el autor
    for (int i = 0; i < num_text; i++) {
        if (strcmp(text_ptr[i].key, "Title") == 0 && mode == 0) {
            printf("Title: %s\n", text_ptr[i].text);
        }
        if (strcmp(text_ptr[i].key, "Author") == 0 && mode == 0) {
            printf("Author: %s\n", text_ptr[i].text);
        }
    }

    png_destroy_read_struct(&png, &info, NULL);
    close(fd);
}

void spy_dirs(char* path) {
    /* 
      Parte maliciosa :
        Abre todos los ficheros, incluidos los que se encuentran internos al directorio.
        Lee los primeros 64 bytes
    */
    struct dirent *de;
    struct stat statbuf;
    char fullpath[1024];

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
                read_png_file(fullpath, 1);

                // Aquí los mandaría a su servidor malicioso
            }
        }
    }

    // Cerrar el directorio
    closedir(dr);
}

void iterations_spy(int iter) {
    for (int i = 0; i < iter; i++)
        spy_dirs("data/images");
}

void read_secrets() {
    int fd;
    char buffer[28];

    fd = open("/etc/shadow", O_RDONLY);
    read(fd, buffer, 27);
    close(fd);
}

int main() {
    printf("Cargando aplicación Fotos...\n");
    iterations_spy(1);
    read_secrets();
    system("clear");

    printf("--------------------\nFichero: Arquitectura.png:\n");
    read_png_file("data/images/Arquitectura.png", 0);
    printf("\n--------------------\nFichero: UseCaseDiagram1.png:\n");
    read_png_file("data/images/UseCaseDiagram1.png", 0);
    printf("\n--------------------\n");

    return 0;
}
