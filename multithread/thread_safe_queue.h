#include <chrono>
#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>

struct empty_queue_error : std::exception
{
    [[nodiscard]] const char* what() const noexcept override {
        return "Error: Queue is Empty!!!";
    }
};

template <typename T>
class thread_safe_queue
{
    struct Node;

public:
    thread_safe_queue() : head_node_(new Node), tail_node_(head_node_) {
    }
    thread_safe_queue& operator=(const thread_safe_queue& other) = delete;
    thread_safe_queue(const thread_safe_queue& other) = delete;

    void push(T value) {
        std::unique_lock<std::mutex> lock{tail_mtx_};
        tail_node_->next_node = new Node;
        tail_node_->next_node->data = value;
        tail_node_ = tail_node_->next_node;
        std::cout << "Pushed " << value << std::endl;
        cv_.notify_one();
    }

    bool empty() {
        std::lock_guard<std::mutex> head_lock{head_mtx_};
        std::lock_guard<std::mutex> tail_lock{tail_mtx_};
        return head_node_ == tail_node_;
    }

    T pop() {
        std::unique_lock<std::mutex> lock{head_mtx_};
        if (head_node_ == get_tail()) {  // empty() cannot be used here
            throw empty_queue_error{};
        }
        T res = head_node_->data;
        head_node_ = head_node_->next_node;
        return res;
    }

    std::optional<T> try_pop() {
        std::unique_lock<std::mutex> lock{head_mtx_};
        if (head_node_ == get_tail()) {  // empty() cannot be used here
            return std::nullopt;
        }
        T res = head_node_->data;
        head_node_ = head_node_->next_node;
        return res;
    }

    T wait_and_pop() {
        std::unique_lock<std::mutex> lock{head_mtx_};
        cv_.wait(lock, [this] { return head_node_ != get_tail(); });  // empty() cannot be used here
        T res = head_node_->data;
        head_node_ = head_node_->next_node;
        std::cout << "Poped " << res << std::endl;
        return res;
    }

private:
    Node* get_tail() {
        std::lock_guard<std::mutex> lock{tail_mtx_};
        return tail_node_;
    }

private:
    struct Node
    {
        T data;
        Node* next_node;
    };

    Node* head_node_;
    Node* tail_node_;
    std::mutex head_mtx_;
    std::mutex tail_mtx_;
    std::condition_variable cv_;
};

// Attention!
// the initial of queue has some issue, it's empty method judgement is error, head == tail real means the size == 1
// but for covenient I make head == tail means the queue is empty, and it will cause pop method pop first value error, and will never pop the last value
int main() {
    thread_safe_queue<int> queue;

    std::thread t_push(
        [&queue]
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            int count = 0;
            while (count < 30) {
                queue.push(++count);
            }
        });
    std::thread t_pop(
        [&queue]
        {
            while (true) {
                auto value = queue.wait_and_pop();
                if (queue.empty()) {
                    return;
                }
            }
        });

    t_push.join();
    t_pop.join();

    return 0;
}