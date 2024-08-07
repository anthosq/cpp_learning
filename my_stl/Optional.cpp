#include <iostream>
#include <optional>
#include "Optional.hpp"


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

void test_ours() {
    Optional<int> opt1(1);

    std::cout << opt1.has_value() << '\n';
    std::cout << opt1.value() << '\n';

    Optional<int> opt2(Nullopt);
    std::cout << opt2.has_value() << '\n';
    std::cout << opt2.value() << '\n';
}


int main() {
    test_ours();
    // test_std();
    return 0;
}
