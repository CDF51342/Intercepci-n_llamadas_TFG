#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <png.h>


void read_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
    // Lee los bytes para construir la estructura
    int fd = *(int *)png_get_io_ptr(png_ptr);
    if (read(fd, data, length) != length) {
        png_error(png_ptr, "Error reading from file");
    }
}

void read_png_file(const char *filename) {
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
        if (strcmp(text_ptr[i].key, "Title") == 0) {
            printf("Title: %s\n", text_ptr[i].text);
        }
        if (strcmp(text_ptr[i].key, "Author") == 0) {
            printf("Author: %s\n", text_ptr[i].text);
        }
    }

    png_destroy_read_struct(&png, &info, NULL);
    close(fd);
}

int main() {
    printf("--------------------\nFichero: Arquitectura.png:\n");
    read_png_file("data/images/Arquitectura.png");
    printf("\n--------------------\nFichero: UseCaseDiagram1.png:\n");
    read_png_file("data/images/UseCaseDiagram1.png");
    printf("\n--------------------\n");
    return 0;
}
