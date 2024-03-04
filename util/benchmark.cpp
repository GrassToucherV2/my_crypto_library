#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>  // For std::accumulate
#include <cmath>    // For std::sqrt and std::pow
#include <algorithm> // For std::sort
#include "benchmark_timer.hpp"

extern "C"{
    #include "bigint.h"
}

#define NUM_ITERATION_TIMER_OVERHEAD 10000
#define NUM_ITERATIUN_WARM_UP 500
#define NUM_ITERATION_BENCHMARK 100000

// Meausure the timer's own overhead and return the average overhead
double measure_overhead() {
    Timer timer;
    double total_overhead = 0;

    for (int i = 0; i < NUM_ITERATION_TIMER_OVERHEAD; ++i) {
        timer.start();
        timer.stop();
        total_overhead += timer.elapsed_microseconds();
    }

    return total_overhead / NUM_ITERATION_TIMER_OVERHEAD;
}

double sum(std::vector<double> &timings){
    return std::accumulate(timings.begin(), timings.end(), 0.0);
}

double mean(double sum){
    return sum / NUM_ITERATION_BENCHMARK;
}

double std_dev(std::vector<double> &timings, double mean){
    double sum = 0.0;
    for(auto timing : timings){
        sum += std::pow(timing - mean, 2);
    }
    double variance = sum / timings.size();
    return std::sqrt(variance);
}

void print_stats(std::vector<double> &timings, char *str){
    double timer_overhead = measure_overhead();
    double total_ms = sum(timings);
    double avg = mean(total_ms);
    std::cout << str << std::endl;
    std::cout << "Total elapsed time     : " << total_ms << " ms" << std::endl;
    std::cout << "Timer overhead         : " << timer_overhead << " us" << std::endl;
    std::cout << "Average time per call  : " << avg << " ms" << std::endl;
    std::cout << "Standard Deviation     : " << std_dev(timings, avg) << " ms" << std::endl;
    std::cout << std::endl;
}

void bigint_add_benchmark(){
    unsigned char dataa[] = {
        0x79, 0xEE, 0x36, 0xD7, 0xBC, 0x26, 0xB3, 0xBC, 
        0x9A, 0x60, 0x4E, 0x98, 0xEA, 0xEB, 0x7C, 0xBB
    };
    unsigned char datab[] = {
        0x59, 0x25, 0xA9, 0x03, 0x60, 0x40, 0xED, 0x3D, 
        0xCE, 0x91, 0xD2, 0xBB, 0x8B, 0x95, 0x1C, 0xE7
    };

    bigint a, b, c;

    bigint_init(&a, 4);
    bigint_init(&b, 4);
    bigint_init(&c, 5);

    bigint_from_bytes(&a, dataa, 16);
    bigint_from_bytes(&b, datab, 16);

    // this loop is a warm up before the actual benchmarking
    for(int i = 0; i < NUM_ITERATIUN_WARM_UP; i++){
        bigint_add(&a, &b, &c);
    }

    double timer_overhead = measure_overhead();

    Timer timer;
    std::vector<double> timings;
    timings.reserve(NUM_ITERATION_BENCHMARK);

    for(int i = 0; i < NUM_ITERATION_BENCHMARK; i++){
        timer.start();
        bigint_add(&a, &b, &c);
        timer.stop();
        timings.push_back(timer.elapsed_microseconds() - (timer_overhead / 1000.0));
    }
    char str[] = "=================== Statistics for bigint_add =====================\n"; 
    print_stats(timings, str);
    
    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&c);

}

void bigint_mul_benchmark(){
    unsigned char dataa[] = {
        0x79, 0xEE, 0x36, 0xD7, 0xBC, 0x26, 0xB3, 0xBC, 
        0x9A, 0x60, 0x4E, 0x98, 0xEA, 0xEB, 0x7C, 0xBB
    };
    unsigned char datab[] = {
        0x59, 0x25, 0xA9, 0x03, 0x60, 0x40, 0xED, 0x3D, 
        0xCE, 0x91, 0xD2, 0xBB, 0x8B, 0x95, 0x1C, 0xE7
    };

    bigint a, b, c;

    bigint_init(&a, 4);
    bigint_init(&b, 4);
    bigint_init(&c, 8);

    bigint_from_bytes(&a, dataa, 16);
    bigint_from_bytes(&b, datab, 16);

    // this loop is a warm up before the actual benchmarking
    for(int i = 0; i < NUM_ITERATIUN_WARM_UP; i++){
        bigint_mul(&a, &b, &c);
    }

    double timer_overhead = measure_overhead();

    Timer timer;
    std::vector<double> timings;
    timings.reserve(NUM_ITERATION_BENCHMARK);

    for(int i = 0; i < NUM_ITERATION_BENCHMARK; i++){
        timer.start();
        bigint_mul(&a, &b, &c);
        timer.stop();
        timings.push_back(timer.elapsed_microseconds() - (timer_overhead / 1000.0));
    }
    char str[] = "=================== Statistics for bigint_mul =====================\n"; 
    print_stats(timings, str);

    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&c);

}

void bigint_mul_karatsuba_benchmark(){
    unsigned char dataa[] = {
        0x79, 0xEE, 0x36, 0xD7, 0xBC, 0x26, 0xB3, 0xBC, 
        0x9A, 0x60, 0x4E, 0x98, 0xEA, 0xEB, 0x7C, 0xBB
    };
    unsigned char datab[] = {
        0x59, 0x25, 0xA9, 0x03, 0x60, 0x40, 0xED, 0x3D, 
        0xCE, 0x91, 0xD2, 0xBB, 0x8B, 0x95, 0x1C, 0xE7
    };

    bigint a, b, c;

    bigint_init(&a, 4);
    bigint_init(&b, 4);
    bigint_init(&c, 8);

    bigint_from_bytes(&a, dataa, 16);
    bigint_from_bytes(&b, datab, 16);

    // this loop is a warm up before the actual benchmarking
    for(int i = 0; i < NUM_ITERATIUN_WARM_UP; i++){
        bigint_mul_karatsuba(&a, &b, &c);
    }

    double timer_overhead = measure_overhead();

    Timer timer;
    std::vector<double> timings;
    timings.reserve(NUM_ITERATION_BENCHMARK);

    for(int i = 0; i < NUM_ITERATION_BENCHMARK; i++){
        timer.start();
        bigint_mul_karatsuba(&a, &b, &c);
        timer.stop();
        timings.push_back(timer.elapsed_microseconds() - (timer_overhead / 1000.0));
    }
    char str[] = "=================== Statistics for bigint_mul_karatsuba =====================\n"; 
    print_stats(timings, str);

    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&c);

}

void bigint_div_benchmark(){
    unsigned char a3[] = {
        0x40, 0x8B, 0x01, 0x51, 0x8C, 0xE4, 0xCE, 0x8D,
        0xED, 0x19, 0x6F, 0xFC, 0x94, 0x2B, 0x76, 0x1F
    };

    unsigned char b3[] = {
        0x15, 0x5C, 0x50, 0xD9, 0x97, 0x8E, 0x1A, 0x28
    };

    bigint a, b, q, r;

    bigint_init(&a, 4);
    bigint_init(&b, 2);
    bigint_init(&q, 3);
    bigint_init(&r, 2);

    bigint_from_bytes(&a, a3, 16);
    bigint_from_bytes(&b, b3, 8);

    for(int i = 0; i < NUM_ITERATIUN_WARM_UP; i++){
        bigint_div(&a, &b, &q, &r);
    }

    double timer_overhead = measure_overhead();

    Timer timer;
    std::vector<double> timings;
    timings.reserve(NUM_ITERATION_BENCHMARK);

    for(int i = 0; i < NUM_ITERATION_BENCHMARK; i++){
        timer.start();
        bigint_div(&a, &b, &q, &r);
        timer.stop();
        timings.push_back(timer.elapsed_microseconds() - (timer_overhead / 1000.0));
    }
    char str[] = "=================== Statistics for bigint_div =====================\n"; 
    print_stats(timings, str);

    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&q);
    bigint_free(&r);
}

int main(){
    // bigint_add_benchmark();
    bigint_mul_benchmark();
    bigint_mul_karatsuba_benchmark();
    bigint_div_benchmark();
    return 0;
}

