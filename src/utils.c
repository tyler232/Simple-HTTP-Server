#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void print_usage() {
    fprintf(stderr, "Usage: program [--port=<port>] [--directory=<directory>]\n");
    fprintf(stderr, "  --port=<port>         Specify the port number (1024-65535).\n");
    fprintf(stderr, "  --directory=<directory>  Specify the directory to change to.\n");
}

int is_numeric(const char *str) {
    while (*str) {
        if (!isdigit(*str))
            return 0;
        str++;
    }
    return 1;
}

char* local_path_create(char* path) {
    char* local_path = path;
    if (strcmp(local_path, "/") == 0) {
        local_path = ".";
    } else {
        local_path++;
    }
    return local_path;
}

int compare_string(const void *a, const void *b) {
    const char **str_A = (const char **)a;
    const char **str_B = (const char **)b;
    return strcasecmp(*str_A, *str_B);
}

void sort_string_array(char **array, int size) {
    qsort(array, size, sizeof(char *), compare_string);
}
