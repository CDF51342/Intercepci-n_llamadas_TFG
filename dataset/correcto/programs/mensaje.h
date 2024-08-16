#define MAX 256
#include <unistd.h>
#include <netdb.h>

struct package {
    char op[2];
    char path[276];
    char clnt_time[50];
    char process[256];
    // Open/Close
    char flags[11];
    char mode[12];
    // Read/Write
    char buffer[512];
    char count[11];
    // Lseek
    char offset[11];
    char whence[11];
    // Rename
    char newpath[276];
} typedef package;

int sendMessage(int socket, char * buffer, int len);
ssize_t readLine(int fd, char *buffer, size_t n);

int set_socket_and_connect(int *sc);
