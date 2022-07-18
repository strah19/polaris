#ifndef VALUE_H
#define VALUE_H

#include "mem.h"

//typedef double Value;

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
} ValueType;

typedef struct {
    ValueType type;

    union {
        int   int_value;
        float float_value;
    };
} Value;

#define INT_VALUE(value)   ((Value) { TYPE_INT, {.int_value = value }})
#define FLOAT_VALUE(value) ((Value) { TYPE_FLOAT, {.float_value = value }})

#define AS_INT(value) value.int_value
#define AS_FLOAT(value) value.float_value

#define IS_INT(value)   (value.type == TYPE_INT)
#define IS_FLOAT(value) (value.type == TYPE_FLOAT)
#define IS_NUMBER(value) (value.type == TYPE_FLOAT || value.type == TYPE_INT)

typedef struct {
    int    capacity;
    int    count;
    Value* values;
} Values;

extern void value_init(Values* array);

extern void value_write(Value value, Values* array);

extern void value_free(Values* array);

extern void value_print(Value value);

#endif // !VALUE_H