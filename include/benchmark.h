#ifndef BENCHMARK_H
#define BENCHMARK_H

#define MAX_BENCHMARKS 32

extern void begin_benchmark(const char* name);

extern void stop_benchmark();

#endif // !BENCHMARK_H