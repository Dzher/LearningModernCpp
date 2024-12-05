#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

void implyRiskMethod() {
    std::atomic<int> result = 0;

    std::atomic<bool> x{false}, y{false};

    std::thread write_x([&] { x.store(true, std::memory_order_release); });
    std::thread write_y([&] { y.store(true, std::memory_order_release); });
    std::thread read_x_then_y(
        [&]
        {
            while (!x.load(std::memory_order_acquire)) {
            }
            if (y.load(std::memory_order_acquire)) {
                ++result;
            }
        });
    std::thread read_y_then_x(
        [&]
        {
            while (!y.load(std::memory_order_acquire)) {
            }
            if (x.load(std::memory_order_acquire)) {
                ++result;
            }
        });

    write_x.join();
    read_x_then_y.join();
    write_y.join();
    read_y_then_x.join();

    assert(result != 0);  // maybe assert
    std::cout << "result value is: " << result << std::endl;
}

int main() {
    implyRiskMethod();

    return 0;
}