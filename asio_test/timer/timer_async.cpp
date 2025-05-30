#include <functional>
#include <iostream>
#include <system_error>
#include "asio/detail/chrono.hpp"
#include "asio/io_context.hpp"
#include "asio/placeholders.hpp"
#include "asio/steady_timer.hpp"

void printWithParams(const std::error_code&, asio::steady_timer* timer, int& count) {
    if (count < 5) {
        std::cout << "Count is " << count++ << std::endl;
        timer->expires_at(timer->expiry() + asio::chrono::seconds(1));
        timer->async_wait(std::bind(printWithParams, asio::placeholders::error, timer, count));
    }
    else {
        std::cout << "Method 2 Finished" << std::endl;
        std::cout << std::endl;
    }
}

class Printer
{
public:
    explicit Printer(asio::io_context& io) : timer_(io, asio::chrono::seconds(1)), count_(0) {
        timer_.async_wait(std::bind(&Printer::print, this));
    }
    ~Printer() {
        std::cout << "Method 3 Finished" << std::endl;
        std::cout << std::endl;
    }

    void print() {
        if (count_ < 5) {
            std::cout << "Count is " << count_++ << std::endl;
            timer_.expires_at(timer_.expiry() + asio::chrono::seconds(1));
            timer_.async_wait(std::bind(&Printer::print, this));
        }
    }

private:
    asio::steady_timer timer_;
    int count_;
};

int main() {
    // method 1
    asio::io_context io;
    asio::steady_timer t{io, asio::chrono::seconds{5}};

    auto print = [](const std::error_code&)
    {
        std::cout << "Method 1 Finished" << std::endl;
        std::cout << std::endl;
    };

    std::cout << "Start method 1..." << std::endl;
    t.async_wait(print);
    std::cout << "Wait for method 1..." << std::endl;
    io.run();

    // method 2
    std::cout << "Start method 2..." << std::endl;
    asio::io_context io1;
    int count = 0;
    asio::steady_timer timer{io1, asio::chrono::seconds(1)};
    timer.async_wait(std::bind(printWithParams, asio::placeholders::error, &timer, count));
    std::cout << "Wait for method 2..." << std::endl;
    io1.run();

    // method 3
    std::cout << "Start method 3..." << std::endl;
    asio::io_context io2;
    Printer printer(io2);
    std::cout << "Wait for method 3..." << std::endl;
    io2.run();
    return 0;
}