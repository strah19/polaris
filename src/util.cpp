/**
 * Copyright (C) 2023 Strahinja Marinkovic - All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the MIT License along with
 * this program. If not, see https://opensource.org/license/mit/
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