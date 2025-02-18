#include <string.h>
#include "../include/mime_types.h"

// Define MIME type mappings
static const MimeType mime_types[] = {
    {".html", "text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".ico", "image/x-icon"},
    {".txt", "text/plain"},
    {NULL, "application/octet-stream"}  // Default MIME type
};

const char* get_mime_type(const char* filepath) {
    // Find last occurrence of '.'
    const char* ext = strrchr(filepath, '.');
    if (!ext) {
        return mime_types[sizeof(mime_types)/sizeof(MimeType) - 1].mime_type;
    }

    // Search for matching extension
    for (int i = 0; mime_types[i].extension != NULL; i++) {
        if (strcmp(ext, mime_types[i].extension) == 0) {
            return mime_types[i].mime_type;
        }
    }

    // Return default MIME type if no match found
    return mime_types[sizeof(mime_types)/sizeof(MimeType) - 1].mime_type;
}