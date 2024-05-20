#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void print_usage() {
    fprintf(stderr, "Usage: ./mini_server [port] [--directory path]\n");
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

