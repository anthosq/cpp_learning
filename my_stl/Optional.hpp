#pragma once
#include <exception>
#include <functional>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

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

struct InPlace_t {
    explicit InPlace_t() = default;
};

inline constexpr InPlace_t InPlace;

template <class T>
struct Optional {
    Optional(T &&value) : m_has_value(true), m_value(std::move(value)) {}
    Optional(T const &value) : m_has_value(true), m_value(std::move(value)) {}
    
    Optional() noexcept : m_has_value(false) {}

    Optional(Nullopt_t) noexcept : m_has_value(false) {}

    template <class ...Ts>
    explicit Optional(InPlace_t, Ts ...value_args) noexcept : m_has_value(true), m_value(std::forward<Ts>(value_args)...) {}

    template <class U, class ...Ts>
    explicit Optional(InPlace_t, std::initializer_list<U> ilist, Ts &&...value_args) noexcept : m_has_value(true), m_value(ilist, std::forward<Ts>(value_args)...) {}

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

    T const *operator->() const noexcept {
        return std::addressof(m_value);
    }

    T *operator->() noexcept {
        return std::addressof(m_value);
    }

    explicit operator bool()  const noexcept {
        return m_has_value;
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

    template <class U, class ...Ts>
    void emplace(std::initializer_list<U> ilist, Ts &&...value_args) {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
        new (&m_value) T(ilist,std::forward<Ts>(value_args)...);
        m_has_value = true;
    }

    void reset() noexcept {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
    }

    void swap(Optional &other) noexcept {
        if (m_has_value && other.m_has_value) {
            using std::swap; // 利用ADL实现多态效果
            // 若对象的类型在自己的名字空间定义了swap，优先调用对象的swap函数
            std::swap(m_value, other.m_value);
        } else if (m_has_value) {
            new (&other.m_value) T(std::move(m_value));
            m_value.~T();
            m_has_value = false;
            other.m_has_value = true;
        } else if (other.m_has_value) {
            new (&m_value) T(std::move(other.m_value));
            other.m_value.~T();
            m_has_value = true;
            other.m_has_value = false;
        }
    }

    bool operator==(Nullopt_t) const noexcept {
        return !m_has_value;
    }

    friend bool operator==(Nullopt_t, Optional const &opt) noexcept {
        return !opt.m_has_value;
    }

    bool operator!=(Nullopt_t) const noexcept {
        return m_has_value;
    }

    friend bool operator!=(Nullopt_t, Optional const &opt) noexcept {
        return opt.m_has_value;
    }

    template <class U>
    bool operator==(Optional<U> const &other) const {
        if (m_has_value != other.m_has_value) {
            return false;
        }
        if (!m_has_value) {
            return true;
        }
        return m_value == other.m_value;
    }

    template <class U>
    bool operator!=(Optional<U> const &other) const {
        return !(*this == other);
    }

    template <class U>
    bool operator<(Optional<U> const &other) const {
        if (!m_has_value) 
            return false;
        return m_value<other.m_value;
    }

    template <class U>
    bool operator<=(Optional<U> const &other) const noexcept {
        if (!m_has_value)
            return false;
        return m_value <= other.m_value;
    }

    template <class U>
    bool operator>(Optional<U> const &other) const noexcept {
        if (!m_has_value)
            return false;
        return m_value > other.m_value;
    }

    template <class F>
    constexpr auto and_then(F &&f) & {
        // std::remove_cvref_t<> C++20
        // using RetType = std::decay_t<decltype(f(m_value))>;
        // using RetType = std::remove_cvref_t<std::invoke_result_t<F, T&>>{};
        using RetType = std::remove_cv_t<std::remove_reference_t<decltype(f(m_value))>>;
        if (m_has_value) {
            return std::invoke(std::forward<F>(f), m_value);
        } else {
        return RetType{};
        }
    }
    // 使用万能引用用于针对移动式引用
    // std::unique_ptr<int> up = std::make_unique<int>();
    // auto optional2 = ooptional1.and_then([up = std::move(up)] (int i) -> int {});
    // 因为unique_ptr只许移动不许拷贝，lambda变得也无法拷贝

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

#if __cpp_deduction_guides
template <class T> // CTAD
Optional(T) -> Optional<T>;
#endif

template <class T>
Optional<T> makeOptional(T value) {
    return Optional<T>(std::move(value));
}











