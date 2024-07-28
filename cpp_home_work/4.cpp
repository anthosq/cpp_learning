#include <cstddef>
#include <cstdio>
#include <format>
#include <vector>
#include <functional>
#include <iostream>
#include <string>

/* static int noob =  std::puts("before main\n"); */
/* 上面是在main之前对int进行初始化,int没有析构 */

/* 跨平台 */
/* struct Noob { */
/*     Noob() { */
/*         puts("noob before main"); */
/*     } */
/*     ~Noob() { */
/*         puts("noob after main"); */
/*     } */
/* }; */
/* Noob nb; */

/*  gcc 静态初始化 */
/*  在__crt_main函数里面遍历所有object文件, */
/*  constructor会被放到init.ctor的段里， */
/*  该段里全是函数指针，__crt_main搜索一遍函数指针的表，然后把里面所有东西都执行一遍 */
/*  */
/* __attribute__((constructor)) void before_main() { */
/*     puts("before main"); */
/* } */
/*  */
/* __attribute__((destructor)) void after_main() { */
/*     puts("after main"); */
/* } */
struct Compbase {
    inline static size_t count = 0;
};

template<class T>
struct Comp :Compbase {
    inline static const size_t id = count++;
    static constexpr size_t component_type_id() {
        return id;
    }
};

struct A : Comp<A> {};
struct B : Comp<B> {};
struct C : Comp<C> {};

void func() {
    static int dummy = puts("func first launch");
    puts("func launch");
}
/* CRTP */
/* class ComponentBase { */
/* protected: */
/*     static inline size_t component_type_count = 0; */
/* }; */
/*  */
/* template <class T> */
/* class Component : public ComponentBase { */
/*      */
/* }; */


int main () {
    std::printf("A: %zd\n", A::component_type_id());
    std::printf("B: %zd\n", B::component_type_id());
    std::printf("C: %zd\n", C::component_type_id());
    std::printf("A: %zd\n", A::component_type_id());
    /* func(); */
    /* func(); */
    return 0;
}




