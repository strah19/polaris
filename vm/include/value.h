#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOLEAN,
    TYPE_BINARY
} ValueType;

typedef enum {
    OBJ_STRING
} ObjectType;

typedef struct {
    ObjectType type;
} Object;

typedef struct {
    Obj obj;
    int len;
    char* chars;
} ObjString;

typedef struct {
    ValueType type;

    union {
        int      int_value;
        float    float_value;
        bool     bool_value;
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

#define INT_VALUE(value)   ((Value) { TYPE_INT, {.int_value = value }})
#define FLOAT_VALUE(value) ((Value) { TYPE_FLOAT, {.float_value = value }})
#define BOOLEAN_VALUE(value) ((Value) { TYPE_BOOLEAN, {.bool_value = value }})

#define IS_INT(value)   (value.type == TYPE_INT)
#define IS_FLOAT(value) (value.type == TYPE_FLOAT)
#define IS_BOOLEAN(value) (value.type == TYPE_BOOLEAN)
#define IS_NUMBER(value) (value.type == TYPE_FLOAT || value.type == TYPE_INT)
#define IS_NUMERIC(value) (value.type == TYPE_FLOAT || value.type == TYPE_INT || value.type == TYPE_BOOLEAN)

extern void value_init(Values* array);

extern void value_write(Value value, Values* array);

extern void value_free(Values* array);

extern void value_print(Value value, bool newline);

#endif // !VALUE_H