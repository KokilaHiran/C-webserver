#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <sys/types.h>
#include "../lib/http_helper.h"
#include "../lib/file_helper.h"

#define PORT 8080
#define BUFFER_SIZE 8192  // Buffer size for file transfers

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received request: %s\n", buffer);

        char method[10], path[100];
        sscanf(buffer, "%s %s", method, path);
        
        if (path[0] == '/') {
            memmove(path, path + 1, strlen(path));
        }

        const char *ext = strrchr(path, '.');
        const char *mime_type = get_mime_type(ext ? ext + 1 : "");
        if (mime_type == NULL) {
            const char *error_response = "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n<h1>415 Unsupported Media Type</h1>";
            send(client_socket, error_response, strlen(error_response), 0);
            return;
        }

        char full_path[150];
        snprintf(full_path, sizeof(full_path), "src/%s", path);

        FILE *file = fopen(full_path, "rb");
        if (file == NULL) {
            const char *error_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
            send(client_socket, error_response, strlen(error_response), 0);
            return;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", mime_type, file_size);
        send(client_socket, response, strlen(response), 0);

        // Stream file in chunks
        char file_buffer[BUFFER_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
            send(client_socket, file_buffer, bytes_read, 0);
        }

        fclose(file);
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return 1;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        perror("Failed to listen on socket");
        return 1;
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Failed to accept connection");
            continue;
        }

        handle_request(client_socket);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
