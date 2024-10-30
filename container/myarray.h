#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

template <typename _Tp, size_t _N>
struct myArray
{
    using value_type = _Tp;
    using iterator = _Tp*;
    using const_iterator = _Tp const*;

    _Tp _M_element[_N];

    _Tp& operator[](size_t __n) {
        return _M_element[__n];
    }

    _Tp const& operator[](size_t __n) const {
        return _M_element[__n];
    }

    // if user input n < 0, the n will cast to size_t and out of range.
    _Tp& at(size_t __n) {
        if (__n >= _N) [[unlikely]] {
            throw std::runtime_error("out of range: index = " + std::to_string(__n));
            return;
        }
        return _M_element[__n];
    }

    _Tp const& at(size_t __n) const {
        if (__n >= _N) [[unlikely]] {
            throw std::runtime_error("out of range: index = " + std::to_string(__n));
            return;
        }
        return _M_element[__n];
    }

    void fill(_Tp const& __value) noexcept(std::is_nothrow_copy_assignable_v<_Tp>) {
        for (size_t __i = 0; __i < _N; ++__i) {
            _M_element[__i] = __value;
        }
    }

    void swap(myArray& __other) noexcept(std::is_nothrow_copy_assignable_v<_Tp>) {
        for (size_t __i = 0; __i < _N; __i++) {
            std::swap(_M_element[__i], __other._M_element[__i]);
        }
    }

    static constexpr size_t size() noexcept {
        return _N;
    }

    static constexpr size_t max_size() noexcept {
        return _N;
    }

    _Tp* data() noexcept {
        return _M_element;
    }

    _Tp* const cdata() const noexcept {
        return _M_element;
    }

    _Tp& front() {
        return _M_element[0];
    }

    _Tp const& front() const noexcept {
        return _M_element[0];
    }

    _Tp& back() {
        return _M_element[_N - 1];
    }

    _Tp const& end() const noexcept {
        return _M_element[_N - 1];
    }

    _Tp* begin() noexcept {
        return _M_element;
    }

    _Tp* end() noexcept {
        return _M_element + _N;
    }

    _Tp* const cbegin() const noexcept {
        return std::as_const(begin());
    }

    _Tp* const cend() const noexcept {
        return std::as_const(end());
    }

    _Tp* rbegin() noexcept {
        return std::make_reverse_iterator(begin());
    }

    _Tp* rend() noexcept {
        return std::make_reverse_iterator(end());
    }

    _Tp* const crbegin() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    _Tp* const crend() const noexcept {
        return std::make_reverse_iterator(cend());
    }
};

template <typename _Tp>
struct myArray<_Tp, 0>
{
    using value_type = _Tp;
    using iterator = _Tp*;
    using const_iterator = _Tp const*;

    _Tp& operator[](size_t __n) {
        assert(0);
        // std::unreachable(); // in cpp23
    }

    _Tp const& operator[](size_t __n) const {
        assert(0);
    }

    _Tp& at(size_t __n) {
        throw std::runtime_error("out of range: __n = " + std::to_string(__n));
    }

    _Tp const& at(size_t __n) const {
        throw std::runtime_error("out of range: __n = " + std::to_string(__n));
    }

    static constexpr size_t size() noexcept {
        return 0;
    }

    _Tp* begin() noexcept {
        return nullptr;
    }

    _Tp* end() noexcept {
        return nullptr;
    }

    _Tp* begin() const noexcept {
        return nullptr;
    }

    _Tp* end() const noexcept {
        return nullptr;
    }

    _Tp* const cbegin() const noexcept {
        return nullptr;
    }

    _Tp* const cend() const noexcept {
        return nullptr;
    }

    _Tp* rbegin() noexcept {
        return nullptr;
    }

    _Tp* rend() noexcept {
        return nullptr;
    }

    _Tp* rbegin() const noexcept {
        return nullptr;
    }

    _Tp* rend() const noexcept {
        return nullptr;
    }

    _Tp* const crbegin() const noexcept {
        return nullptr;
    }

    _Tp* const crend() const noexcept {
        return nullptr;
    }
};

template <class _Tp, class... _Ts>
myArray(_Tp, _Ts...) -> myArray<_Tp, 1 + sizeof...(_Ts)>;

template <class _Tp, size_t _N>
myArray<std::decay_t<_Tp>, _N> toArray(_Tp (&)[_N]) {
}