#ifndef COMPILER_H
#define COMPILER_H

#include "vm.h"
#include <stdbool.h>

extern VMResults compiler_run();

extern bool compiler_compile();

extern void compiler_end_compilation();

#endif // !COMPILER_H