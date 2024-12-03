#include <atomic>
#include <iostream>
#include <thread>

class SpinLock
{
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }

private:
    // if init flag_ as true, the result will be quite the opposite
    std::atomic_flag flag_{false};  //ATOMIC_FLAG_INIT
};

int main() {
    SpinLock spin_lock;

    std::thread t1(
        [&spin_lock]
        {
            spin_lock.lock();
            for (int index = 0; index < 3; ++index) {
                std::cout << '<' << std::endl;
            }
            spin_lock.unlock();
        });
    std::thread t2(
        [&spin_lock]
        {
            spin_lock.lock();
            for (int index = 0; index < 3; ++index) {
                std::cout << '>' << std::endl;
            }
            spin_lock.unlock();
        });

    t1.join();
    t2.join();

    return 0;
}