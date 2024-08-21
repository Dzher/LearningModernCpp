#include <iostream>
#include "asio/detail/chrono.hpp"
#include "asio/io_context.hpp"
#include "asio/steady_timer.hpp"

int main()
{
    asio::io_context io;
    asio::steady_timer t{io, asio::chrono::seconds{5}};

    auto print = [](const std::error_code&)
    {
        std::cout << "Timeout" << std::endl;
    };

    t.async_wait(print);

    std::cout << "Do something else..." << std::endl;

    io.run();

    return 0;
}