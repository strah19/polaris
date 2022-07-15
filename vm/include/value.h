#ifndef VALUE_H
#define VALUE_H

#include "mem.h"

typedef double Value;

typedef struct {
    int capacity;
    int count;
    Value* values;
} Values;

extern void value_init(Values* array);

extern void value_write(Value value, Values* array);

extern void value_free(Values* array);

extern void value_print(Value value);

#endif // !VALUE_H