#include "misc.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

char* open_file(const char* filepath) {
    FILE* file = fopen(filepath, "rb");

    if (!file)
        fatal_error("Unable to open file '%s' for compilation.\n", filepath);

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);

    if (!buffer)
        fatal_error("Unable to allocate memory to read file '%s'\n", filepath);

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}