#pragma once

#include <utility>
#include <stdexcept>
#include <memory>
#include <type_traits>
#include <functional>


template <class FnSig>
struct Function {
    static_assert(!std::is_same_v<FnSig, FnSig>, "not a valid function signature");
};

template <class Ret, class ...Args>
struct Function<Ret(Args...)> {
    private:
    struct FuncBase {
        virtual Ret call(Args ...args) = 0;
        virtual ~FuncBase() = default;
    };

    template <class F>
    struct FuncImpl : FuncBase {
        FuncImpl(F _f) : m_f(std::move(_f)) {}

        virtual Ret call(Args...args) override {
            return std::invoke(m_f, std::forward<Args>(args)...);
            /* return m_f(std::forward<Args>(args)...); */
            /* 一样的作用，在另一次题目中我们使用tuple args接受了参数 */
            /* 然后使用std::apply(m_f,args);来完成函数的调用, */
            /* 实质也是使用了std::invoke */
        }

        F m_f;
    };
    std::shared_ptr<FuncBase> m_base;
    // 使用share_ptr, 让Function支持拷贝

public:
    Function() = default;
    template <class F, class = std::enable_if_t<std::is_invocable_r_v<Ret, F &, Args...>>>
    // 使用enable_if_t阻止Function从不可调用的对象中初始化
#if  __cplusplus > 202002L
    template <class F> requires (std::is_invocable_v<Ret, F &, Args...>)
    // C++20 使用requires也能实现相同的约束
#endif
    Function(F _f) : m_base(std::make_shared<FuncImpl<F>>(std::move(_f))) {}
    // 不是用explict,允许lambda隐式转换为function
    Ret operator() (Args ...args) const {
        if(!m_base) [[unlikely]]
            throw std::runtime_error("function uninitialized");
        return m_base->call(std::forward<Args>(args)...);
        // 完美转发，即便Args中存在引用也不产生额外拷贝开销
    }

};

/* struct print_arg { */
/*     void operator()() const { */
/*         printf("Numbers are: %d, %d\n", x, y); */
/*     } */
/*     // lambda的实质 */
/*     int &x; */
/*     int &y; */
/* }; */
/*  */
/* template <class Func> */
/* void call_twi(const Func &f) { */
/*     f(); */
/*     f(); */
/* } */
/*  */
/* int main() { */
/*     int x, y; */
/*     std::cin >> x >> y; */
/*     print_arg a{x, y}; */
/*     call_twi(a); */
/*     call_twi([&x, &y] () { */
/*         printf("Numbers are: %d, %d\n",x, y); */
/*     }); */
/*  */
/*     return 0; */
/* } */
