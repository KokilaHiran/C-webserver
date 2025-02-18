#include "../include/mime_types.h"
#include <string.h>

// MIME type mappings
struct MimeType mime_types[] = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".mp4", "video/mp4"},
    {".mp3", "audio/mpeg"},
    {".ico", "image/x-icon"},
    {NULL, NULL}
};

// Function to get MIME type based on file extension
const char* get_mime_type(const char* filename) {
    char* ext = strrchr(filename, '.');
    if (ext == NULL) return "application/octet-stream";
    
    for (int i = 0; mime_types[i].extension != NULL; i++) {
        if (strcmp(ext, mime_types[i].extension) == 0) {
            return mime_types[i].type;
        }
    }
    return "application/octet-stream";
}