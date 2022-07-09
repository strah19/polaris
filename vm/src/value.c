#include "value.h"

void init_value_array(ValueArray* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void write_value_array(Value value, ValueArray* array) {
    if (array->capacity < array->count + 1) {
        array->capacity = NEW_CAPACITY(array->capacity);
        array->values = REALLOC(Value, array->values, array->capacity);
    }

    array->values[array->count++] = value;
}

void free_value_array(ValueArray* array) {
    FREE(Value, array->values);
    init_value_array(array);
}

void print_value(Value value) {
    printf("%g", value);
}