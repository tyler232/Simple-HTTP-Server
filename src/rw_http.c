#include "rw_http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>

static const char *http_request_500 = "HTTP/1.1 500 Internal Server Error\r\n"
                                       "Content-Type: text/html\r\n"
                                       "Connection: close\r\n"
                                       "\r\n"
                                       "<!DOCTYPE html>\n"
                                       "<html>\n"
                                       "<head><title>Simple HTTP Server</title></head>\n"
                                       "<body>\n"
                                       "<h1>500 Internal Server Error</h1>\n"
                                       "</body>\n"
                                       "</html>\n";

static const char *http_request_404 = "HTTP/1.1 404 Not Found\r\n"
                                       "Content-Type: text/html\r\n"
                                       "Connection: close\r\n"
                                       "\r\n"
                                       "<!DOCTYPE html>\n"
                                       "<html>\n"
                                            "<head><title>Simple HTTP Server</title></head>\n"
                                            "<body>\n"
                                            "<h1>404 Not Found</h1>\n"
                                            "</body>\n"
                                       "</html>\n";

void write_500(int socket_fd) {
    write(socket_fd, http_request_500, strlen(http_request_500));
}

void write_404(int socket_fd) {
    write(socket_fd, http_request_404, strlen(http_request_404));
}

