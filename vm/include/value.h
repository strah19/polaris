#ifndef VALUE_H
#define VALUE_H

#include "mem.h"

typedef double Value;

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

extern void init_value_array(ValueArray* array);

extern void write_value_array(Value value, ValueArray* array);

extern void free_value_array(ValueArray* array);

extern void print_value(Value value);

#endif // !VALUE_H