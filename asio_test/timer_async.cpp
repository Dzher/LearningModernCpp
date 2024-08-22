#include <functional>
#include <iostream>
#include "asio.hpp"
#include "asio/detail/chrono.hpp"
#include "asio/io_context.hpp"
#include "asio/steady_timer.hpp"

void printWithParams(const std::error_code&, asio::steady_timer* timer, int& count)
{
    if (count < 5)
    {
        std::cout << "count is " << count++ << std::endl;
        timer->expires_at(timer->expiry() + asio::chrono::seconds(1));
        timer->async_wait(std::bind(printWithParams, asio::placeholders::error, timer, count));
    }
    else
    {
        std::cout << "Finished" << std::endl;
    }
}

int main()
{
    asio::io_context io;
    asio::steady_timer t{io, asio::chrono::seconds{5}};

    auto print = [](const std::error_code&)
    {
        std::cout << "5s Timeout" << std::endl;
    };

    std::cout << "Start..." << std::endl;
    t.async_wait(print);
    std::cout << "Do something else..." << std::endl;
    io.run();

    std::cout << "Continue..." << std::endl;
    asio::io_context io1;
    int count = 0;
    asio::steady_timer timer{io1, asio::chrono::seconds(1)};
    timer.async_wait(std::bind(printWithParams, asio::placeholders::error, &timer, count));
    io1.run();

    return 0;
}