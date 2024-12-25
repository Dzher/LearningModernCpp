#include <ios>
#include <iostream>
#include <type_traits>
#include <utility>

// C++20
template <typename T>
bool has_func_foo_20(T t) {
    if constexpr (requires { t.foo(); }) {
        return true;
    }
    return false;
}

template <typename T>
// concept has_foo = requires {&T::foo;};
bool has_func_foo_20() {
    if constexpr (requires { &T::foo; }) {
        return true;
    }
    return false;
}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
// C++17
template <typename T, typename = void>
struct has_foo
{
    static constexpr bool value = false;
};

template <typename T>
struct has_foo<T, std::void_t<decltype(std::declval<T>().foo())>>
{
    static constexpr bool value = true;
};

template <typename T>
bool has_func_foo_17() {
    return has_foo<T>::value;
}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
// C++14
template <typename T, std::enable_if_t<has_foo<T>::value, int> = 0>
bool has_func_foo_14() {
    return true;
}
template <typename T, std::enable_if_t<!has_foo<T>::value, int> = 0>
bool has_func_foo_14() {
    return false;
}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
// C++11
// cannot use *_t, so void_t cannot use
// template <class T>
// struct has_foo<T, decltype(std::declval<T>().foo(), (void)0)> {
//     static constexpr bool value = true;
// };

template <typename T, std::enable_if<has_foo<T>::value, int>::type = 0>
bool has_func_foo_11() {
    return true;
}
template <typename T, std::enable_if<!has_foo<T>::value, int>::type = 0>
bool has_func_foo_11() {
    return false;
}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
// C++98 
// FUCK C++

int main() {
    struct A
    {
        static void foo() {
            std::cout << "static foo() from struct A" << std::endl;
        }
    };

    struct B
    {
        bool is_foo;
        void foo() {
            is_foo = true;
            std::cout << "public foo() from struct B" << std::endl;
        }
    };

    struct C
    {
        bool is_foo;

    private:
        void foo() {
            is_foo = true;
            std::cout << "private foo() from struct C" << std::endl;
        }
    };

    std::cout << std::boolalpha;
    std::cout << "C++20 test A " << has_func_foo_20(A()) << std::endl;
    std::cout << "C++20 test B " << has_func_foo_20(B()) << std::endl;
    std::cout << "C++20 test C " << has_func_foo_20(C()) << std::endl;
    std::cout << "C++20 test A " << has_func_foo_20<A>() << std::endl;
    std::cout << "C++20 test B " << has_func_foo_20<B>() << std::endl;
    std::cout << "C++20 test C " << has_func_foo_20<C>() << std::endl;
    std::cout << "C++17 test A " << has_func_foo_17<A>() << std::endl;
    std::cout << "C++17 test B " << has_func_foo_17<B>() << std::endl;
    std::cout << "C++17 test C " << has_func_foo_17<C>() << std::endl;
    std::cout << "C++14 test A " << has_func_foo_14<A>() << std::endl;
    std::cout << "C++14 test B " << has_func_foo_14<B>() << std::endl;
    std::cout << "C++14 test C " << has_func_foo_14<C>() << std::endl;
    std::cout << "C++11 test A " << has_func_foo_11<A>() << std::endl;
    std::cout << "C++11 test B " << has_func_foo_11<B>() << std::endl;
    std::cout << "C++11 test C " << has_func_foo_11<C>() << std::endl;
}