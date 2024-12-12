#include <atomic>
#include <cassert>
#include <iostream>
#include <mutex>
#include <optional>
#include <set>
#include <thread>

template <typename T>
class lock_free_stack
{
    struct Node;

public:
    lock_free_stack() : head_node_(nullptr), delay_delete_node_list_(nullptr), threads_which_poping_(0) {
    }
    lock_free_stack(const lock_free_stack& other) = delete;
    lock_free_stack& operator=(const lock_free_stack& other) = delete;

    void push(T value) {
        Node* new_node = new Node();
        do {
            new_node->next_node = head_node_.load();
        } while (!head_node_.compare_exchange_weak(new_node->next_node, new_node));
    }

    std::optional<T> pop() {
        ++threads_which_poping_;
        Node* old_head = nullptr;
        do {
            old_head = head_node_.load();
            if (old_head == nullptr) {
                --threads_which_poping_;
                return std::nullopt;
            }
        } while (!head_node_.compare_exchange_weak(old_head, old_head->next_node));

        T res = old_head->data;
        try_recycle(old_head);
        return res;
    }

private:
    void try_recycle(Node* delete_node) {
        if (threads_which_poping_.load() == 1) {

            delete delete_node;

            Node* delay_delete_head_node = delay_delete_node_list_.load();
            if (--threads_which_poping_ == 0) {
                delete_delay_nodes(delay_delete_head_node);
            }
            else {
                append_to_delay_node_list(delete_node);
            }
        }
        else {
            append_to_delay_node_list(delete_node);
            --threads_which_poping_;
        }
    }

    void delete_delay_nodes(Node* delete_node) {
        while (delete_node) {
            Node* next = delete_node->next_node;
            delete delete_node;
            delete_node = next;
        }
    }

    void append_to_delay_node_list(Node* delay_delete_node) {
        if (delay_delete_node) {
            Node* old_head = delay_delete_node_list_.load();
            delay_delete_node_list_.store(old_head->next_node);
        }
    }

private:
    struct Node
    {
        T data{};
        Node* next_node = nullptr;
    };

    std::atomic<Node*> head_node_;
    std::atomic<Node*> delay_delete_node_list_;
    std::atomic<int> threads_which_poping_;
};

void TestLockFreeStack() {
    lock_free_stack<int> lk_free_stack;
    std::set<int> rmv_set;
    std::mutex set_mtx;
    std::thread t1(
        [&]()
        {
            for (int i = 0; i < 200; i++) {
                lk_free_stack.push(i);
                std::cout << "push data " << i << " success!" << std::endl;
            }
        });
    std::thread t2(
        [&]()
        {
            for (int i = 0; i < 100; i++) {
                auto head = lk_free_stack.pop();
                if (!head.has_value()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                std::lock_guard<std::mutex> lock(set_mtx);
                rmv_set.insert(head.value());
                std::cout << "pop data " << head.value() << " success!" << std::endl;
            }
        });
    std::thread t3(
        [&]()
        {
            for (int i = 0; i < 100;i++) {
                auto head = lk_free_stack.pop();
                if (head.value()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                std::lock_guard<std::mutex> lock(set_mtx);
                rmv_set.insert(head.value());
                std::cout << "pop data " << head.value() << " success!" << std::endl;
            }
        });
    t1.join();
    t2.join();
    t3.join();
    assert(rmv_set.size() == 20000);
}

int main() {
    TestLockFreeStack();
}