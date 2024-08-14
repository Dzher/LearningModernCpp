#include <coroutine>
#include <iostream>

template <typename T>
struct Coro_Ret
{
    struct CoroPromise;
    using promise_type = CoroPromise;
    using handle_type = std::coroutine_handle<promise_type>;

    handle_type handle_;

    Coro_Ret(handle_type h) : handle_(h)
    {
    }
    Coro_Ret(const Coro_Ret&) = delete;
    Coro_Ret(Coro_Ret&& coro) : handle_(coro.handle_)
    {
        coro.handle_ = nullptr;
    }
    ~Coro_Ret()
    {
        if (handle_.done())
        {
            handle_.destroy();
        }
    }

    Coro_Ret& operator=(const Coro_Ret&) = delete;
    Coro_Ret& operator=(Coro_Ret&& coro)
    {
        handle_ = coro.handle_;
        coro.handle_ = nullptr;
        return *this;
    }

    bool hasNext()
    {
        return !handle_.done();
    }

    void moveNext()
    {
        if (hasNext())
        {
            handle_.resume();
        }
    }

    T get()
    {
        return handle_.promise().value_;
    }

    struct CoroPromise
    {
        T value_{};

        auto initial_suspend()
        {
            std::cout << "initial_suspend" << std::endl;
            return std::suspend_always{};
        }

        auto get_return_object()
        {
            std::cout << "get_return_object" << std::endl;
            return Coro_Ret<T>{handle_type::from_promise(*this)};
        }

        void return_value(T value)
        {
            std::cout << "return_value" << std::endl;
            value_ = value;
        }

        auto yield_value(T value)
        {
            std::cout << "yield_value" << std::endl;
            value_ = value;
            return std::suspend_always{};
        }

        auto final_suspend() noexcept
        {
            std::cout << "final_suspend" << std::endl;
            return std::suspend_always{};
        }

        void unhandled_exception()
        {
            std::exit(1);
        }
    };
};