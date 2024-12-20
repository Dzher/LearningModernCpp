#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <iterator>
#include <thread>
#include <vector>

static constexpr int kMinPerThreadCount = 25;  // must set a suitable value or the efficiency will be bad
static const unsigned long kHardwareThreadCount = std::thread::hardware_concurrency();

template <typename Iterator, typename MatchType>
Iterator paraller_find(Iterator first, Iterator last, MatchType type) {

    const auto findMatchType =
        [](Iterator first, Iterator last, MatchType type, std::promise<Iterator>& result, std::atomic_bool& has_found)
    {
        for (auto it = first; it != last && !has_found.load(); ++it) {
            if (*it == type) {
                result.set_value(it);
                has_found.store(true);
                return;
            }
        }
    };

    const unsigned long find_num = std::distance(first, last);
    if (find_num == 0) {
        return last;
    }
    const unsigned long need_thread_num = (find_num - 1) / kMinPerThreadCount + 1;
    const unsigned long real_thread_num = std::min(kHardwareThreadCount, need_thread_num);
    const unsigned long each_thread_task_num = find_num / real_thread_num;
    const unsigned long no_main_thread_num = real_thread_num - 1;

    // other threads
    std::promise<Iterator> result;
    std::atomic_bool has_found{false};
    Iterator thread_start_itor = first;
    for (auto i = 0; i < no_main_thread_num; ++i) {
        Iterator thread_end_itor = thread_start_itor;
        std::advance(thread_end_itor, each_thread_task_num);

        auto future = std::async([&] { findMatchType(thread_start_itor, thread_end_itor, type, result, has_found); });
        future.get();
        thread_start_itor = thread_end_itor;
    }
    // main thread
    findMatchType(thread_start_itor, last, type, result, has_found);

    if (!has_found.load()) {
        return last;
    }

    return result.get_future().get();
}

int main() {
    int task_num = 50000000;
    std::vector<int> funcs(task_num, 0);
    int correct_index = 2222222;
    int find_value = 1;
    funcs[correct_index] = find_value;

    auto start = std::chrono::high_resolution_clock::now();
    auto index = paraller_find(funcs.begin(), funcs.end(), find_value);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Found position is " << std::distance(funcs.begin(), index) << std::endl;
    std::cout << "DIY Parallel Find Elapsed Time: " << duration.count() << " us\n";

    start = std::chrono::high_resolution_clock::now();
    index = std::find(funcs.begin(), funcs.end(), find_value);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Found position is " << std::distance(funcs.begin(), index) << std::endl;
    std::cout << "Std::for_each Elapsed Time: " << duration.count() << " us\n";
}