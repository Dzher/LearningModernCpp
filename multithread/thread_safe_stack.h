#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <optional>
#include <stack>
#include <thread>

struct empty_stack_error : std::exception
{
    [[nodiscard]] const char* what() const noexcept override {
        return "Error: Stack is Empty!!!";
    }
};

template <typename T>
class thread_safe_stack
{
public:
    thread_safe_stack() = default;
    thread_safe_stack(const thread_safe_stack& other) {
        std::lock_guard<std::mutex> lock(other.mtx_);
        data_ = std::move(other.data_);
    }
    thread_safe_stack& operator=(const thread_safe_stack& other) = delete;

public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx_);
        data_.push(value);
        cv_.notify_one();
    }

    T pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (data_.empty()) {
            throw empty_stack_error();
        }
        auto value = std::move(data_.top());
        data_.pop();
        return value;
    }

    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (data_.empty()) {
            return std::nullopt;
        }
        auto value = std::move(data_.top());
        data_.pop();
        return value;
    }

    T wait_and_pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !data_.empty(); });
        auto value = std::move(data_.top());
        data_.pop();
        return value;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_.empty();
    }

private:
    std::stack<T> data_;
    std::mutex mtx_;
    std::condition_variable cv_;
};

int main() {
    thread_safe_stack<int> stack;
    std::mutex print_mtx;

    std::thread t_push(
        [&stack, &print_mtx]
        {
            int count = 0;
            while (count < 30) {
                stack.push(++count);
                {
                    std::lock_guard lock(print_mtx);
                    std::cout << "Pushed " << count << std::endl;
                }
            }
        });
    std::thread t_pop(
        [&stack, &print_mtx]
        {
            while (true) {
                auto value = stack.wait_and_pop();
                {
                    std::lock_guard lock(print_mtx);
                    std::cout << "Poped " << value << std::endl;
                }
                if (stack.empty()) {
                    return;
                }
            }
        });

    t_push.join();
    t_pop.join();

    return 0;
}