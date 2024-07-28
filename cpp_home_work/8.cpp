#include <array>
#include <format>
#include <type_traits>
#include <vector>
#include <functional>
#include <iostream>

template <class Ty,size_t size>
struct array {
    Ty* begin() { return arr; };
    Ty* end() { return arr + size; };
    Ty const* begin() const { return arr; };
    Ty const* end() const { return arr + size; };
    Ty arr[size];
};
/* C++17 use SFINAE */
/* (std::same_as<Arg0, Args> && ...) C++17折叠表达式，但Args为0自动变为false */
/* template <class Arg0, class ...Args> requires (sizeof...(Args) == 0 || (std::same_as<Arg0, Args> && ...)) */
template <class Arg0, class ...Args>
array(Arg0 arg0, Args...args) -> array<std::common_type_t<Arg0, Args...>, sizeof...(Args) + 1>;
/* array(Arg0 arg0, Args ...args) ->array<Arg0>; */
int main() {
    ::array arr{1, 3.14, 2.0, 4.2, 5};
    for (const auto& i : arr) {
        std::cout << i << '\n';
    } // 原理是硬编码begin和end,固定调用begin和end函数
    return 0;
}
