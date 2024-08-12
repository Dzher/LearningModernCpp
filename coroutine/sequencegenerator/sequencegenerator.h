#include <coroutine>

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
    };

    std::coroutine_handle<promise_type> handle;
    int next()
    {
        handle.resume();
        return handle.promise().value;
    }
};