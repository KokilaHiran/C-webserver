#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/http_utils.h"
#include "../include/mime_types.h"

// Define buffer size for HTTP headers
#define BUFFER_SIZE 1024

void send_response(SOCKET client_socket, const char* status_code, 
                  const char* content_type, const char* content, 
                  long content_length) {
    // Prepare HTTP response header
    char header[1024];
    sprintf(header, 
            "HTTP/1.1 %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n"
            "\r\n",
            status_code, content_type, content_length);

    // Send header
    send(client_socket, header, strlen(header), 0);
    
    // Send content
    send(client_socket, content, content_length, 0);
}

void send_file(SOCKET client_socket, const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        // File not found - send 404 response
        const char* error_msg = "<h1>404 Not Found</h1>";
        send_response(client_socket, "404 Not Found", "text/html", 
                     error_msg, strlen(error_msg));
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file content
    char* content = malloc(file_size);
    fread(content, 1, file_size, file);
    fclose(file);

    // Get MIME type based on file extension
    const char* mime_type = get_mime_type(filepath);
    
    // Send response
    send_response(client_socket, "200 OK", mime_type, content, file_size);
    
    // Clean up
    free(content);
}

// Add this function for handling large files
void send_file_chunked(SOCKET client_socket, const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) return;

    char buffer[8192];  // 8KB chunks
    size_t bytes_read;
    
    // Send headers with Transfer-Encoding: chunked
    char headers[1024];
    snprintf(headers, sizeof(headers),
        "HTTP/1.1 200 OK\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Content-Type: %s\r\n"
        "\r\n",
        get_mime_type(filepath));
    send(client_socket, headers, strlen(headers), 0);
    

    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        // Send chunk size in hex
        char chunk_header[32];
        sprintf(chunk_header, "%zu\r\n", bytes_read);

        send(client_socket, chunk_header, strlen(chunk_header), 0);
        
        // Send chunk data
        send(client_socket, buffer, bytes_read, 0);
        send(client_socket, "\r\n", 2, 0);
    }
    
    // Send final chunk
    send(client_socket, "0\r\n\r\n", 5, 0);
    fclose(file);
}
