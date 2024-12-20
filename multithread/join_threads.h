#include <iostream>
// #include <syncstream>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>

class join_threads
{
public:
    join_threads() = default;
    explicit join_threads(std::vector<std::thread> threads) : join_thread_list_(std::move(threads)) {
    }

    ~join_threads() {
        for (auto& each : join_thread_list_) {
            if (each.joinable()) {
                each.join();
            }
        }
    }

    void push_back(std::thread&& thread) {
        join_thread_list_.push_back(std::move(thread));
    }

    std::thread& operator[](const unsigned int index) {
        return join_thread_list_[index];
    }

private:
    std::vector<std::thread> join_thread_list_;
};

void printValue(int value) {
    // std::osyncstream sync_cout(std::cout);
    // sync_cout << "Thread " << std::this_thread::get_id() << " value is: " << value << std::endl;
    std::ostringstream data;
    data << "Thread " << std::this_thread::get_id() << " value is: " << value << std::endl;
    std::cout << data.str();
}

int main() {
    join_threads jthreads;
    for (int count = 0; count < 20; ++count) {
        std::thread t(printValue, count);
        jthreads.push_back(std::move(t));
    }
    std::cout << "Exec Successful" << std::endl;
}