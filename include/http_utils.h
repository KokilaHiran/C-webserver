#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <winsock2.h>

// Function to send HTTP response
void send_response(SOCKET client_socket, const char* status_code, const char* content_type, const char* content, long content_length);

// Function to send file content
void send_file(SOCKET client_socket, const char* filepath);

#endif // HTTP_UTILS_H