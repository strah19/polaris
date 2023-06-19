/**
 * Copyright (C) 2023 Strahinja Marinkovic - All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the MIT License along with
 * this program. If not, see https://opensource.org/license/mit/
 */

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