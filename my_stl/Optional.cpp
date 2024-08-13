#include <iostream>
#include <optional>
#include "Optional.hpp"


struct C {
    C(int x, int y) : m_x(x), m_y(y) {}
    
    int m_x;
    int m_y;
};

void test_ours() {
    Optional<C> opt(C(1, 2));
    Optional<C> opt3(Nullopt);
    Optional<int> opt1(1);

    std::cout << opt1.has_value() << '\n';
    std::cout << opt1.value() << '\n';

    Optional<int> opt2(Nullopt);
    std::cout << opt2.has_value() << '\n';
    std::cout << opt2.value() << '\n';
}

void test_std() {
    std::optional<int> opt(1);
    std::cout << opt.has_value() << '\n'; // true
    std::cout << opt.value() << '\n'; // 1
    std::cout << opt.value_or(0) << '\n'; // 1

    std::optional<int> opt2(std::nullopt);
    std::cout << opt2.has_value() << '\n'; // false
    try {
        opt2.value();
    } catch (std::bad_optional_access const &) {
        std::cout << "opt2 exception ok\n";
    }
    std::cout << opt2.value_or(0) << '\n'; // 0

    opt2 = 42;
    std::cout << opt2.has_value() << '\n'; // true
    std::cout << opt2.value() << '\n'; // 42
    std::cout << opt2.value_or(0) << '\n'; // 42
}

int main() {
    test_ours();
    // test_std();
    return 0;
}
