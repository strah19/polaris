#include "mem.h"

void* reallocate(void* pointer, size_t new_size) {
    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void* new = realloc(pointer, new_size);
    if (!new) exit(1);
    return new;
}