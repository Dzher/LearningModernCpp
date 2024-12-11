#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

int assert_count = 0;

std::atomic<int> res{0};
std::atomic<bool> x{false}, y{false};

void write_x() {
    x.store(true, std::memory_order_release);
}

void write_y() {
    y.store(true, std::memory_order_release);
}

void read_x_then_y() {
    while (!x.load(std::memory_order_acquire)) {
    }
    if (y.load(std::memory_order_acquire)) {
        res++;
    }
}

void read_y_then_x() {
    while (!y.load(std::memory_order_acquire)) {
    }
    if (x.load(std::memory_order_acquire)) {
        res++;
    }
}

// for this demo1, I think the assert will not happen forever, even change all memory order to relaxed
// So, I will test in demo2
void demo1() {
    res = 0;
    x = false;
    y = false;
    std::thread d(read_y_then_x);
    std::thread c(read_x_then_y);
    std::thread a(write_x);
    std::thread b(write_y);
    a.join();
    b.join();
    c.join();
    d.join();

    if (res.load() == 0) {
        std::cout << "Assert!!! " << assert_count << " times" << std::endl;
        return;
    }

    // std::cout << "res value is " << res.load() << std::endl;
}

bool a, b;
int r = 0;

void write_a() {
    a = true;
}

void write_b() {
    b = true;
}

void read_a_then_b() {
    while (!a) {
    }
    if (b) {
        r++;
    }
}

void read_b_then_a() {
    while (!b) {
    }
    if (a) {
        r++;
    }
}

void demo2() {
    r = 0;
    a = false;
    b = false;
    std::thread t1(read_b_then_a);
    std::thread t2(read_a_then_b);
    std::thread t3(write_a);
    std::thread t4(write_b);
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    if (r == 0) {
        assert_count++;
        std::cout << "Assert!!! " << assert_count << " times" << std::endl;
        return;
    }
}

int main() {
    int count = 100;
    while (count--) {
        // !demo1 will never assert, and demo2 assert frequenctly
        // demo1();
        demo2();
    }

    return 0;
}