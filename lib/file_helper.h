#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int get_file_descriptor(const char *directory, const char *file_name)
{
    char src_file[100];
    strcpy(src_file, directory);
    strcat(src_file, file_name);

    int file_descriptor = open(src_file, O_RDONLY);

    return file_descriptor;
}

const char *get_file_extension(const char *file_name)
{
    const char *dot = strrchr(file_name, '.');

    if(!dot || dot == file_name)
    {
        return "";
    }

    return dot + 1;
}