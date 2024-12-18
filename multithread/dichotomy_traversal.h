#include <algorithm>
#include <future>
#include <iostream>
#include <iterator>

static constexpr int kMinPerThreadCount = 25;  // must set a suitable value or the efficiency will be bad

template <typename Iterator, typename Func>
void dichotomy_traversal(Iterator first, Iterator last, Func func) {
    unsigned long need_thread_num = std::distance(first, last);
    if (first >= last) {
        return;
    }

    if (need_thread_num <= kMinPerThreadCount) {
        std::for_each(first, last, func);
    }
    else {
        Iterator mid = first + need_thread_num / 2;
        std::future<void> first_half = std::async(&dichotomy_traversal<Iterator, Func>, first, mid, func);
        dichotomy_traversal(mid, last, func);
        first_half.get();
    }
}

int main() {
    int task_num = 50000;  // this implement cannot calculate too large number, so just a demo
    std::vector<int> funcs(task_num, 1);

    std::atomic_int value = 0;
    const auto sumValue = [&value](int each_value)
    {
        value += each_value;
    };

    auto start = std::chrono::high_resolution_clock::now();
    dichotomy_traversal(funcs.begin(), funcs.end(), sumValue);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "total value is " << value << std::endl;
    std::cout << "DIY Parallel Elapsed Time: " << duration.count() << " us\n";

    value = 0;
    start = std::chrono::high_resolution_clock::now();
    std::for_each(funcs.begin(), funcs.end(), sumValue);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "total value is " << value << std::endl;
    std::cout << "Std::for_each Elapsed Time: " << duration.count() << " us\n";
}