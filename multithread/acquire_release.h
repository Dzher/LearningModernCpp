#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

void implyRiskMethod1() {
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

    assert(result != 0);  // maybe assert?
    std::cout << "result value is: " << result << std::endl;
}

void implyRiskMethod2(std::memory_order order = std::memory_order_seq_cst) {
    std::atomic_int x{0};
    std::atomic_int y{0};

    std::thread t1(
        [&]
        {
            y.store(20, order);  //1
            x.store(10, order);  //2
        });
    std::thread t2(
        [&]
        {
            if (x.load(order) == 10) {        //3
                assert(y.load(order) == 20);  //4
                y.store(10, order);           //5
            }
        });
    std::thread t3(
        [&]
        {
            if (y.load(order) == 10) {        //6
                assert(x.load(order) == 10);  //7
            }
        });

    t1.join();
    t2.join();
    t3.join();

    std::string memory_order = order == std::memory_order_seq_cst ? "seq_cst" : "relax";
    std::cout << "Run success with order: " << memory_order << std::endl;
}

void lookLikeDangerButSafe() {
    std::atomic_int x{0};
    std::atomic_int x1, x2;

    std::thread thread_set(
        [&]
        {
            x.store(1, std::memory_order_relaxed);
            x.store(2, std::memory_order_relaxed);
        });
    std::thread thread_get(
        [&]
        {
            x1 = x.load(std::memory_order_relaxed);
            x2 = x.load(std::memory_order_relaxed);
            assert(x1 <= x2);
        });

    thread_set.join();
    thread_get.join();

    std::cout << "x1: " << x1 << std::endl;
    std::cout << "x2: " << x2 << std::endl;
    std::cout << std::endl;
}

void ProductorCustomer() {
    std::atomic_bool blocker{false};
    std::string str;
    int value = 0;

    std::thread productor(
        [&]
        {
            str = "hello";
            value = 10;
            blocker.store(true, std::memory_order_release);  //guarantee the above process before this line
        });
    std::thread customer(
        [&]
        {
            blocker.load(std::memory_order_acquire);  //guarantee the below process after this line
            assert(str == "hello");
            assert(value == 10);
        });

    productor.join();
    customer.join();

    std::cout << "Run ProductorCustomer Successful" << std::endl;
}

void threadFence() {
    std::string str;
    int value = 0;

    std::thread productor(
        [&]
        {
            str = "hello";
            value = 10;
            std::atomic_thread_fence(std::memory_order_release);  //guarantee the above process before this line
        });
    std::thread customer(
        [&]
        {
            std::atomic_thread_fence(std::memory_order_acquire);  //guarantee the below process after this line
            assert(str == "hello");
            assert(value == 10);
        });

    productor.join();
    customer.join();

    std::cout << "Run threadFence Successful" << std::endl;
}

int main() {
    // implyRiskMethod1();

    // implyRiskMethod2();

    // assert failed at 4: 2->3->4->assert or 2->3->5->4->assert
    // assert failed at 7: I dont know how it happen?
    // AI Say: t2 set y=10, but in t3 only async y no async x, so in t3 x still is 0
    // implyRiskMethod2(std::memory_order_relaxed);

    lookLikeDangerButSafe();
    // ProductorCustomer();
    // threadFence();

    return 0;
}