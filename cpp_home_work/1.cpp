#include <type_traits>
#include <vector>
#include <functional>
#include <iostream>
#include <string>
#include <concepts>
 
//template <class T>
//inline constexpr bool false_v = false;

//C++17
//template <class T, class Func>
//auto &operator|(std::vector<T> &vec, Func const& func) {
    //if constexpr (std::is_invocable_v<Func,T &>){
        //for(auto &element: vec){
            //func(element);
        //}    
    //}
    //else {
        //static_assert(false_v<Func>,"not a func")
    //}
    //return vec;
//}

//C++20
//template<class T, class Func> requires (std::is_invocable_v<Func, T &>)
//auto &operator|(std::vector<T> &vec, Func const &func) {
    //for (auto &element: vec) {
        //func(element);
    //}
    //return vec;
//}

//better
//template<class T, std::invocable<T &> Func> 
template<class T>
auto &operator|(std::vector<T> &vec, std::invocable<T &>auto const &func) {
    for (auto &element: vec) {
        func(element);
    }
    return vec;
}

int main() {
    std::vector vec{1.0,2.0,3.0};
    auto printer = [] (auto const &i) {
        std::cout<< "printer1: " << i << '\n';
    };
    auto modifier = [] (auto &i) {
        i *= i;
    };
    vec | modifier | printer;
    return 0;
}
