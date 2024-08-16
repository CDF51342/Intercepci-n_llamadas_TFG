// Struct values
struct values {
    char cod_op[2];
    char ip[15];
    char path[256];
    char clnt_time[50];
    char process[256];
    char flags[11];
    char mode[12];
    char buffer[512];
    char count[11];
    char offset[11];
    char whence[11];
    char newpath[256];
} typedef values;

/* FUNCIONES POSIX SERVER */
// FUNCIONES FICHEROS
int server_open(values values);
int server_close(values values);
int server_read(values values);
int server_write(values values);
int server_lseek(values values);

// FUNCIONES DIRECTORIOS
int server_unlink(values values);
int server_remove(values values);
int server_rename(values values);
int server_mkdir(values values);
int server_rmdir(values values);
