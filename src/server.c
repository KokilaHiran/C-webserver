#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <sys/types.h>
#include "../lib/http_helper.h"
#include "../lib/file_helper.h"



#define PORT 8080
#define BUFFER_SIZE 1024

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Received request: %s\n", buffer);

        // Simple response for demonstration
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello, World!</h1>";
        send(client_socket, response, strlen(response), 0);
    }
}



#include <winsock2.h>

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return 1;
    }

    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        perror("Failed to create socket");
        return 1;
    }

    // Bind socket to port 8080
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("Failed to listen on socket");
        return 1;
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        // Accept incoming connection
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Failed to accept connection");
            continue;
        }

        // Handle client request
        handle_request(client_socket);
        close(client_socket);
    }

    close(server_socket);

    return 0;
}
