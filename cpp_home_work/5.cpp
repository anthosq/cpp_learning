#include <cstdio>
#include <cassert>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <tuple>
#include <utility>

/* 考虑function容器类型擦除低效，改用template */
/* struct scope_guard { */
/*     std::function<void()> f; */
/*     scope_guard(std::function<void()> f_) : f(f_){ */
/*         puts("constructor"); */
/*     } */
/*     ~scope_guard() { */
/*         puts("deconstructor"); */
/*         f(); */
/*     } */
/* }; */

/* template实现 */
template<class F, class ...Args>
struct scope_guard {
    F f;
    std::tuple<Args...> args;
    // ...Args表示定义，args...表示使用
    // 保存的args,下面args_对应捕获的args, 但注意，此处args已经转换为了tuple
    // 此处都是转发，前面万能引用，转发可以自动匹配拷贝或移动
    scope_guard(F f_, Args ...args_) : f(std::move(f_)), args(std::move(args_)...) {
    }
    ~scope_guard() noexcept { // 其实自带noexcept，operator delete也是
        std::apply(f, args);
        // 所以此处使用apply, 因为args已经为tuple
        // 相当于 std::invoke(f(get<0>(args), get<1>(args), ...));
        puts("deconstruct successfullly");
    }
};

/* C++17 CTAD guide */
template <class F, class ...Args>
scope_guard(F f, Args ...args) -> scope_guard<F, Args...>;
/* C++20对于构造函数正好就是模板参数的情况自动隐含上述规则，不用写 */
/* 但如果比较复杂，会认不出来，还是得手动写 */

struct ErrorBase {
    virtual const char *What() const noexcept {
        return "error undefine";
    }
    virtual ~ ErrorBase() = default;
};

struct HttpError : ErrorBase {
    virtual const char *What() const noexcept {
        return "404 not found!";
    }
};

int tmain() {
    puts("enter main");
    {
        puts("open file");
        FILE *fp = fopen("./CMakeLists.txt","r");
        printf("open file: %p\n",fp);
        /* auto f = [&] { */
        /*     puts("close file"); */
        /*     fclose(fp); */
        /* }; */
        /* C++17 CTAD之前, 要手动decltype出f的具体类型(匿名lambda) */
        /* scope_guard<decltype(f)> a{f}; */
        struct Test {
            /* FILE *fp; // [=] */
            void f(FILE *fp) const {
                puts("test called");
                printf("close file:%p\n",fp);
                fclose(fp);
            }
        };
        /* scope_guard a{[&] { */
        /*     puts("close file"); */
        /*     fclose(fp); */
        /* }}; */
        Test t;
        auto membf = &Test::f;
        /* (t.*membf)(fp); */
        /* std::invoke(membf, t, fp); */
        // void (Test::*)(FILE *, int &)
        // void (*)(FILE *, int &)
        scope_guard a{&Test::f, Test{}, fp};
        // 析构函数不能有异常
        /* throw std::bad_alloc(); */
        /* throw std::runtime_error("error and exit"); */
        throw HttpError();
        if(1) {
            puts("return unexpectedly");
            return 2;
        }
    }
    puts("left main");
    return 0;
}


void catcher(auto test) {
    try {
        test();
    } catch (...) {
        if (!std::current_exception()) puts("null");
        std::rethrow_exception(std::current_exception());
    }
}
int main() {
    try {
        catcher(tmain);
    } catch (ErrorBase const &e) {
        puts(e.What());
    } catch (...) {
        puts("unknown error");
    }
}
