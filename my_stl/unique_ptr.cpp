#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <type_traits>
#include <utility>

template <class T> 
struct Deleter {
    void operator()(T *p) { delete p; }
};

template <class T> 
struct Deleter<T[]> {
    void operator()(T *p) { delete[] p; }
};

/* template <> struct Deleter<FILE> { */
/*   void operator()(FILE *p) { fclose(p); } */
/* }; */

// 同std::exchange, 交换dst和val, 返回原dst的值
// 用于跟nullptr交换很方便
template <class T, class U> 
T exchange(T &dst, U &&val) {
    T tmp = std::move(dst);
    dst = std::forward<U>(val);
    return tmp;
}

// STL中为了让lambda能够捕获外部的指针，因此还使用了Deleter空基类优化，避免将Deleter作为unique_ptr的成员而产生额外的字节开销。
template <class T, class Deleter = Deleter<T>> 
struct UniquePtr {
private:
    template <class U, class UDeleter> friend struct UniquePtr;
    // 方便互相转换

    T *m_p;

public:
    UniquePtr(std::nullptr_t dafault = nullptr) { m_p = nullptr; }

    explicit UniquePtr(T *_p) { m_p = _p; }
    // 显式构造，避免发生什么栈上变量发生隐式转换，delete栈上的指针出错
    // C++20 前
    // template <class U, class UDeleter, class
    // std::enable_if_t<std::is_convertible_v<U *, T *>>> C++20 后
    template <class U, class UDeleter>
    requires(std::convertible_to<U *, T *>)
    UniquePtr(UniquePtr<U, UDeleter> &&that) {
        m_p = ::exchange(that.m_p, nullptr);
    }

    ~UniquePtr() {
        if (m_p)
            Deleter{}(m_p);
    }

    UniquePtr(UniquePtr const &that) = delete;
    UniquePtr &operator=(UniquePtr const &that) = delete;

    UniquePtr(UniquePtr &&that) {
        m_p = ::exchange(that, nullptr);
        // 构造，不用free
    }
    UniquePtr &operator=(UniquePtr &&that) {
        // 常见小知识，判断this和that是否相等，避免重复释放
        if (this != &that) [[likely]] {
            if (m_p)
                Deleter{}(m_p);
            // 构造Deleter对象然后调用
            // 先释放m_p,避免原来m_p的内容泄漏
            m_p = ::exchange(that.m_p, nullptr);
            // m_p存储that.m_p
        }
        // 相等就直接返回this
        return *this;
    }

    T *get() const { return m_p; }

    T *release() { return ::exchange(m_p, nullptr); }

    void reset(T *p = nullptr) {
        if (m_p)
            Deleter{}(m_p);
        m_p = p;
    }

    T &operator*() const { return *m_p; }

    T *operator->() const { return m_p; }
};

template <class T, class Deleter>
struct UniquePtr<T[], Deleter> : UniquePtr<T, Deleter> {};
// 析构时调用Deleter<T[]>

template <class T, class... Args> 
UniquePtr<T> makeUnique(Args &&...args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <class T> 
UniquePtr<T> makeUniqueForOverwrite() {
    // 等同于std::make_unique_for_overwrite, 不初始化里面的值
    return UniquePtr<T>(new T);
}

struct Test {
    Test() { puts(__PRETTY_FUNCTION__); }
    ~Test() noexcept { puts(__PRETTY_FUNCTION__); }
    /* Test(Test const &_test) { */
    /*     puts(__PRETTY_FUNCTION__); */
    /* } */
    /* Test(Test const &_test) = delete; */
    /* Test &operator=(Test const &_test) = delete; */
    /*  */
    /* Test(Test &&_test) = delete; */
    /* Test &operator=(Test &&_test) = delete; */
};

int main() {
    {
        puts("enter domain");
        auto tst = Test();
        std::unique_ptr a = std::make_unique<Test>(tst);
        puts("left domain");
    }
    puts("return");
    return 0;
}
