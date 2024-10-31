#include <exception>
#include <memory>
#include <type_traits>
#include <utility>

struct __my_bad_optional_access : public std::exception
{
    __my_bad_optional_access() = default;
    virtual ~__my_bad_optional_access() = default;

    const char* what() const noexcept override {
        return "bad myoptional access!";
    }
};

struct __my_nullopt_t
{
    explicit __my_nullopt_t() = default;
};

template <typename _Tp>
struct myOptional
{
    bool __m_has_value;
    // union default will not construct it's member
    union
    {
        _Tp __m_value;
        __my_nullopt_t __m_my_nullopt_t;
    };

    myOptional(const _Tp& value) : __m_has_value(true), __m_value(value) {
    }

    myOptional() : __m_has_value(false), __m_my_nullopt_t() {
    }

    myOptional(__my_nullopt_t) : __m_has_value(false), __m_my_nullopt_t() {
    }

    myOptional(const myOptional& __that) : __m_has_value(__that.__m_has_value) {
        if (__m_has_value) {
            // !error: m_value = T(__that.m_value); // use myoptional.operator=(const T&)
            new (&__m_value) _Tp(__that.m_value);  // placement new
        }
    }

    myOptional(myOptional&& __that) : __m_has_value(__that.__m_has_value) {
        if (__m_has_value) {
            new (&__m_value) _Tp(std::move(__that.m_value));  // placement new
        }
    }

    myOptional& operator=(__my_nullopt_t) {
        if (__m_has_value) {
            __m_value.~_Tp();
            __m_has_value = false;
        }

        return *this;
    }

    myOptional& operator=(_Tp __value) {
        if (__m_has_value) {
            __m_value.~_Tp();
            __m_has_value = false;
        }

        new (&__m_value) _Tp(std::move(__value));
        __m_has_value = true;

        return *this;
    }

    myOptional& operator=(const myOptional& __that) {
        if (this == __that) {
            return *this;
        }

        if (__m_has_value) {
            __m_value.~_Tp();
            __m_has_value = false;
        }

        if (__that.__m_has_value) {
            new (&__m_value) _Tp(__that.m_value);
        }
        __m_has_value = __that.__m_has_value;

        return *this;
    }

    myOptional& operator=(myOptional&& __that) {
        if (__m_has_value) {
            __m_value.~_Tp();
            __m_has_value = false;
        }

        if (__that.__m_has_value) {
            new (&__m_value) _Tp(std::move(__that.m_value));
            __that.m_value.~_Tp();
        }
        __m_has_value = __that.__m_has_value;
        __that.__m_has_value = false;

        return *this;
    }

    ~myOptional() noexcept {
        if (__m_has_value) {
            __m_value.~_Tp();
        }
    }

    bool has_value() const {
        return __m_has_value;
    }

    explicit operator bool() const noexcept {
        return __m_has_value;
    }

    _Tp& value() & {
        if (__m_has_value) {
            return __m_value;
        }
        throw __my_bad_optional_access();
    }

    _Tp const& value() const& {
        if (__m_has_value) {
            return __m_value;
        }
        throw __my_bad_optional_access();
    }

    _Tp&& value() && {
        if (__m_has_value) {
            return std::move(__m_value);
        }
        throw __my_bad_optional_access();
    }

    _Tp const&& value() const&& {
        if (__m_has_value) {
            return std::move(__m_value);
        }
        throw __my_bad_optional_access();
    }

    _Tp& operator*() & noexcept {
        return __m_value;
    }

    _Tp const& operator*() const& noexcept {
        return __m_value;
    }

    _Tp&& operator*() && noexcept {
        return std::move(__m_value);
    }

    _Tp const&& operator*() const&& noexcept {
        return std::move(__m_value);
    }

    _Tp* operator->() noexcept {
        // return &m_value;
        return std::addressof(__m_value);
    }

    _Tp const* operator->() const noexcept {
        // return &m_value;
        return std::addressof(__m_value);
    }

    _Tp value_or(_Tp default_value) const& {
        if (__m_has_value) {
            return __m_value;
        }
        return default_value;
    }

    _Tp value_or(_Tp default_value) const&& {
        if (__m_has_value) {
            return std::move(__m_value);
        }
        return std::move(default_value);
    }

    template <typename... _Ts>
    void emplace(_Ts&&... __ts) {
        if (__m_has_value) {
            __m_value.~_Tp();
            __m_has_value = false;
        }
        new (&__m_value) _Tp(std::forward<_Ts>(__ts)...);
        __m_has_value = true;
    }

    void reset() {
        if (__m_has_value) {
            __m_value.~_Tp();
            __m_has_value = false;
        }
    }

    template <typename _Func>
    auto and_then(_Func&& __f) {
        using _RetType = std::remove_cv_t<std::remove_reference_t<decltype(__f(__m_value))>>;
        if (__m_has_value) {
            return std::forward<_Func>(__f)(__m_value);
        }
        return _RetType{};
    }

    template <typename _Func>
    // avoid undefined variable '__f'
    auto transform(_Func&& __f) -> myOptional<std::remove_cv_t<std::remove_reference_t<decltype(__f(__m_value))>>> {
        if (__m_has_value) {
            return std::forward<_Func>(__f)(__m_value);
        }
        return __m_my_nullopt_t;
    }

    template <typename _F>
    myOptional or_else(_F&& __f) {
        if (__m_has_value) {
            return *this;
        }
        return std::forward<_F>(__f)();
    }
};