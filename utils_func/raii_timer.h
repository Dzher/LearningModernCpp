#ifndef _UTILS_FUNC_RAII_TIMER_
#define _UTILS_FUNC_RAII_TIMER_

#include <chrono>
#include <iostream>

namespace utils
{
#define RAII_TIMER RAIITimer(__func__)
class RAIITimer
{
    explicit RAIITimer(const char* name) : func_name_(name), start_time_(std::chrono::steady_clock::now()) {
        std::cout << "Timer Start..." << std::endl;
    }
    
    ~RAIITimer() {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = end_time - start_time_;
        std::cout << "Timer End, function: " << func_name_ << " cost: " << duration.count() << std::endl;
    }

private:
    const char* func_name_;
    std::chrono::steady_clock::time_point start_time_;
};
}  // namespace utils
#endif
