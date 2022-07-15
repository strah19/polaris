#include "benchmark.h"
#include <time.h>
#include <stdio.h>

static const char* benchmark_name;
static clock_t start_time;

void begin_benchmark(const char* name) {
    benchmark_name = name;
    printf("Beginning '%s' benchmark.\n", name);
    start_time = clock();
}

void stop_benchmark() {
  double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
  printf("Benchmark '%s' done in %f seconds\n", benchmark_name, elapsed_time);
}   