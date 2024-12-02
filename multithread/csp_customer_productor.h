
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

template <typename T>
class CSPChannel
{
public:
    explicit CSPChannel(std::size_t capacity) : capacity_(capacity), closed_(false) {
    }

    ~CSPChannel() {
        close();
    }

    bool send(T value) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_productor_.wait(lock, [this]() { return capacity_ > queue_.size() || closed_; });

        if (closed_) {
            return false;
        }

        queue_.push(value);
        cv_customer_.notify_one();

        return true;
    }

    bool receive(T& value) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_customer_.wait(lock, [this]() { return !queue_.empty() || closed_; });

        if (closed_) {
            return false;
        }

        value = queue_.front();
        queue_.pop();
        cv_productor_.notify_one();

        return true;
    }

    void close() {
        std::unique_lock<std::mutex> lock(mtx_);

        closed_ = true;
        cv_productor_.notify_all();
        cv_customer_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::size_t capacity_;
    std::condition_variable cv_productor_;
    std::condition_variable cv_customer_;
    std::mutex mtx_;
    bool closed_;
};

int main() {
    CSPChannel<std::string> channel(5);

    // modify the time each spend as you want.
    int product_cost_time = 1;
    int custom_cost_time = 2;

    std::thread productor(
        [&channel, product_cost_time]
        {
            int index = 1;
            while (index <= 10) {
                std::this_thread::sleep_for(std::chrono::seconds(product_cost_time));
                std::cout << "send message: " << index << std::endl;
                std::string value = "the " + std::to_string(index) + " message";
                channel.send(value);
                ++index;
            }
            channel.close();
        });

    std::thread customer(
        [&channel, custom_cost_time]
        {
            std::string value;
            while (channel.receive(value)) {
                std::cout << "receive message: " << value << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(custom_cost_time));
            }
        });

    productor.join();
    customer.join();

    return 0;
}