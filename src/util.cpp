/**
 * @file util.cpp
 * @author strah19
 * @brief Helper functions that can be used throughout the code base.
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

char* open_file(const char* filepath) {
    FILE* file = fopen(filepath, "rb");

    if (!file)
        fatal_error("Unable to open file '%s' for compilation.\n", filepath);

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = new char[file_size + 1];

    if (!buffer)
        fatal_error("Unable to allocate memory to read file '%s'\n", filepath);

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

char* create_string(char* start, int size) {
    char* str = (char*) malloc(size + 1);
    memset(str, '\0', size + 1);
    strncpy(str, start, size);
}