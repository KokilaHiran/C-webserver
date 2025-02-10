#include <string.h>
#include <stdlib.h>

const char *get_mime_type(const char *file_ext)
{
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0)
    {
        return "text/html";
    }
    else if (strcasecmp(file_ext, "txt") == 0)
    {
        return "text/plain";
    }
    else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0)
    {
        return "image/jpeg";
    }
    else if (strcasecmp(file_ext, "png") == 0)
    {
        return "image/png";
    }
    else if (strcasecmp(file_ext, "mp4") == 0)
    {
        return "video/mp4";
    }
    else if (strcasecmp(file_ext, "mp3") == 0)
    {
        return "audio/mpeg";
    }
    else
    {
        return "application/octet-stream";
    }
}

char *url_decode(const char *src)
{
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    size_t decoded_len = 0;

    // decode %2x to hex
    for (size_t i = 0; i < src_len; i++)
    {
        if (src[i] == '%' && i + 2 < src_len)
        {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = hex_val;
            i += 2;
        }
        else
        {
            decoded[decoded_len++] = src[i];
        }
    }

    // add null terminator
    decoded[decoded_len] = '\0';
    return decoded;
}