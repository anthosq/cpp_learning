#include <format>
#include <iostream>
#include <string>

struct Int {
    int i;
};

Int operator""_it(unsigned long long i) {
    return {static_cast<int>(i*i)};
}

auto operator""_mystr(const char *s, size_t len) {
    return [=] (auto &&...args) {
        auto fmt = std::string_view(s, len);
        return std::vformat(fmt, std::make_format_args(args...));
    };
}

int main() {
    auto i = 40_it;
    std::cout << "你是{}个{}个"_mystr(5,5) << '\n';
    return 0;
}
