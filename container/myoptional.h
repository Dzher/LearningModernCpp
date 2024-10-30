#include <exception>

struct my_bad_optional_access : public std::exception
{
    my_bad_optional_access() = default;
    virtual ~my_bad_optional_access() = default;

    const char* what() const noexcept override {
        return "bad myoptional access!";
    }
};

struct my_nullopt_t
{
    explicit my_nullopt_t() = default;
};

template <typename T>
struct myoptional
{
    bool m_has_value;
    // union default will not construct it's member
    union
    {
        T m_value;
        // my_nullopt_t m_my_nullopt_t;
    };

    myoptional(const T& value) : m_has_value(true), m_value(value) {
    }

    myoptional()
        : m_has_value(false)
    // ,m_my_nullopt_t()
    {
    }

    myoptional(my_nullopt_t)
        : m_has_value(false)
    // ,m_my_nullopt_t()
    {
    }

    myoptional(const myoptional& that) : m_has_value(that.m_has_value) {
        if (m_has_value) {
            // !error: m_value = T(that.m_value); // use myoptional.operator=(const T&)
            new (&m_value) T(that.m_value);  // placement new
        }
    }

    myoptional(myoptional&& that) : m_has_value(that.m_has_value) {
        if (m_has_value) {
            new (&m_value) T(std::move(that.m_value));  // placement new
        }
    }

    myoptional& operator=(my_nullopt_t) {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }

        return *this;
    }

    myoptional& operator=(T value) {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }

        new (&m_value) T(std::move(value));
        m_has_value = true;

        return *this;
    }

    myoptional& operator=(const myoptional& that) {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }

        if (that.m_has_value) {
            new (&m_value) T(that.m_value);
        }
        m_has_value = that.m_has_value;

        return *this;
    }

    myoptional& operator=(myoptional&& that) {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }

        if (that.m_has_value) {
            new (&m_value) T(std::move(that.m_value));
            that.m_value.~T();
        }
        m_has_value = that.m_has_value;
        that.m_has_value = false;

        return *this;
    }

    ~myoptional() {
        if (m_has_value) {
            m_value.~T();
        }
    }

    bool has_value() const {
        return m_has_value;
    }

    T& value() & {
        if (m_has_value) {
            return m_value;
        }
        throw my_bad_optional_access();
    }

    T const& value() const& {
        if (m_has_value) {
            return m_value;
        }
        throw my_bad_optional_access();
    }

    T&& value() && {
        if (m_has_value) {
            return std::move(m_value);
        }
        throw my_bad_optional_access();
    }

    T const&& value() const&& {
        if (m_has_value) {
            return std::move(m_value);
        }
        throw my_bad_optional_access();
    }

    T value_or(T default_value) const& {
        if (m_has_value) {
            return m_value;
        }
        return default_value;
    }

    T value_or(T default_value) const&& {
        if (m_has_value) {
            return std::move(m_value);
        }
        return std::move(default_value);
    }
};