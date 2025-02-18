#include "../include/http_utils.h"
#include "../include/mime_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Define buffer size for HTTP headers
#define BUFFER_SIZE 1024

void send_response(SOCKET client_socket, const char* status_code, 
                  const char* content_type, const char* content, 
                  long content_length) {
    // Buffer for storing HTTP headers
    char header[BUFFER_SIZE];
    
    // Format HTTP response headers
    // Note: Using multiple string literals for better readability
    sprintf(header, 
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, content_type, content_length);
    
    // Send headers and content separately
    // TODO: Add error handling for send() calls
    send(client_socket, header, strlen(header), 0);
    send(client_socket, content, content_length, 0);
}

void send_file(SOCKET client_socket, const char* filepath) {
    // Try to open the file in binary mode
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        // Handle file not found error
        const char* error_content = "<h1>404 Not Found</h1>";
        send_response(client_socket, "404 Not Found", "text/html", 
                     error_content, strlen(error_content));
        return;
    }

    // Get file size using seek operations
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for file content
    char* content = malloc(file_size);
    if (content == NULL) {
        // Handle memory allocation failure
        fclose(file);
        const char* error_content = "<h1>500 Internal Server Error</h1>";
        send_response(client_socket, "500 Internal Server Error", 
                     "text/html", error_content, strlen(error_content));
        return;
    }

    // Read entire file into memory
    // TODO: Add error handling for fread
    fread(content, 1, file_size, file);
    fclose(file);

    // Send file with appropriate MIME type
    send_response(client_socket, "200 OK", get_mime_type(filepath), 
                 content, file_size);
    
    // Clean up allocated memory
    free(content);
}

// Add this function for handling large files
void send_file_chunked(SOCKET client_socket, const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) return;

    char buffer[8192];  // 8KB chunks
    size_t bytes_read;
    
    // Send headers with Transfer-Encoding: chunked
    const char* headers = 
        "HTTP/1.1 200 OK\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Content-Type: %s\r\n"
        "\r\n";
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        // Send chunk size in hex
        char chunk_header[32];
        sprintf(chunk_header, "%zx\r\n", bytes_read);
        send(client_socket, chunk_header, strlen(chunk_header), 0);
        
        // Send chunk data
        send(client_socket, buffer, bytes_read, 0);
        send(client_socket, "\r\n", 2, 0);
    }
    
    // Send final chunk
    send(client_socket, "0\r\n\r\n", 5, 0);
    fclose(file);
}