#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

void noAssertForever() {
    std::atomic<int> x;
    std::atomic<int> y;
    std::thread t_store(
        [&x, &y]
        {
            x.store(1);
            y.store(2);
        });
    std::thread t_load(
        [&x, &y]
        {
            if (y.load() == 2) {
                assert(x.load() == 1);
            }
        });

    t_store.join();
    t_load.join();

    std::cout << "Run noAssertForever Success" << std::endl;
}

void infinityLoop() {
    int x_prev = 1;
    int x_next = -1;
    int y_prev = 2;
    int y_next = -2;
    std::atomic<int> x{x_prev};
    std::atomic<int> y{y_prev};

    std::thread t1(
        [&]
        {
            y.store(y_next);
            while (x.load() == x_prev) {
                std::cout << "loop t1" << std::endl;
            }
        });
    std::thread t2(
        [&]
        {
            while (y.load() == y_prev) {
                std::cout << "loop t2" << std::endl;
            }
            x.store(x_next);
        });

    t1.join();
    t2.join();

    std::cout << "Run infinityLoop Success" << std::endl;
}

int main() {
    noAssertForever();
    infinityLoop();
    return 0;
}
