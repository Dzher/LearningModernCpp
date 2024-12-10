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

unsigned random() {
    static std::uniform_int_distribution<unsigned> distribution{2U, 9U};  // [delays]
    static std::random_device engine;
    static std::mt19937 noise{engine()};
    return distribution(noise);
}

// std::hardware_destructive_interference_size just support upper clang version > 19.0
class alignas(64) Guide
{
public:
    static void initStartTime() {
        started_time = std::chrono::high_resolution_clock::now();
    }

    void initial_delay() const {
        std::this_thread::sleep_for(delay * kTimeTick);
    }

    void occupy_sema() {
        wait_on_sema =
            static_cast<unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                      std::chrono::high_resolution_clock::now() - started_time - delay * kTimeTick)
                                      .count() /
                                  kTimeTick.count());
        std::this_thread::sleep_for(occupy * kTimeTick);
    }

    void visualize(unsigned id) const {
        auto kPrintNStr = [=](auto str, const unsigned n)
        {
            for (unsigned count = n; count > 0; count--) {
                std::cout << str;
            }
        };
        std::lock_guard lk{cout_mutex};
        std::cout << '#' << std::setw(2) << id << ' ';
        kPrintNStr("░", delay);
        kPrintNStr("▒", wait_on_sema);
        kPrintNStr("█", occupy);
        std::cout << '\n';
    }

private:
    inline static std::mutex cout_mutex;
    inline static std::chrono::time_point<std::chrono::high_resolution_clock> started_time;
    unsigned delay{random()};
    unsigned occupy{random()};
    unsigned wait_on_sema{};
    static constexpr auto kTimeTick{10ms};
};

constexpr std::size_t kMaxThreadsCount{10U};  // change and see the effect
constexpr std::ptrdiff_t kMaxSemaCount{3};
std::counting_semaphore semaphore{kMaxSemaCount};

std::array<Guide, kMaxThreadsCount> guides;

void workerThread(unsigned id) {
    guides[id].initial_delay();  // emulate some work before sema acquisition
    semaphore.acquire();         // wait until a free sema slot is available
    guides[id].occupy_sema();    // emulate some work while sema is acquired
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