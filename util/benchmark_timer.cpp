#include "benchmark_timer.hpp"

void Timer::start() {
    start_time = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
    end_time = std::chrono::high_resolution_clock::now();
}

double Timer::elapsed_seconds() const {
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    return duration.count();
}

// Returns the elapsed time in milliseconds
double Timer::elapsed_milliseconds() const {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    return duration.count() / 1000.0;
}

// Returns the elapsed time in microseconds
double Timer::elapsed_microseconds() const {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    return duration.count();
}

