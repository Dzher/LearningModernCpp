#include <windows.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

/*
There are 6 kind of memory orders defined in C++, and we can use these 6 memory orders to implement 3 memory models:
    Sequence Consistent
    Acquire-Release
    Relax
We use characters init from different threads to print "鸡你太美" to test the 6 memory orders
*/

std::atomic<bool> atm_ikun1, atm_ikun2, atm_ikun3, atm_ikun4;

// Sequence Consistent by memory_order_seq_cst
void ikunJiNi(std::memory_order order) {
    atm_ikun1.store(true, order);
    atm_ikun2.store(true, order);
}

void ikunTaiMei(std::memory_order order) {
    atm_ikun3.store(true, order);
    atm_ikun4.store(true, order);
}

void iKunSpeaker1(std::memory_order order) {
    if (atm_ikun1.load(order)) {
        std::cout << "鸡";
    }
}
void iKunSpeaker2(std::memory_order order) {
    if (atm_ikun2.load(order)) {
        std::cout << "你";
    }
}
void iKunSpeaker3(std::memory_order order) {
    if (atm_ikun3.load(order)) {
        std::cout << "太";
    }
}
void iKunSpeaker4(std::memory_order order) {
    if (atm_ikun4.load(order)) {
        std::cout << "美";
    }
}

void testSeqCst() {
    std::thread t1([] { ikunJiNi(std::memory_order_seq_cst); });
    std::thread t2([] { ikunTaiMei(std::memory_order_seq_cst); });
    std::thread speak1([] { iKunSpeaker1(std::memory_order_seq_cst); });
    std::thread speak2([] { iKunSpeaker2(std::memory_order_seq_cst); });
    std::thread speak3([] { iKunSpeaker3(std::memory_order_seq_cst); });
    std::thread speak4([] { iKunSpeaker4(std::memory_order_seq_cst); });

    t1.join();
    t2.join();
    speak1.join();
    speak2.join();
    speak3.join();
    speak4.join();
}

void testRelax() {
    std::thread t1([] { ikunJiNi(std::memory_order_relaxed); });
    std::thread t2([] { ikunTaiMei(std::memory_order_relaxed); });
    std::thread speak1([] { iKunSpeaker1(std::memory_order_relaxed); });
    std::thread speak2([] { iKunSpeaker2(std::memory_order_relaxed); });
    std::thread speak3([] { iKunSpeaker3(std::memory_order_relaxed); });
    std::thread speak4([] { iKunSpeaker4(std::memory_order_relaxed); });

    t1.join();
    t2.join();
    speak1.join();
    speak2.join();
    speak3.join();
    speak4.join();
}

void testAcuqireRelease() {
    std::thread t1([] { ikunJiNi(std::memory_order_seq_cst); });
    std::thread t2([] { ikunTaiMei(std::memory_order_seq_cst); });
    std::thread speak1([] { iKunSpeaker1(std::memory_order_acquire); });
    std::thread speak2([] { iKunSpeaker2(std::memory_order_acquire); });
    std::thread speak3([] { iKunSpeaker3(std::memory_order_acquire); });
    std::thread speak4([] { iKunSpeaker4(std::memory_order_acquire); });

    t1.join();
    t2.join();
    speak1.join();
    speak2.join();
    speak3.join();
    speak4.join();
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Test memory_order_seq_cst" << std::endl;
    // maybe 6 kinds: jini taimei, ji taimei ni, ji tai mei ni, taimei jini, tai ji mei ni, tai jini mei
    testSeqCst();
    std::cout << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // maybe any kinds
    std::cout << "Test memory_order_relaxed" << std::endl;
    testRelax();
    std::cout << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // must be jinitaimei
    std::cout << "Test memory_order_acq_rel" << std::endl;
    testAcuqireRelease();
    std::cout << std::endl;
    return 0;
}
