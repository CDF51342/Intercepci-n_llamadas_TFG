// Struct petition
struct petition {
    int socket;
    char ip[15];
} typedef petition;

// Mutex functions
void init_mutex();
void lock_mutex();
void unlock_mutex();
void destroy_mutex();

// Data functions
void obtain_actualtime(char *timestamp_str);
void obtain_filename(char *path, char **filename, char **extension);
void format_bytes_inside(char *string);
int get_path_logstash(char **path);
void insert_datalog(char *jsonline);
