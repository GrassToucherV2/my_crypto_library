#include <chrono>
#include <iostream>

class Timer {
    public:
        void start();
        void stop();
        double elapsed_seconds() const;
        double elapsed_milliseconds() const;
        double elapsed_microseconds() const;
        
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time, end_time;
};