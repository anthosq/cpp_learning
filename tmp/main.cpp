#include <print>
#include <iostream>
#include <type_traits>

// C++ 20
template<class T>
void try_call_foo(T &t) {
    if constexpr (requires { t.foo(); })
        t.foo();
}

// C++ 17
template <class T, class = void>
struct has_foo : std::false_type {};

template <class T>
struct has_foo<T, std::void_t<decltype(std::declval<T>().foo())>> : std::true_type {};

template <class T>
void try_call_foo2(T &t) {
    if constexpr (has_foo<T>::value)
        t.foo();
}

struct A {
    void foo() {
        std::printf("A: foo()\n");
    }
};

// C++ 14
template <class T, class = void>
struct has_foo2 : std::false_type {};

template <class T>
struct has_foo2<T, std::void_t<decltype(std::declval<T>().foo())>> : std::true_type {};

template <class T, std::enable_if_t<has_foo2<T>::value, int> = 0>
void try_call_foo3(T &t) {
    t.foo();
}

int main() {
    A a;
    try_call_foo(a);
    try_call_foo2(a);
    try_call_foo3(a);
    return 0;
}
