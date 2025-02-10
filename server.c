#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_request(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    char file_path[256];
    char full_path[512];

    // Read the request from the client
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    // Extract the requested file path from the request
    sscanf(buffer, "GET /%s HTTP/1.1", file_path);

    // If no specific file is requested, default to "home.html"
    if (strcmp(file_path, "") == 0) {
        strcpy(file_path, "home.html");
    }

    // Construct the full path to the file
    snprintf(full_path, sizeof(full_path), "src/%s", file_path);
    // Allow serving files from subdirectories
    if (strstr(file_path, "/") != NULL) {
        snprintf(full_path, sizeof(full_path), "src/%s", file_path);
    }


    // Open the requested file
    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        // Handle file not found
        const char *not_found_response = "HTTP/1.1 404 Not Found\r\n"
                                         "Content-Type: text/html\r\n\r\n"
                                         "<html><body><h1>404 Not Found</h1></body></html>\r\n";
        send(client_socket, not_found_response, strlen(not_found_response), 0);
        closesocket(client_socket);
        return;
    }

#include "http_helper.h" // Include the header for get_mime_type

    // Use get_mime_type to determine the content type
    const char *file_ext = strrchr(file_path, '.') + 1; // Get the file extension
    const char *mime_type = get_mime_type(file_ext); // Declare and initialize mime_type

    // Send HTTP response headers
    char ok_response[256]; // Change to a char array to hold the response
    snprintf(ok_response, sizeof(ok_response), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", mime_type);

    printf("Requested file: %s\n", file_path); // Debug: log requested file path

    // Determine the content type based on the file extension
    if (strstr(file_path, ".jpg") != NULL || strstr(file_path, ".jpeg") != NULL) {
        const char *ok_response = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: image/jpeg\r\n\r\n";
    } else if (strstr(file_path, ".png") != NULL) {
        const char *ok_response = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: image/png\r\n\r\n";
    } else if (strstr(file_path, ".gif") != NULL) {
        const char *ok_response = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: image/gif\r\n\r\n";
    }

    send(client_socket, ok_response, strlen(ok_response), 0);

    // Send the file content
    printf("Sending response with Content-Type: %s\n", ok_response); // Debug: log content type

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // Close the file and the client socket
    fclose(file);
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %d\n", PORT);

    // Main loop to accept and handle incoming connections
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            closesocket(server_fd);
            WSACleanup();
            return 1;
        }

        handle_request(client_socket);
    }

    // Cleanup
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
