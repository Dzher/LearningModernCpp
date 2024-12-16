#include <atomic>
#include <condition_variable>
#include <exception>
#include <future>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

struct thread_interrupted : std::exception
{
    thread_interrupted() : message_("thread interrupted.") {
    }

    [[nodiscard]] const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};

extern void interruption_point();

class interrupt_flag
{
public:
    interrupt_flag() : cv_(nullptr), cv_any_(nullptr) {
    }

    void set() {
        flag_.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock{cv_mtx_};
        if (cv_) {
            cv_->notify_all();
        }
        else if (cv_any_) {
            cv_any_->notify_all();
        }
    }

    bool is_set() {
        return flag_.load(std::memory_order_relaxed);
    }

    void set_condition_variable(std::condition_variable& cv) {
        std::lock_guard<std::mutex> lk(cv_mtx_);
        cv_ = &cv;
    }

    void clear_condition_variable() {
        std::lock_guard<std::mutex> lk(cv_mtx_);
        cv_ = nullptr;
    }

    template <typename Lockable>
    void wait(std::condition_variable_any& cv, Lockable& lk) {
        struct custom_lock
        {
            interrupt_flag* self_;
            Lockable& lock_;

            custom_lock(interrupt_flag* flag, std::condition_variable_any& cond, Lockable& lock)
                : self_(flag), lock_(lock) {
                self_->cv_mtx_.lock();
                self_->cv_any_ = &cond;
            }

            ~custom_lock() {
                self_->cv_any_ = nullptr;
                self_->cv_mtx_.unlock();
            }

            void unlock() {
                lock_.unlock();
                self_->cv_mtx_.unlock();
            }

            void lock() {
                std::lock(self_->cv_mtx_, lock_);
            }
        };

        custom_lock cl(this, cv, lk);
        interruption_point();
        cv.wait(cl);
        interruption_point();
    }

private:
    std::atomic<bool> flag_;
    std::mutex cv_mtx_;
    std::condition_variable* cv_;
    std::condition_variable_any* cv_any_;
};

thread_local interrupt_flag current_thread_interrupt_flag;

// template <typename FunctionType>
class interruptible_thread
{
public:
    template <typename FunctionType>
    explicit interruptible_thread(FunctionType func) {
        std::promise<interrupt_flag*> promise_flag;
        flag_ = promise_flag.get_future().get();

        thread_ = std::thread(
            [&func, &promise_flag]()
            {
                promise_flag.set_value(&current_thread_interrupt_flag);
                func();
            });
    }

    void interrupt() {
        std::cout << "ss" << std::endl;
        flag_->set();
    }

    void join() {
        thread_.join();
    }

private:
    std::thread thread_;
    interrupt_flag* flag_;
};

struct clear_cv_on_destruct
{
    ~clear_cv_on_destruct() {
        current_thread_interrupt_flag.clear_condition_variable();
    }
};

void interruption_point() {
    if (current_thread_interrupt_flag.is_set()) {
        throw thread_interrupted();
    }
}

void interruptible_wait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk) {
    interruption_point();
    current_thread_interrupt_flag.set_condition_variable(cv);
    clear_cv_on_destruct guard;
    interruption_point();
    cv.wait_for(lk, std::chrono::milliseconds(1));
    interruption_point();
}

template <typename Predicate>
void interruptible_wait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk, Predicate pred) {
    interruption_point();
    current_thread_interrupt_flag.set_condition_variable(cv);
    clear_cv_on_destruct guard;
    while (!current_thread_interrupt_flag.is_set() && !pred()) {
        cv.wait_for(lk, std::chrono::milliseconds(1));
    }
    interruption_point();
}

template <typename Lockable>
void interruptible_wait(std::condition_variable_any& cv, Lockable& lk) {
    current_thread_interrupt_flag.wait(cv, lk);
}

template <typename T>
void interruptible_wait(std::future<T>& uf) {
    while (!current_thread_interrupt_flag.is_set()) {
        if (uf.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
            break;
    }
    interruption_point();
}

// Demo for use
std::vector<interruptible_thread /*<std::function<void(void)>>*/> background_threads;
std::mutex mtx1;
std::mutex mtx2;
std::condition_variable cv1;
std::condition_variable_any cv2;

void startBackgroundThreads() {
    interruptible_thread thread1{[]()
                                 {
                                     try {
                                         std::unique_lock<std::mutex> lock(mtx1);
                                         interruptible_wait(cv1, lock);
                                     }
                                     catch (std::exception& ex) {
                                         std::cout << "catch exception is " << ex.what() << std::endl;
                                     }
                                 }};
    background_threads.push_back(std::move(thread1));

    interruptible_thread thread2{[]()
                                 {
                                     try {
                                         std::unique_lock<std::mutex> lock(mtx2);
                                         current_thread_interrupt_flag.wait(cv2, mtx2);
                                     }
                                     catch (std::exception& ex) {
                                         std::cout << "catch exception is " << ex.what() << std::endl;
                                     }
                                 }};
    background_threads.push_back(std::move(thread2));
}

int main() {
    startBackgroundThreads();
    for (auto&& background_thread : background_threads) {
        background_thread.interrupt();
    }

    for (auto&& background_thread : background_threads) {
        background_thread.join();
    }
}