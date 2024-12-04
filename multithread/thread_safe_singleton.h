#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

class Singleton
{
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // !imply thread safe issue
    // static Singleton* getInstance() {
    //     if (instance_ != nullptr) {
    //         return instance_;
    //     }

    //     mtx_.lock();

    //     if (instance_ != nullptr) {
    //         mtx_.unlock();
    //         return instance_;
    //     }

    //     instance_ = new Singleton();
    //     mtx_.unlock();
    //     return instance_;
    // }

    static Singleton* getInstance() {
        if (atm_.load(std::memory_order_acquire)) {
            return instance_;
        }

        mtx_.lock();

        if (atm_.load(std::memory_order_relaxed)) {
            mtx_.unlock();
            return instance_;
        }

        instance_ = new Singleton();
        atm_.store(true, std::memory_order_release);
        mtx_.unlock();
        return instance_;
    }

private:
    Singleton() = default;

private:
    static Singleton* instance_;
    static std::mutex mtx_;
    static std::atomic<bool> atm_;
};

Singleton* Singleton::instance_ = nullptr;
std::mutex Singleton::mtx_;
std::atomic<bool> Singleton::atm_{false};

int main() {
    std::thread t1([] { std::cout << "Thread1 get the instance address: " << Singleton::getInstance() << std::endl; });
    std::thread t2([] { std::cout << "Thread2 get the instance address: " << Singleton::getInstance() << std::endl; });

    t1.join();
    t2.join();

    return 0;
}