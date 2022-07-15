/**
* @file compiler.c
* @author strah19
* @date July 15, 2022
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
* Used to check speed of compiler.
*/

#include "benchmark.h"
#include <time.h>
#include <stdio.h>

static const char* benchmark_name;
static clock_t start_time;

/**
 * @brief Place at the beginning of an operation and will start a clock.
 * 
 * @param name 
 */
void begin_benchmark(const char* name) {
    benchmark_name = name;
    printf("Beginning '%s' benchmark.\n", name);
    start_time = clock();
}

/**
 * @brief Will stop the benchmark and print how long it lasted.
 * 
 */
void stop_benchmark() {
  double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
  printf("Benchmark '%s' done in %f seconds\n", benchmark_name, elapsed_time);
}   