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

/**
 * @brief Initializes an array of values.
 * 
 * @param array 
 */
void value_init(Values* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

/**
 * @brief Writes a value into the array.
 * 
 * @param value 
 * @param array 
 */
void value_write(Value value, Values* array) {
    if (array->capacity < array->count + 1) {
        array->capacity = NEW_CAPACITY(array->capacity);
        array->values = REALLOC(Value, array->values, array->capacity);
    }

    array->values[array->count++] = value;
}

/**
 * @brief Frees up the array.
 * 
 * @param array 
 */
void value_free(Values* array) {
    FREE(Value, array->values);
    value_init(array);
}

/**
 * @brief Prints the value to stdout.
 * 
 * @param value 
 */
void value_print(Value value) {
    switch (value.type) {
    case TYPE_FLOAT: printf("%g", AS_FLOAT(value)); break;
    case TYPE_INT:   printf("%d", AS_INT(value)); break;
    }
}