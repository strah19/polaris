/**
* @file main.c
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
* Main function of Polaris!
*/

#include "config.h"
#include <stdio.h>

// For VM
#ifdef USE_VM
#include "vm.h"
#include "opcodes.h"
#endif // !USE_VM

int main(int argc, char* argv[]) {
    printf("Polaris %d.%d\n", POLARIS_VERSION_MAJOR, POLARIS_VERSION_MINOR);
    return 0;
}
