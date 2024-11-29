#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable can_print_1_cv;
std::condition_variable can_print_2_cv;
unsigned int current_printed_num = 0;

void alternativePrintNum() {
    std::thread print_1_thread(
        []()
        {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx);
                can_print_1_cv.wait(lock, []() { return current_printed_num == 2; });
                std::cout << "Thread 1 Print 1" << std::endl;
                current_printed_num = 1;
                can_print_2_cv.notify_one();
            }
        });
    std::thread print_2_thread(
        []()
        {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx);
                can_print_2_cv.wait(lock, []() { return current_printed_num == 1; });
                std::cout << "Thread 2 Print 2" << std::endl;
                current_printed_num = 2;
                can_print_1_cv.notify_one();
            }
        });

    print_1_thread.join();
    print_2_thread.join();
}

int main() {
    current_printed_num = 1;
    alternativePrintNum();
    return 0;
}
