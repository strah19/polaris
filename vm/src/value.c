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
    for (int i = 0; i < array->count; i++) 
        if (array->values[i].type == TYPE_OBJ) 
            free(array->values[i].obj);

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

void value_print_debug(Value value, FILE* log_file) {
    switch (value.type) {
    case TYPE_FLOAT:   fprintf(log_file, "%g", AS_FLOAT(value));   break;
    case TYPE_INT:     fprintf(log_file, "%d", AS_INT(value));     break;
    case TYPE_BOOLEAN: fprintf(log_file, "%d", AS_BOOLEAN(value)); break;
    case TYPE_CHAR:    {
        if (AS_CHAR(value) == '\n')
            fprintf(log_file, "NL");
        else
            fprintf(log_file, "%c", AS_CHAR(value));    
        break;
    }
    case TYPE_OBJ: {
        switch (AS_OBJ(value)->type) {
        case OBJ_STRING: fprintf(log_file, "%s", AS_STRING(value)->chars); break;
        }
        break;
    }
    default: fprintf(log_file, "(null)"); break;
    }
}

void value_print_output(Value value) {
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
}