#include "../include/http_utils.h"
#include "../include/mime_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

// Function to send HTTP response
void send_response(SOCKET client_socket, const char* status_code, const char* content_type, const char* content, long content_length) {
    char header[BUFFER_SIZE];
    sprintf(header, 
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, content_type, content_length);
    
    send(client_socket, header, strlen(header), 0);
    send(client_socket, content, content_length, 0);
}

// Function to send file content
void send_file(SOCKET client_socket, const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        const char* error_content = "<h1>404 Not Found</h1>";
        send_response(client_socket, "404 Not Found", "text/html", error_content, strlen(error_content));
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file content
    char* content = malloc(file_size);
    if (content == NULL) {
        fclose(file);
        const char* error_content = "<h1>500 Internal Server Error</h1>";
        send_response(client_socket, "500 Internal Server Error", "text/html", error_content, strlen(error_content));
        return;
    }

    fread(content, 1, file_size, file);
    fclose(file);

    // Send file with appropriate MIME type
    send_response(client_socket, "200 OK", get_mime_type(filepath), content, file_size);
    
    free(content);
}