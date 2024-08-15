#include <condition_variable>
#include <coroutine>
#include <exception>
#include <functional>
#include <list>
#include <mutex>
#include <optional>

template <typename T>
struct Result
{
public:
    explicit Result(T&& value) : value_(value)
    {
    }

    explicit Result(std::exception_ptr&& exception_ptr) : exception_ptr_(exception_ptr)
    {
    }

    T valueOrException()
    {
        if (exception_ptr_)
        {
            std::rethrow_exception(exception_ptr_);
        }
        return value_;
    }

private:
    T value_{};
    std::exception_ptr exception_ptr_;
};

template <typename T>
struct TaskPromise;

template <typename T>
struct Task
{
    using promise_type = TaskPromise<T>;
    explicit Task(std::coroutine_handle<promise_type> handle) noexcept : handle_(handle)
    {
    }
    Task(Task&& task) noexcept : handle_(std::exchange(task.handle, {}))
    {
    }
    Task(Task&) = delete;
    Task& operator=(Task&) = delete;
    ~Task()
    {
        if (handle_)
        {
            handle_.destroy();
        }
    }

    T getResult()
    {
        return handle_.promise().getResult();
    }

    Task& then(std::function<void(T)>&& func)
    {
        handle_.promise().onCompleted(
            [func](auto result)
            {
                try
                {
                    func(result.get_or_throw());
                }
                catch (std::exception& e)
                {
                }
            });
        return *this;
    }

    Task& catching(std::function<void(std::exception&)>&& func)
    {
        handle_.promise().onCompleted(
            [func](auto result)
            {
                try
                {
                    result.valueOrException();
                }
                catch (std::exception& e)
                {
                    func(e);
                }
            });
        return *this;
    }

    Task& finally(std::function<void()>&& func)
    {
        handle_.promise().onCompleted([func](auto result) { func(); });
        return *this;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

template <typename T>
struct TaskAwaiter
{
    explicit TaskAwaiter(Task<T>&& task) noexcept : task_(std::move(task))
    {
    }
    TaskAwaiter(TaskAwaiter&) = delete;
    TaskAwaiter& operator=(TaskAwaiter&) = delete;

    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        task_.finally([handle]() { handle.resume(); });
    }

    T await_resume() noexcept
    {
        return task_.getResult();
    }

private:
    Task<T> task_;
};

template <typename T>
struct TaskPromise
{
    auto initial_suspend()
    {
        return std::suspend_never{};
    }

    auto final_suspend()
    {
        return std::suspend_always{};
    }

    Task<T> get_return_object()
    {
        return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
    }

    template <typename U>
    TaskAwaiter<U> await_transform(Task<U>&& task)
    {
        return std::move(task);
    }

    void return_value(T value)
    {
        std::lock_guard lock(lock_);
        result_ = value;
        condition_.notify_all();
        notifyCallbacks();
    }

    Task<T> unhandled_exception()
    {
        std::lock_guard lock(lock_);
        result_ = std::current_exception();
        condition_.notify_all();
        notifyCallbacks();
    }

    T getResult()
    {
        std::unique_lock lock(lock_);
        if (!result_.has_value())
        {
            condition_.wait(lock);
        }
        return result_.value().valueOrException();
    }

    void onCompleted(std::function<void(Result<T>)>&& func)
    {
        std::unique_lock lock(lock_);
        if (result_.has_value())
        {
            auto value = result_.value();
            lock.unlock();
            func(value);
        }
        else
        {
            completion_callbacks_.push_back(func);
        }
    }

private:
    void notifyCallbacks()
    {
        auto value = result_.value();
        for (auto& callback : completion_callbacks_)
        {
            callback(value);
        }
        completion_callbacks_.clear();
    }

private:
    std::optional<Result<T>> result_;
    std::mutex lock_;
    std::condition_variable condition_;
    std::list<std::function<void(Result<T>)>> completion_callbacks_;
};
