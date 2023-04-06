/**
* @file value.c
* @author strah19
* @date July 13, 2022
* @version 1.0
*
* @section LICENSE
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the MIT License as published
* by the Free Software Foundation.
*
* @section DESCRIPTION
*
* Lays out the structure for values in the virtual machine 
* and the value array in a bytecode chunk.
*/

#include "value.h"

#define BIN_BUF_SIZE 33

static char* int_to_bin(int a, char *buffer, int buf_size);

void value_init(Values* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void value_write(Value value, Values* array) {
    if (array->capacity < array->count + 1) {
        array->capacity = NEW_CAPACITY(array->capacity);
        array->values = REALLOC(Value, array->values, array->capacity);
    }

    array->values[array->count++] = value;
}

void value_free(Values* array) {
    FREE(Value, array->values);
    value_init(array);
}

static char* int_to_bin(int a, char *buffer, int buf_size) {
    buffer += (buf_size - 1);
    for (int i = 31; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';
        a >>= 1;
    }
    return buffer;
}

void value_print(Value value, bool newline) {
    switch (value.type) {
    case TYPE_FLOAT:   printf("%g", AS_FLOAT(value));   break;
    case TYPE_INT:     printf("%d", AS_INT(value));     break;
    case TYPE_BOOLEAN: printf("%d", AS_BOOLEAN(value)); break;
    case TYPE_BINARY:  {
        char buffer[BIN_BUF_SIZE];
        buffer[BIN_BUF_SIZE - 1] = '\0';
        int_to_bin(AS_BINARY(value), buffer, BIN_BUF_SIZE - 1);
        printf("%s", buffer);
        break;
    }
    default: printf("(null)"); break;
    }
    printf("%s", (newline) ? "\n" : "");
}