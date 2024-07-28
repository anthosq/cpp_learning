#include <iostream>
#include <atomic>


/* auto b = a; // 不允许，要通过a.load()*/
/* int b = a; auto b = (int) a; auto b = a.load(); auto b = a.store();这几种可以*/

int main() {
    std::atomic<int> a = 6;
    /* std::atomic<int> a = std::atomic<int> 5; 禁止了拷贝与移动*/
    /* C++17复制消除->返回值复制消除;变量初始化拷贝构造->直接构造a(5),可以放心使用auto idom */
    
    std::cout << a << '\n';
    return 0;
}
