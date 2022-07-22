#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>

class Benchmark {
public:
	Benchmark(const char* name);
	~Benchmark();
	void stop();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startpoint;
    const char* name;
    bool stopped = false;
};

#endif // !BENCHMARK_H