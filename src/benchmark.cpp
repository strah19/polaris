/**
* @file benchmark.cpp
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
#include <stdio.h>

Benchmark::Benchmark(const char* name) : name(name) {
    startpoint = std::chrono::high_resolution_clock::now();
    stopped = false;
}

Benchmark::~Benchmark() {
    if (!stopped)
        stop();
}

void Benchmark::stop() {
    auto endpoint =
        std::chrono::high_resolution_clock::now();

    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startpoint)
                    .time_since_epoch()
                    .count();

    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endpoint)
                    .time_since_epoch()
                    .count();

    auto duration = end - start;
    auto ms = duration * 0.001;

    printf("%s: %gms", name, ms);
    stopped = true;
}