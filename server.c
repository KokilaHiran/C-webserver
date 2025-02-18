#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "include/http_utils.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_PATH_LENGTH 2048

// Function to initialize Winsock
void initialize_winsock() {
    WSADATA wsa;
    printf("\nInitializing Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialized.\n");
}

// Function to create a socket
SOCKET create_socket() {
    SOCKET s;
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");
    return s;
}

// Function to bind the socket to a port
void bind_socket(SOCKET s, struct sockaddr_in *server) {
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY;
    server->sin_port = htons(PORT);

    if (bind(s, (struct sockaddr *)server, sizeof(*server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Bind done.\n");
}

// Function to listen for incoming connections
void listen_for_connections(SOCKET s) {
    listen(s, SOMAXCONN);
    printf("Waiting for incoming connections...\n");
}

// Function to handle client connections
void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';

        // Parse HTTP request to get requested file
        char method[32], path[MAX_PATH_LENGTH], protocol[32];
        sscanf(buffer, "%s %s %s", method, path, protocol);

        // Remove leading slash and convert to local path
        char filepath[MAX_PATH_LENGTH] = "src";
        if (strcmp(path, "/") == 0) {
            strcat(filepath, "/home.html");
        } else {
            strcat(filepath, path);
        }

        // Send requested file
        send_file(client_socket, filepath);
    }

    closesocket(client_socket);
}

int main(int argc, char *argv[]) {
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c;

    // Initialize Winsock
    initialize_winsock();

    // Create a socket
    s = create_socket();

    // Bind the socket
    bind_socket(s, &server);

    // Listen for incoming connections
    listen_for_connections(s);

    // Accept and handle incoming connections
    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        printf("Connection accepted.\n");
        handle_client(new_socket);
    }

    if (new_socket == INVALID_SOCKET) {
        printf("accept failed with error code: %d", WSAGetLastError());
        return 1;
    }

    // Cleanup Winsock
    closesocket(s);
    WSACleanup();

    return 0;
}