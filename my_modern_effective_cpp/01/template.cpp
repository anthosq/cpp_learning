#include <iostream>
#include <vector>

template <class T>
T max(T a, T b) {
    return a > b ? a : b;
}
decltype(auto) max(const auto& a, const auto& b)  {
    return a > b ? a : b;
}
int main() {
    return 0;
}
