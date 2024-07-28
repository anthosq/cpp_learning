#include <format>
#include <iostream>
#include <string_view>
struct Frac {
    int a,b;
};

template<>
struct std::formatter<Frac, char> {
    std::formatter<int, char> format_int;

    constexpr typename std::basic_format_parse_context<char>::iterator
    parse(std::basic_format_parse_context<char> &pc) {
        std::cout << string_view(pc.begin(), pc.end()-pc.begin()) << '\n'; 
        return format_int.parse(pc);
    }
    template <class Out>
    constexpr typename std::basic_format_context<Out, char>::iterator
    format(Frac const &value, std::basic_format_context<Out, char> &fc) const {
        format_int.format(value.a, fc);
        *fc.out() = '/';
        return format_int.format(value.b, fc);
    }
};

template<class ...Args>
void print(std::string_view fmt, Args &&...args) {
    auto str = vformat(fmt, std::make_format_args(args...));
    std::cout << str <<'\n';
}

int main() {
    Frac f{1, 10};
    print("({:^10})",f);
    return 0;
}
