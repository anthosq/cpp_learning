#pragma once
#include <exception>
#include <initializer_list>
#include <type_traits>

struct BadOptionalAccess : std::exception {
    BadOptionalAccess() = default;
    virtual ~BadOptionalAccess() = default;

    const char *what() const noexcept override {
        return "bad optional access"; 
    }
};

// tag类
struct Nullopt_t {
    explicit Nullopt_t() = default;
};

inline Nullopt_t Nullopt;

template <class T>
struct Optional {
    Optional(T value) : m_has_value(true), m_value(std::move(value)) {}
    
    Optional() noexcept : m_has_value(false) {}

    Optional(Nullopt_t) noexcept : m_has_value(false) {}

    Optional(Optional const &other) : m_has_value(other.m_has_value) {
        if (m_has_value) {
            new (&m_value) T(other.m_value); //placement-new (不分配内存只是构造)
        }
    }

    Optional(Optional &&other) noexcept : m_has_value(other.m_has_value) {
        if (m_has_value) {
            new (&m_value) T(std::move(other.m_value)); // 此处that已退化为左值引用，使用move重新转为右值引用
        }
    }

    Optional &operator=(Nullopt_t) noexcept {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
        return *this;
    }

    Optional &operator=(T value) noexcept {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
        new (&m_value) T(std::move(value));
        m_has_value = true;
        return *this;
    }

    Optional &operator=(Optional const &other) {
        if (this == &other) return *this;
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
        if (other.m_has_value) {
            new (&m_value) T(other.m_value);
        }
        m_has_value = other.m_has_value;
        return *this;
    }

    Optional &operator=(Optional &&other) noexcept {
        if (this == &other) return *this;
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
        if (other.m_has_value) {
            new (&m_value) T(std::move(other.m_value));
            other.m_value.~T();
        }
        m_has_value = other.m_has_value;
        other.m_has_value = false;
        return *this;
    }

    ~Optional() noexcept {
        if (m_has_value) {
            m_value.~T(); // 不释放内存只是析构
        }
    }

    bool has_value() const noexcept {
        return m_has_value;
    }
    
    T const &value() const & {
        if (!m_has_value)
            throw BadOptionalAccess();
        return m_value;
    }

    T &value() & {
        if (!m_has_value)
            throw BadOptionalAccess();
        return m_value;
    }

    T const &&value() const && {
        if (!m_has_value)
            throw BadOptionalAccess();
        return std::move(m_value);
    }

    T &&value() && {
        if (!m_has_value)
            throw BadOptionalAccess();
        return std::move(m_value);
    }

    T const &operator*() const & noexcept {
        return m_value;
    }

    T &operator*() & noexcept {
        return m_value;
    }

    T &&operator*() && noexcept {
        return std::move(m_value);
    }

    T const &&operator*() const && noexcept {
        return std::move(m_value);
    }

    // 拷贝考虑string,不加noexcept
    T value_or(T default_value) const & {
        if (!m_has_value)
            return default_value;
        return m_value;
    }

    T value_or(T default_value)  && noexcept(std::is_nothrow_move_assignable_v<T>) {
        if (!m_has_value)
            return default_value;
        return std::move(m_value);
    }

    template <class ...Ts>
    void emplace(Ts &&...args) noexcept {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
        new (&m_value) T(std::forward<Ts>(args)...);
        m_has_value = true;
    }

    void reset() noexcept {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
    }

private:
    bool m_has_value;
    union {
        T m_value;
    };
    // union _Storage, m_value, m_empty
    // union不默认初始化，union在C++17前用来模拟实现variant
    // union中的结构体记得手动调用析构函数
    // 或者给union定义一个析构函数
};












