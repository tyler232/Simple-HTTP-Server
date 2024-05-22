#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "rw_http.h"
#include "utils.h"

#define RESPONSE_OK \
    "HTTP/1.1 200 OK\r\n" \
    "Content-Type: text/html\r\n" \
    "Connection: close\r\n\r\n" \
    "<!DOCTYPE html>\n" \
        "<html lang=\"en\">\n" \
        "<head><title>Simple HTTP Server</title></head>\n" \
        "<body>\n"

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024
#define MAX_EVENTS 128

static int port = DEFAULT_PORT;
static int server_fd, epoll_fd;

void run_server();
void close_server();
void handle_client(int client_socket);
void handle_dirreq(int client_socket, char* path);
void handle_filereq(int client_socket, char* path);
int set_non_blocking(int fd);
void parse_args(int argc, char **argv);

int main(int argc, char **argv) {
    parse_args(argc, argv);

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = close_server;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    run_server();
    close_server();
    return 0;
}

void run_server() {
    int client_socket;
    struct sockaddr_in address;
    struct epoll_event ev, events[MAX_EVENTS];
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if ((epoll_fd = epoll_create1(0)) < 0) {
        perror("Epoll create failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
        perror("Epoll ctl failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", port);

    if (set_non_blocking(server_fd) < 0) {
        perror("Failed to set server socket non-blocking");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            if (errno == EINTR) {
                // Interrupted by signal, continue loop
                continue;
            } else {
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_fd) {
                while (1) {
                    client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    if (client_socket < 0) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            // No more incoming connections
                            break;
                        } else {
                            perror("Accept failed");
                            break;
                        }
                    }

                    if (set_non_blocking(client_socket) < 0) {
                        perror("Failed to set client socket non-blocking");
                        close(client_socket);
                        continue;
                    }

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = client_socket;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev) < 0) {
                        perror("Epoll ctl failed");
                        close(client_socket);
                        continue;
                    }
                }
            } else {
                handle_client(events[i].data.fd);
            }
        }
    }
}

void close_server() {
    printf("Closing server\n");
    close(epoll_fd);
    close(server_fd);
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        perror("Failed to read from socket");
        close(client_socket);
        return;
    }

    if (bytes_read == 0) {
        // Connection closed by client
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0';
    printf("Request:\n%s\n", buffer);

    char method[16], path[256], version[16];
    sscanf(buffer, "%s %s %s", method, path, version);

    if (strcmp(method, "GET") != 0) {
        // Method besides GET disabled 
        write_500(client_socket);
        close(client_socket);
        return;
    }

    if (path[strlen(path) - 1] == '/') {
        handle_dirreq(client_socket, local_path_create(path));
    } else {
        handle_filereq(client_socket, local_path_create(path));
    }

    close(client_socket);
}

// handle directory request, list content of directory if index.html or index.htm not found, 
// else serve index.html or index.htm
void handle_dirreq(int client_socket, char* path) {
    char index_path_html[BUFFER_SIZE];
    char index_path_htm[BUFFER_SIZE];
    snprintf(index_path_html, sizeof(index_path_html), "%s/index.html", path);
    snprintf(index_path_htm, sizeof(index_path_htm), "%s/index.htm", path);

    struct stat st;

    if ((stat(index_path_html, &st) == 0 && S_ISREG(st.st_mode)) ||
        (stat(index_path_htm, &st) == 0 && S_ISREG(st.st_mode))) {
        // Either index.html or index.htm exists and is a regular file, serve it
        if (stat(index_path_html, &st) == 0 && S_ISREG(st.st_mode)) {
            handle_filereq(client_socket, index_path_html);
        } else {
            handle_filereq(client_socket, index_path_htm);
        }
        return;
    }

    char **contents = (char **)malloc(sizeof(char *) * BUFFER_SIZE);
    int num_contents = 0;
    int current_buffer_size = BUFFER_SIZE;

    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            char *curr_name = dir->d_name;
            if (strcmp(curr_name, "..") == 0 || strcmp(curr_name, ".") == 0)
                continue;

            char *name = (char *)malloc(strlen(curr_name) + 2);
            strcpy(name, curr_name);
            if (dir->d_type == DT_DIR) {
                strcat(name, "/");
            }

            contents[num_contents] = name;
            num_contents++;
            if (num_contents >= current_buffer_size) {
                current_buffer_size *= 2;
                contents = (char **)realloc(contents, sizeof(char *) * current_buffer_size);
            }
        }
        closedir(d);
    } else {
        // directory not found
        write_404(client_socket);
        free(contents);
        return;
    }

    char response_header[BUFFER_SIZE] = RESPONSE_OK;
    strcat(response_header, "<h1>Directory listing</h1>\n<hr>\n<ul>\n");
    write(client_socket, response_header, strlen(response_header));

    sort_string_array(contents, num_contents);
    char curr_line[BUFFER_SIZE];
    for (int i = 0; i < num_contents; i++) {
        snprintf(curr_line, sizeof(curr_line), "<li><a href=\"%s\">%s</a></li>\n", contents[i], contents[i]);
        write(client_socket, curr_line, strlen(curr_line));
        free(contents[i]);
    }

    char response[BUFFER_SIZE] = "</ul>\n<hr>\n</body>\n</html>\n";
    write(client_socket, response, strlen(response));
    free(contents);
}

// Handle file request, if file is an html then host it, else download it
void handle_filereq(int client_socket, char* path) {
    int file_fd = open(path, O_RDONLY);
    if (file_fd < 0) {
        perror("Failed to open file");
        // file not found
        write_404(client_socket);
        return;
    }

    char *filename = strrchr(path, '/');
    if (filename == NULL) {
        filename = path;
    } else {
        filename++;
    }

    // if file is html, send as text/html, otherwise send as application/octet-stream
    char response_header[BUFFER_SIZE];
    if (strstr(filename, ".html") != NULL || strstr(filename, ".htm") != NULL) {
        snprintf(response_header, sizeof(response_header),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Connection: close\r\n\r\n");
    } else {
        snprintf(response_header, sizeof(response_header),
                "HTTP/1.1 200 OK\r\n"
                "Content-Disposition: attachment; filename=\"%s\"\r\n"
                "Content-Type: application/octet-stream\r\n"
                "Connection: close\r\n\r\n", filename);
    }

    write(client_socket, response_header, strlen(response_header));

    char file_buffer[BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = read(file_fd, file_buffer, sizeof(file_buffer))) > 0) {
        write(client_socket, file_buffer, bytes_read);
    }

    close(file_fd);
}

int set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
        return -1;
    return 0;
}

void parse_args(int argc, char **argv) {
    switch (argc) {
        case 1:
            break;
        case 2:
            if (strncmp(argv[1], "--port=", 7) == 0) {
                char* port_number = strchr(argv[1], '=') + 1;
                if (is_numeric(port_number)) {
                    port = atoi(port_number);
                    if (port < 1024 || port > 65535) {
                        fprintf(stderr, "Invalid port number\n");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    print_usage();
                    exit(EXIT_FAILURE);
                } 
            } else if (strncmp(argv[1], "--directory=", 12) == 0) {
                char* directory_path = strchr(argv[1], '=') + 1;
                if (chdir(directory_path) < 0) {
                    perror("Failed to change directory");
                    exit(EXIT_FAILURE);
                }
            } else {
                print_usage();
                exit(EXIT_FAILURE);
            }
            break;
        case 3:
            if ((strncmp(argv[1], "--port=", 7) == 0 && strncmp(argv[2], "--directory=", 12) == 0) || 
                (strncmp(argv[1], "--directory=", 12) == 0 && strncmp(argv[2], "--port=", 7) == 0)) {
                char *port_number = (strncmp(argv[1], "--port", 6) == 0) ? 
                                    strchr(argv[1], '=') + 1 : 
                                    strchr(argv[2], '=') + 1;

                char *directory_path = (strncmp(argv[1], "--directory", 11) == 0) ? 
                                       strchr(argv[1], '=') + 1 : 
                                       strchr(argv[2], '=') + 1;
                                       
                if (is_numeric(port_number)) {
                    port = atoi(port_number);
                    if (port < 1024 || port > 65535) {
                        fprintf(stderr, "Invalid port number\n");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    print_usage();
                    exit(EXIT_FAILURE);
                }

                if (chdir(directory_path) < 0) {
                    perror("Failed to change directory");
                    exit(EXIT_FAILURE);
                }
            } else {
                print_usage();
                exit(EXIT_FAILURE);
            }
            break;
        default:
            print_usage();
            exit(EXIT_FAILURE);
    }
}
