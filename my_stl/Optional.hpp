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
    Optional(T value) : m_has_value(true), m_value(value) {}
    
    Optional() : m_has_value(false), m_value() {}

    Optional(Nullopt_t) : m_has_value(false), m_value() {}

    bool has_value() const {
        return m_has_value;
    }
    
    T value() const {
        if (!m_has_value)
            throw BadOptionalAccess();
        return m_value;
    }


private:
    bool m_has_value;
    T m_value;

};












