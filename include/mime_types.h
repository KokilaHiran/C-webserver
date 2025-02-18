#ifndef MIME_TYPES_H
#define MIME_TYPES_H

// Structure to hold MIME type mappings
typedef struct {
    const char* extension;  // File extension
    const char* mime_type;  // Corresponding MIME type
} MimeType;

// Function to get MIME type based on file extension
const char* get_mime_type(const char* filepath);

#endif // MIME_TYPES_H