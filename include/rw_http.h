#ifndef RW_HTTP_H
#define RW_HTTP_H

/**
 * write_500 - Sends an HTTP 500 Internal Server Error response to the client.
 *
 * @param socket_fd: The file descriptor of the socket to write the response to.
 *
 * This function sends a complete HTTP 500 Internal Server Error response,
 * including the headers and a simple HTML body, to the specified socket.
 * It is typically called when the server encounters an unexpected condition
 * that prevents it from fulfilling the request.
 */
void write_500(int socket_fd);

/**
 * write_404 - Sends an HTTP 404 Not Found response to the client.
 *
 * @param socket_fd: The file descriptor of the socket to write the response to.
 *
 * This function sends a complete HTTP 404 Not Found response, including
 * the headers and a simple HTML body, to the specified socket.
 * It is typically called when the server cannot find the requested resource.
 */
void write_404(int socket_fd);

#endif
