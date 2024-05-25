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

#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "mem.h"

typedef enum {
    TYPE_FLOAT = 1,
    TYPE_BOOLEAN = 2,
    TYPE_INT = 3,
    TYPE_CHAR = 5,
    TYPE_OBJ
} ValueType;

typedef enum {
    OBJ_STRING = 4
} ObjectType;

typedef struct {
    ObjectType type;
} Object;

typedef struct {
    Object obj;
    int len;
    char* chars;
} ObjString;

typedef struct {
    ValueType type;

    union {
        int      int_value;
        float    float_value;
        bool     bool_value;
        char     char_value;
        Object* obj;
    };
} Value;

typedef struct {
    int    capacity;
    int    count;
    Value* values;
} Values;

#define AS_INT(value) value.int_value
#define AS_FLOAT(value) value.float_value
#define AS_BOOLEAN(value) value.bool_value
#define AS_CHAR(value) value.char_value
#define AS_OBJ(value) value.obj
#define AS_STRING(value) ((ObjString*) AS_OBJ(value))

#define INT_VALUE(value)   ((Value) { TYPE_INT, {.int_value = value }})
#define FLOAT_VALUE(value) ((Value) { TYPE_FLOAT, {.float_value = value }})
#define BOOLEAN_VALUE(value) ((Value) { TYPE_BOOLEAN, {.bool_value = value }})
#define CHAR_VALUE(value) ((Value) { TYPE_CHAR, {.char_value = value }})
#define OBJ_VALUE(value) ((Value) { TYPE_OBJ, {.obj = (Object*)value }})

#define IS_INT(value)     (value.type == TYPE_INT)
#define IS_FLOAT(value)   (value.type == TYPE_FLOAT)
#define IS_BOOLEAN(value) (value.type == TYPE_BOOLEAN)
#define IS_CHAR(value)    (value.type == TYPE_CHAR)
#define IS_OBJ(value)     (value.type == TYPE_OBJ)
#define IS_NUMBER(value)  (value.type == TYPE_FLOAT || value.type == TYPE_INT)
#define IS_NUMERIC(value) (value.type == TYPE_FLOAT || value.type == TYPE_INT || value.type == TYPE_BOOLEAN)

#define OBJ_TYPE(value) AS_OBJ(value)->type

static Object* allocate_object(size_t size, ObjectType type) {
    Object* object = (Object*)reallocate(NULL, size);
    object->type = type;
    return object;
}

#define ALLOCATE_OBJ(type, obj_type) \
    (type*)allocate_object(sizeof(type), obj_type)

static bool is_obj_type(Value value, ObjectType type) {
    return (IS_OBJ(value) && AS_OBJ(value)->type == type);
}

#define IS_STRING(value) is_obj_type(value, OBJ_STRING)

extern void value_init(Values* array);

extern void value_write(Value value, Values* array);

extern void value_free(Values* array);

extern void value_allocate(Values* array, int capacity);

extern void value_print_debug(Value value, FILE* log_file);

extern void value_print_output(Value value);

#endif // !VALUE_H