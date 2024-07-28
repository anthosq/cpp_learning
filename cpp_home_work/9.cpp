#include <chrono>
#include <cstdint>
#include <iostream>
#include <coroutine>

struct RepeatAwaiter {
    bool await_ready() const noexcept { return false; }
    
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept {
        if (coroutine.done())
            return std::noop_coroutine();
        else
            return coroutine;
    }

    void await_resume() const noexcept {}
};

struct RepeatAwaitable {
    RepeatAwaiter operator co_await() {
        return RepeatAwaiter();
    }
};

struct PreviousAwaiter {
    std::coroutine_handle<> mPrevious;

    bool await_ready() const noexcept { return false;}

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept {
        if(mPrevious)
            return mPrevious;
        else
            return std::noop_coroutine();
    }

    void await_resume() const noexcept {}
};

struct Promise {
    auto initial_suspend() {
        return std::suspend_always();
    }

    auto final_suspend() noexcept {
        return std::suspend_always();
    }

    void unhandled_exception() {
        throw std::runtime_error("!unhandled_exception!");
    }

    auto yield_vaule(int ret) {
        mRetValue = ret;
        return RepeatAwaiter();
    }

    void return_void() {
        mRetValue = 0;
    }
    
    std::coroutine_handle<Promise> get_return_object() {
        return std::coroutine_handle<Promise>::from_promise(*this);
    }

    int mRetValue;
};

struct Task {
    using promise_type = Promise;
    Task(std::coroutine_handle<promise_type> _coroutine)
    : mCoroutine(_coroutine) {}
    
    Task() = default;
    Task(Task &&) = delete;
    ~Task() {
        mCoroutine.destroy();
    }

    std::coroutine_handle<promise_type> mCoroutine;
};
/* constexpr uint32_t switch_test = 2e9; */
/* Task cycle(int sec, const char *message) { */
/*     while (true) { */
/*         printf("%s\n", message); */
/*     } */
/* } */
/* void test() { */
/*     std::coroutine_handle<Promise> h = [i = 0](int i) -> std::coroutine_handle(Promise) { */
/*         std::cout<< i <<'\n'; */
/*         co_return; */
/*     }(0); */
/*     h.await_resume() */
/*     h.destroy(); */
/* } */

int main() {
    
    return 0;
}
