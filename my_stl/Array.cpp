#include <array>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <iterator>
#include <type_traits>
#include <utility>

#if defined (_MSC_VER)
#define MYSTLL_UNREACHABLE() __assume(0)
#elif defined (__GNUC__)
#define MYSTLL_UNREACHABLE() __builtin_unreachable()
#endif

// WIP: CTAD {}; many functions; template<class T, 0>; to_array

template <class T,size_t N>
struct Array {
    using value_type = T;
    using iterator = T *;
    using const_iterator = T const *;



    T m_elements[N];

    T &at(size_t i) {
        if (i < 0 || i >= N) [[unlikely]]
            throw std::runtime_error("out of range");
        return m_elements[i];
    }

    T const &at(size_t i) const {
        if (i < 0 || i >= N) [[unlikely]] 
            throw std::runtime_error("out of range");
        //使用unlikely,把冷代码丢到.cold区间
        return m_elements[i];
    }

    T &operator[](int i) {
        return m_elements[i];
    }

    T const &operator[](int i) const noexcept {
        return m_elements[i];
    }
    static constexpr size_t size() noexcept {
        return N;
    }

    // concept RandomAccessorIterator:
    // *p
    // p->...
    // ++p p++
    // --p p--
    //p += n p -= n p + n p - n
    // p1 - p2
    // p1 != p2
    // p1 < p2 > <= >=

    /* struct iterator{}; */
    
    T *begin() noexcept {
        return m_elements;
    }

    T *end() noexcept {
        return m_elements + N;
    }

    T const *begin() const noexcept {
        return m_elements;
    }

    T const *end() const noexcept {
        return m_elements+N;
    }

    /* void fill(T const &val) noexcept(noexcept(std::declval<T &>() = std::declval<T> ())) { */
    /* void fill(T const &val) noexcept(noexcept(m_elements[0] = val)) { */
    /* void fill(T const &val) noexcept(std::is_nothrow_copy_assignable_v<T>) { */
    void fill(T const &val) noexcept(noexcept(T(val))) {
        for(size_t i{0}; i < N; i++) {
            m_elements[i] = val;
        }
    }
};
/* template <class Arg0, class ...Args> */
/* Array(Arg0, Args...) -> Array<Arg0, sizeof...(Args)+1>; */
void test(Array<int, 32> const &a) {
}

void test(Array<int32_t, 32> &a) {
    /* for(size_t i{0}; i < std::decay_t<decltype(a)>::size(); i++) */
    /*     printf("%d\n",a.at(i)); */
    for (auto it = a.begin(); it != a.end(); ++it)
        *it = it - a.begin();
    /* for (Array<int32_t, 32>::iterator it = a.begin(); it != a.end(); ++it) */
    /*     *it = it - a.begin(); */
}

void func(Array<int32_t, 32> &a) {
    size_t count = 0;
    std::as_const(a).begin();
    for(auto &ai: a) {
        ai = count++; // a[i] = i;
    }
}

int main() {
    Array<int32_t, 32> a{1,2,3};
    std::array<int32_t, 10> b{};
    /* std::iota(a); */
    /* test(a); */
    // for(auto it = a.begin(), eit = a.end()l it != eit; ++it)
    for (auto &ai: a) {
        std::cout << ai <<'\n';
    }
    puts("1");
    return 0;
}
