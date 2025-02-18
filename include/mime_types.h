#ifndef MIME_TYPES_H
#define MIME_TYPES_H

// Structure to hold MIME type mappings
struct MimeType {
    const char* extension;
    const char* type;
};

// Function to get MIME type based on file extension
const char* get_mime_type(const char* filename);

#endif // MIME_TYPES_H