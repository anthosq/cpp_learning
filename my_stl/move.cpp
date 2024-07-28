#include <algorithm>
#include <cctype>
#include <iostream>
#include <type_traits>


template <class DecltypeT, class T>
constexpr std::remove_reference_t<T>&& move_impl(T &&obj) {
    static_assert(!std::is_const_v<std::remove_reference_t<T>>);
    static_assert(!std::is_lvalue_reference_v<DecltypeT>);
    /* static_assert(IsIdExpression); */
    return static_cast<std::remove_reference_t<T>&&>(obj);
}

constexpr bool is_id_expression_impl(char const* const expr) {
    for (auto str=expr; *str; ++str)
        if (!std::isalpha(*str) && !std::isdigit(*str)
            && *str != '_' && *str !=':')
            return false;
    return true;
}

#define is_id_expression(...) is_id_expression_impl(#__VA_ARGS__)

#define _move(...) move_impl<decltype(__VA_ARGS__)>(__VA_ARGS__)



struct Widget {

    void sink(Widget &&arg){
    }

};

void f(Widget &&arg) {
    _move(arg);
}

int main() {
    Widget arg;
    f(std::move(arg));
}
