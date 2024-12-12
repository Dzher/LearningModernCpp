#include <atomic>
#include <optional>

template <typename T>
class lock_free_stack
{
    struct Node;

public:
    lock_free_stack() : head_node_(nullptr) {
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
        Node* old_head = nullptr;
        do {
            old_head = head_node_.load();
            if (old_head == nullptr) {
                return std::nullopt;
            }
        } while (!head_node_.compare_exchange_weak(old_head, old_head->next_node));

        T res = old_head->data;
        delete old_head;
        return res;
    }

private:
    struct Node
    {
        T data;
        Node* next_node = nullptr;
    };
    std::atomic<Node*> head_node_;
};