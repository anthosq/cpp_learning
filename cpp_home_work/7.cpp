#include <cstdlib>
#include <exception>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <string>
#include <atomic>

struct MyException : std::exception {
    const char *s;
    MyException(const char *s_) : s(s_) {
    }
    ~MyException() {
    }
    const char *what() const noexcept {
        return s;
    }
};

struct Error {
    virtual const char *What() const noexcept {
        return "Error";
    }
    virtual ~Error() = default;
};

struct HttpError : Error {
    virtual const char * What() const noexcept override {
        return "404 NotFound!";
    }
};

void test() {
    /* throw std::runtime_error("MyException error"); */
    throw HttpError();
    throw 1;
    throw MyException("MyException error");
    /* throw出来的对象在.eh_frame section中不需要去管理内存 */
}
/* noexcept的作用就是丢掉冷代码，空间问题, LLVM到处加noexcept就是为了减小空间 */

void catcher(auto test) {
    try {
        test();
    } catch (...) {
        if(!std::current_exception()) puts("null");
        std::rethrow_exception(std::current_exception()); // 返回一个exception_ptr(类型擦除容器，异常版any),不能直接扔出去，不然和普通数字没区别
    }
}

int main() {
    std:: exception e = MyException("main error"); // object slicing, 如果直接puts(e.what())会调用默认的what()，显示std::exception
    // 引用的话，构造时会调用exception的构造函数，指向exception的what(),在赋值的时指向我们子类实现的exception的what()
    try {
        catcher(test);
    } catch (Error &e) {    
        puts(e.What());
    } catch (...) {
        puts("unknown error");
    }
    /* 等效于std::exception const &e = std::runtime_error("..."); */
    /* 子类的引用坍缩成父类的引用 */
    /* std::exception const &e也可以写成std::runtime_error const &e,不过损失了多态的能力，抛出别的就捕获失败，就会进入std::terminate */
    /* std::terminate();会调用std::abort();, std::abort()会调用raise(SIGABRT),操作系统若没有设置SIGABRT行为，默认杀死该进程 */
}
