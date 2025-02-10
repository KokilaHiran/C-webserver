#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int get_file_descriptor(const char *directory, const char *file_name)
{
    char src_file[100];
    snprintf(src_file, sizeof(src_file), "%s/%s", directory, file_name);
    return open(src_file, O_RDONLY);
}
