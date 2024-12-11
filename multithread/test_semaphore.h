#include <windows.h>
#include <array>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <new>
#include <random>
#include <semaphore>
#include <thread>
#include <vector>

using namespace std::literals;

unsigned int random() {
    static std::uniform_int_distribution<unsigned int> distribution{2U, 9U};  // [delays]
    static std::random_device engine;
    static std::mt19937 noise{engine()};
    return distribution(noise);
}

// std::hardware_destructive_interference_size just support upper clang version > 19.0
class alignas(64) Guide
{
public:
    static void initStartTime() {
        started_time_ = std::chrono::high_resolution_clock::now();
    }

    void initDelay() const {
        std::this_thread::sleep_for(delay_ * kTimeTick);
    }

    void occupySema() {
        wait_on_sema_ = static_cast<unsigned int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() -
                                                                  started_time_ - delay_ * kTimeTick)
                .count() /
            kTimeTick.count());
        std::this_thread::sleep_for(occupy_ * kTimeTick);
    }

    void visualize(unsigned int id) const {
        auto kPrintNStr = [=](auto str, const unsigned int n)
        {
            for (unsigned int count = n; count > 0; count--) {
                std::cout << str;
            }
        };
        std::lock_guard lk{cout_mtx_};
        std::cout << '#' << std::setw(2) << id << ' ';
        kPrintNStr("░", delay_);
        kPrintNStr("▒", wait_on_sema_);
        kPrintNStr("█", occupy_);
        std::cout << '\n';
    }

private:
    inline static std::mutex cout_mtx_;
    inline static std::chrono::time_point<std::chrono::high_resolution_clock> started_time_;
    unsigned int delay_{random()};
    unsigned int occupy_{random()};
    unsigned int wait_on_sema_{};
    static constexpr auto kTimeTick{100ms};
};

constexpr std::size_t kMaxThreadsCount{10U};  // change and see the effect
constexpr std::ptrdiff_t kMaxSemaCount{3};
std::counting_semaphore semaphore{kMaxSemaCount};

std::array<Guide, kMaxThreadsCount> guides;

void workerThread(unsigned int id) {
    guides[id].initDelay();   // emulate some work before sema acquisition
    semaphore.acquire();      // wait until a free sema slot is available
    guides[id].occupySema();  // emulate some work while sema is acquired
    semaphore.release();
    guides[id].visualize(id);
}

int main() {
    // make correct output
    SetConsoleOutputCP(CP_UTF8);

    std::vector<std::thread> threads;
    threads.reserve(kMaxThreadsCount);

    std::cout << "\nThreads: " << kMaxThreadsCount << ", Throughput: " << kMaxSemaCount << std::endl
              << "labels:" << std::endl
              << "initial delay ░" << std::endl
              << "wait state ▒" << std::endl
              << "sema occupation █" << std::endl;
    std::cout << std::endl;

    Guide::initStartTime();

    for (auto id{0U}; id < kMaxThreadsCount; ++id) {
        auto thread = std::thread(workerThread, id);
        if (thread.joinable()) {
            thread.join();
        }
        threads.push_back(std::move(thread));
    }
}