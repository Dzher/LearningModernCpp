#include "7in7out.h"

Coro_Ret<int> coro7i7oFunc()
{
    std::cout << "Coroutine co_await std::suspend_never" << std::endl;
    co_await std::suspend_never{};
    std::cout << "Coroutine co_await std::suspend_always" << std::endl;
    co_await std::suspend_always{};

    std::cout << "Coroutine stage 1, co_yield" << std::endl;
    co_yield 101;
    std::cout << "Coroutine stage 2, co_yield" << std::endl;
    co_yield 202;
    std::cout << "Coroutine stage 3, co_yield" << std::endl;
    co_yield 303;

    std::cout << "Coroutine stage end, co_return" << std::endl;
    co_return 808;
}

int main(int argc, char* argv[])
{
    bool is_not_done = false;
    std::cout << "Start coroutine_7in7out ()" << std::endl;
    // invoke coroutine func and get return value, then use the return value to handle coroutine.
    auto c_r = coro7i7oFunc();

    // stop at initial_suspend func as its' return value is suspend_always
    is_not_done = c_r.hasNext();
    if (is_not_done)
    {
        c_r.moveNext();
    }
    std::cout << "Coroutine " << (is_not_done ? "isn't done, " : "is done ") << "ret = " << c_r.get() << std::endl;

    is_not_done = c_r.hasNext();
    if (is_not_done)
    {
        //invoke co_await std::suspend_always{}
        c_r.moveNext();
    }
    std::cout << "Coroutine " << (is_not_done ? "isn't done, " : "is done ") << "ret = " << c_r.get() << std::endl;

    is_not_done = c_r.hasNext();
    if (is_not_done)
    {
        c_r.moveNext();
    }
    std::cout << "Coroutine " << (is_not_done ? "isn't done, " : "is done ") << "ret = " << c_r.get() << std::endl;

    is_not_done = c_r.hasNext();
    if (is_not_done)
    {
        c_r.moveNext();
    }
    std::cout << "Coroutine " << (is_not_done ? "isn't done, " : "is done ") << "ret = " << c_r.get() << std::endl;

    is_not_done = c_r.hasNext();
    if (is_not_done)
    {
        c_r.moveNext();
    }
    std::cout << "Coroutine " << (is_not_done ? "isn't done, " : "is done ") << "ret = " << c_r.get() << std::endl;

    is_not_done = c_r.hasNext();
    if (is_not_done)
    {
        c_r.moveNext();
    }
    std::cout << "Coroutine " << (is_not_done ? "isn't done, " : "is done ") << "ret = " << c_r.get() << std::endl;

    return 0;
}