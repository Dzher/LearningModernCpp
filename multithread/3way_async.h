#include <chrono>
#include <future>
#include <iostream>
#include <thread>

void test_promise_and_future(int value) {
    std::cout << "begin test_promise_and_future" << std::endl;

    std::promise<int> promise;
    std::future<int> future = promise.get_future();

    std::thread t(
        [&promise](int v)
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            promise.set_value(v);
        },
        value);
    t.join();

    std::cout << "get test_promise_and_future: " << future.get() << std::endl;
}

void test_package_task_and_future(int value) {
    std::cout << "begin test_package_task_and_future" << std::endl;
    const auto setValue = [](int v)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return v;
    };

    std::packaged_task<int(int)> packaged_task(setValue);
    std::future<int> future = packaged_task.get_future();

    std::thread t(std::move(packaged_task), value);
    t.join();

    std::cout << "get test_package_task_and_future: " << future.get() << std::endl;
}

void test_async_and_future(int value) {
    std::cout << "begin test_async_and_future" << std::endl;
    const auto setValue = [](int v)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return v;
    };

    std::future<int> future = std::async(std::launch::async, setValue, value);

    std::cout << "get test_async_and_future: " << future.get() << std::endl;
}

int main() {
    test_promise_and_future(1);
    std::cout << std::endl;
    test_package_task_and_future(2);
    std::cout << std::endl;
    test_async_and_future(3);
    return 0;
}