#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>
#include <stdio.h>

#include "value.h"

struct Bytecode {
    int capacity;
    int count;
    uint8_t* code;
    int* line;
    Values constants;
    struct Bytecode* next;
};

#endif //!BYTECODE_H