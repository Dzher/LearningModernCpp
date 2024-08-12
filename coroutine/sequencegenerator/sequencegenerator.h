#include <coroutine>
#include <exception>
#include <utility>
struct Generator
{
    struct promise_type
    {
        std::suspend_never initial_suspend()
        {
            return {};
        };

        std::suspend_never final_suspend() noexcept
        {
            return {};
        };

        // co_await expr => co_await promise.await_transform(expr)
        std::suspend_always await_transform(int value)
        {
            this->value = value;
            is_ready = true;
            return {};
        }

        // co_yield expr => co_await promise.yield_value(expr)
        std::suspend_always yield_value(int value)
        {
            this->value = value;
            is_ready = true;
            return {};
        }

        Generator get_return_object()
        {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        void unhandled_exception()
        {
        }

        void return_void()
        {
        }

        int value;
        bool is_ready = false;
    };

    explicit Generator(std::coroutine_handle<promise_type> handle) noexcept : handle_(handle)
    {
    }

    Generator(Generator&& generator) noexcept : handle_(std::exchange(generator.handle_, {}))
    {
    }

    Generator(Generator&) = delete;
    Generator& operator=(Generator&) = delete;

    ~Generator()
    {
        if (handle_)
        {
            handle_.destroy();
        }
    }

    int next()
    {
        if (hasNext())
        {
            handle_.promise().is_ready = false;
            return handle_.promise().value;
        }
        throw ExhaustedException{};
    }

    bool hasNext()
    {
        if (!handle_ || handle_.done())
        {
            return false;
        }

        if (!handle_.promise().is_ready)
        {
            handle_.resume();
        }

        if (handle_.done())
        {
            return false;
        }

        return true;
    }

    // after coroutine executed, invoke form outside throw exception
    class ExhaustedException : std::exception
    {
    };

    std::coroutine_handle<promise_type> handle_;
};