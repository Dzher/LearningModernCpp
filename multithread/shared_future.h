#include <chrono>
#include <future>
#include <iostream>
#include <thread>

void setValue(std::promise<int>&& /*attention must && here*/ promise) {
    std::cout << "Setting the value" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    promise.set_value(1024);
}

void waitValue(const std::shared_future<int>& future) {
    std::cout << "Result is: " << future.get() << std::endl;
}

int main() {
    std::promise<int> the_promise_for_3_threads;
    std::shared_future<int> future = the_promise_for_3_threads.get_future();

    std::thread t0(setValue, std::move(the_promise_for_3_threads));

    std::thread t1(waitValue, future);
    std::thread t2(waitValue, future);
    std::thread t3(waitValue, future);

    t0.join();

    t1.join();
    t2.join();
    t3.join();

    return 0;
}