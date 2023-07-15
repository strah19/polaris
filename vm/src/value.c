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

#include "value.h"
#include "mem.h"

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
    free(array->values);
    value_init(array);
}

void value_allocate(Values* array, int capacity) {
    array->capacity = capacity;
    array->values = REALLOC(Value, array->values, array->capacity);
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
    case TYPE_CHAR:    printf("%c", AS_CHAR(value));    break;
    case TYPE_OBJ: {
        switch (AS_OBJ(value)->type) {
        case OBJ_STRING: printf("%s", AS_STRING(value)->chars); break;
        }
        break;
    }
    default: printf("(null)"); break;
    }

    if (newline) printf("\n");
}