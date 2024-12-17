#include <future>
#include <iostream>
#include <iterator>
#include <thread>

static constexpr int kMinPerThreadCount = 25;
static const unsigned long kHardwareThreadCount = std::thread::hardware_concurrency();

template <typename Iterator, typename Func>
void paraller_traversal(Iterator first, Iterator last, Func func) {
    const unsigned long traversal_num = std::distance(first, last);
    if (traversal_num == 0) {
        return;
    }
    const unsigned long need_thread_num = (traversal_num - 1) / kMinPerThreadCount + 1;
    const unsigned long real_thread_num = std::min(kHardwareThreadCount, need_thread_num);
    const unsigned long each_thread_task_num = traversal_num / real_thread_num;
    const unsigned long no_main_thread_num = real_thread_num - 1;

    // other threads
    std::vector<std::future<void>> futures(no_main_thread_num);
    Iterator thread_start_itor = first;
    for (auto i = 0; i < no_main_thread_num; ++i) {
        Iterator thread_end_itor = thread_start_itor;
        std::advance(thread_end_itor, each_thread_task_num);

        std::packaged_task<void(void)> task{[=]()
                                            {
                                                std::for_each(thread_start_itor, thread_end_itor, func);
                                            }};
        futures[i] = task.get_future();
        std::thread t{std::move(task)};
        t.join();

        thread_start_itor = thread_end_itor;
    }
    // main thread
    std::for_each(thread_start_itor, last, func);

    for (auto i = 0; i < no_main_thread_num; ++i) {
        futures[i].get();
    }
}

int main() {
    int task_num = 101;
    std::vector<int> funcs(task_num, 1);

    static long value = 0;
    const auto sumValue = [](int each_value)
    {
        value += each_value;
    };

    paraller_traversal(funcs.begin(), funcs.end(), sumValue);
    std::cout << "total value is " << value << std::endl;
}