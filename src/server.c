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
#define DEFAULT_PAGE "src/home.html"
#define CACHE_SIZE 10

// Structure for caching frequently accessed files
typedef struct {
    char *path;
    char *content;
    long size;
} FileCache;

FileCache cache[CACHE_SIZE];
int cache_count = 0;

// Function to serve default page
void serve_default_page(int client_socket) {
    FILE *file = fopen(DEFAULT_PAGE, "rb");
    if (file == NULL) {
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<h1>500 Internal Server Error</h1>";
        send(client_socket, error_response, strlen(error_response), 0);
        return;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_contents = malloc(file_size);
    fread(file_contents, 1, file_size, file);
    fclose(file);
    
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n", file_size);
    send(client_socket, response, strlen(response), 0);
    send(client_socket, file_contents, file_size, 0);
    free(file_contents);
}

void handle_request(int client_socket) {

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Received request: %s\n", buffer);

        // Parse the request to get the requested file
        char method[10], path[100];
        sscanf(buffer, "%s %s", method, path);
        
        // Handle root path request
        if (strcmp(path, "/") == 0) {
            serve_default_page(client_socket);
            return;
        }

        // Remove leading '/' from path
        if (path[0] == '/') {
            memmove(path, path + 1, strlen(path));
        }

        // Determine the file extension
        const char *ext = strrchr(path, '.');
        const char *mime_type = get_mime_type(ext ? ext + 1 : "");
        if (mime_type == NULL) {
            const char *error_response = "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n<h1>415 Unsupported Media Type</h1>";
            send(client_socket, error_response, strlen(error_response), 0);
            return;
        }


        // Construct the full path
        char full_path[150];
        snprintf(full_path, sizeof(full_path), "src/%s", path);

        // Check cache first
        for (int i = 0; i < cache_count; i++) {
            if (strcmp(cache[i].path, full_path) == 0) {
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", mime_type, cache[i].size);
                send(client_socket, response, strlen(response), 0);
                send(client_socket, cache[i].content, cache[i].size, 0);
                return;
            }
        }

        // Open the requested file
        FILE *file = fopen(full_path, "rb");
        if (file == NULL) {
            const char *error_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
            send(client_socket, error_response, strlen(error_response), 0);
            return;
        }


        // Read file contents
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *file_contents = malloc(file_size);
        fread(file_contents, 1, file_size, file);
        fclose(file);

        // Add to cache if there's space
        if (cache_count < CACHE_SIZE) {
            cache[cache_count].path = strdup(full_path);
            cache[cache_count].content = malloc(file_size);
            memcpy(cache[cache_count].content, file_contents, file_size);
            cache[cache_count].size = file_size;
            cache_count++;
        }


        // Construct response
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", mime_type, file_size);
        send(client_socket, response, strlen(response), 0);
        send(client_socket, file_contents, file_size, 0);
        free(file_contents);
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
