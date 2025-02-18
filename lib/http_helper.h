#include <string.h>
#include <stdlib.h>

const char *get_mime_type(const char *file_ext)
{
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0)
    {
        return "text/html";
    }
    else if (strcasecmp(file_ext, "css") == 0)
    {
        return "text/css";
    }
    else if (strcasecmp(file_ext, "js") == 0)
    {
        return "application/javascript";
    }
    else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0)
    {
        return "image/jpeg";
    }
    else if (strcasecmp(file_ext, "png") == 0)
    {
        return "image/png";
    }
    else if (strcasecmp(file_ext, "gif") == 0)
    {
        return "image/gif";
    }
    else if (strcasecmp(file_ext, "mp3") == 0)
    {
        return "audio/mpeg";
    }
    else if (strcasecmp(file_ext, "mp4") == 0)
    {
        return "video/mp4";
    }
    else if (strcasecmp(file_ext, "pdf") == 0)
    {
        return "application/pdf";
    }
    // Add more MIME types as needed
    return "application/octet-stream";

}
