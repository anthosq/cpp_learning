#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <initializer_list>

template <class T, class Alloc = std::allocator<T>>
struct Vector {
    using value_type = T;
    using allocator = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;
    using iterator = T *;
    using const_iterator = T const *;
    using reverse_iterator = std::reverse_iterator<T *>;
    using const_reverse_iterator = std::reverse_iterator<T const *>;

    T *m_data;
    size_t m_size;
    size_t m_cap;
    [[no_unique_address]] Alloc m_alloc;
    // 为了支持C++ 17内存池, pmr.allocator有状态，存了指向memory_resource的指针
    // 但是一般情况下普通allocator, 调用全局new和delete
    // 这种allocator无大小，STL中Vector继承Alloc, 使用空基类优化
    // 如果Alloc为空基类，直接写在成员里，会占据1个字节的空间
    // 由于前面的变量都是采用8字节对齐的
    // Alloc会因为空基类问题变成8字节，造成内存的浪费
    // 对此C++20提出[[no_unique_address]],我们可以在结构体里加空的类,让编译器放心把m_alloc编译为0字节

    Vector() noexcept {
        m_data = nullptr;
        m_size = 0;
        m_cap = 0;
    }

    /* explicit Vector(size_t n) { */
    /*     m_data = allocator{}.allocate(n); */
    /*     for (size_t i = 0; i < n; i++) { */
    /*         std::construct_at(&m_data[i]); */
    /*     } */
    /*     m_size = n; */
    /*     m_cap = n; */
    /* } */

    /* explicit Vector(size_t n, T const &val = 0) { */
    /*     m_data = allocator{}.allocate(n); */
    /*     for (size_t i = 0; i < n; i++) { */
    /*         std::construct_at(&m_data[i], val); */
    /*     } */
    /*     m_size = n; */
    /*     m_cap = n; */
    /* } */
    // std::allocate_shared, std::allocate_traits, 待学习

    Vector(std::initializer_list<T> ilist, Alloc const &alloc = Alloc()) 
    : Vector(ilist.begin(), ilist.end(), alloc) {}

    explicit Vector(size_t n, Alloc const &alloc = Alloc()) : m_alloc(alloc) {
        m_data = m_alloc.allocate(n);
        m_cap = m_size = n;
        for (size_t i = 0; i != n; i++) {
            std::construct_at(&m_data[i]);
        }
    }
    
    Vector(size_t n, T const &val, Alloc const &alloc = Alloc()) : m_alloc(alloc) {
        m_data = m_alloc.allocate(n);
        m_cap = m_size = n;
        for (size_t i = 0; i != n; i++) {
            std::construct_at(&m_data[i], val);
        }
    }

    template <std::random_access_iterator InputIt>
    Vector(InputIt first, InputIt last, Alloc const &alloc = Alloc()) : m_alloc(alloc) {
        size_t n = last - first;
        m_data = m_alloc.allocate(n);
        m_cap = m_size = n;
        for (size_t i = 0; i < n; i++) {
            std::construct_at(&m_data[i], *first);
            ++first;
        }
    }

    void clear() noexcept {
        for (size_t i = 0; i != m_size; i++) {
            std::destroy_at(&m_data[i]);
        }
    }

    void resize(size_t n) {
        if (n < m_size) {
            for (size_t i = n; i != m_size; i++) {
                std::destroy_at(&m_data[i]);
            }
            m_size = n;
        } else if (n > m_size) {
            reserve(n);
            for (size_t i = 0; i != n; i++) {
                std::construct_at(&m_data[i]);
            }
        }
        m_size = n;
    }

    void resize(size_t n, T const &val = 0) {
        if (n < m_size) {
            for (size_t i = n; i != m_size; i++) {
                std::destroy_at(&m_data[i]);
            }
            m_size = n;
        } else if (n > m_size) {
            reserve(n);
            for (size_t i = 0; i != n; i++) {
                std::construct_at(&m_data[i],val);
            }
        }
        m_size = n;
    }

    void shrink_to_fit() noexcept {
        auto old_data = m_data;
        auto old_cap = m_cap;
        m_cap = m_size;
        if (m_size == 0) {
            m_data = nullptr;
        } else {
            m_data = allocator{}.allocate(m_size);
        }
        if (old_cap != 0) [[likely]] {
            for (size_t i = 0; i != m_size; i++) {
                std::construct_at(&m_data[i], std::move_if_noexcept(old_data[i]));
                std::destroy_at(&old_data[i]);
            }
            allocator{}.deallocate(old_data, old_cap);
            // considering pmr, 传入old_cap
            // new和以前的allocator都把内存的释放与分配和构造与析构混到了一块，糟糕的设计
            // 现在 allocate与construct互相解耦
        }
    }

    void reserve(size_t n) {
        if (n <= m_cap) [[likely]] return;
        n = std::max(n, m_cap * 2);
        auto old_data = m_data;
        auto old_cap = m_cap;
        if (n == 0) {
            m_data = nullptr;
            m_cap = 0;
        } else {
            m_data = allocator{}.allocate(n);
            m_cap = n;
        }
        if (old_cap != 0) {
            for (size_t i = 0; i != m_size; i++) {
                std::construct_at(&m_data[i], std::move_if_noexcept(old_data[i]));
            }
            for (size_t i = 0; i != m_size; i++) {
                std::destroy_at(&old_data[i]);
            }
    /* return 0; */
            allocator{}.deallocate(old_data, old_cap);
        }
    }

    T *erase(T const *it) noexcept(std::is_nothrow_move_assignable_v<T>) {
        size_t i =  it - m_data;
        for (size_t j = i + 1; j < m_size; j++) {
            m_data[j - 1] = std::move(m_data[j]);
        }
        m_size -= 1;
        std::destroy_at(&m_data[m_size]);
        return const_cast<T *>(it);
    }

    T *erase(T const *first, T const *last) noexcept(std::is_nothrow_move_assignable_v<T>) {
        size_t diff = last - first;
        for (size_t j = last - m_data; j != m_size; j++) {
            m_data[j - diff] = std::move(m_data[j]);
        }
        m_size -= diff;
        for (size_t j = last - m_data; j != m_size; j++) {
            std::destroy_at(&m_data[j]);
        }
        return const_cast<T *>(first);
    }


    void push_back(T const &val) {
        if (m_size + 1 >= m_cap) [[unlikely]] {
            reserve(m_size + 1);
        }
        std::construct_at(&m_data[m_size], val);
        m_size = m_size + 1;
    }

    void push_back(T &&val) {
        if (m_size + 1 >= m_cap) [[unlikely]] {
            reserve(m_size + 1);
        }
        std::construct_at(&m_data[m_size], std::move(val));
        m_size = m_size + 1;
    }

    template<class ...Args>
    T &emplace_back(Args &&...args) {
        if (m_size + 1 >= m_cap) [[unlikely]] reserve(m_size + 1);
        T *p = &m_data[m_size];
        std::construct_at(p, std::forward<Args>(args)...);
        m_size += 1;
        return *p;
    }

    void swap(Vector &that) noexcept {
        std::swap(m_data, that.m_data);
        std::swap(m_size, that.m_size);
        std::swap(m_cap, that.m_cap);
    }

    T *data() noexcept {
        return *m_data;
    }

    T const *data() const noexcept {
        return *m_data;
    }

    T *begin() {
        return m_data;
    }

    T *end() {
        return m_data + m_size;
    }

    T *cbegin() {
        return m_data;
    }

    T *cend() {
        return m_data + m_size;
    }

    std::reverse_iterator<T *> rbegin() {
        return std::make_reverse_iterator(m_data);
    }

    std::reverse_iterator<T *> rend() {
        return std::make_reverse_iterator(m_data + m_size);
    }

    std::reverse_iterator<T *> crbegin() {
        return std::make_reverse_iterator(m_data);
    }

    std::reverse_iterator<T *> crend() {
        return std::make_reverse_iterator(m_data + m_size);
    }

    T const *begin() const {
        return m_data;
    }

    T const *end() const {
        return m_data + m_size;
    }

    T const *cbegin() const {
        return m_data;
    }

    T const *cend() const {
        return m_data + m_size;
    }

    std::reverse_iterator<T const *> rbegin() const {
        return std::make_reverse_iterator(m_data);
    }

    std::reverse_iterator<T const *> rend() const {
        return std::make_reverse_iterator(m_data + m_size);
    }

    std::reverse_iterator<T const *> crbegin() const {
        return std::make_reverse_iterator(m_data);
    }

    std::reverse_iterator<T const *> crend() const {
        return std::make_reverse_iterator(m_data + m_size);
    }

    T &back() noexcept {
        return m_data[m_size - 1];
    }

    T const &back() const noexcept {
        return m_data[m_size - 1];
    }


    T &front() noexcept {
        return *m_data;
    }

    T const &front() const noexcept {
        return *m_data;
    }

    size_t size() const noexcept {
        return m_size;
    }

    size_t capacity() {
        return m_cap;
    }

    T const &at(size_t i) const {
        if (i >= m_size) [[unlikely]] throw std::out_of_range("vector::at, out of range");
        return m_data[i];
    }

    T &at(size_t i) {
        if (i >= m_size) [[unlikely]] throw std::out_of_range("vector::at, out of range");
        return m_data[i];
    }
    T const &operator[](size_t i) const noexcept {
        return m_data[i];
    }

    T &operator[](size_t i) noexcept {
        return m_data[i];
    }

    Vector(Vector const &that) {
        if(m_data != that.m_data) {
            m_size = that.m_size;
            if (m_size != 0) {
                m_data = allocator{}.allocate(m_size);
                for (size_t i = 0; i != m_size; i++) {
                    std::construct_at(&m_data[i], std::as_const(that.m_data[i]));
                }
            } else {
                m_data = nullptr;
            }
        }
    }

    Vector &operator=(Vector const &that) {
        if (&that == this) [[unlikely]] return *this;
        reserve(that.m_size);
        for (size_t i = 0; i != m_size; i++) {
            std::construct_at(&m_data[i], std::as_const(that.m_data[i]));
        }
        return *this;
    }

    void assign(size_t n, const T &val) {
        clear();
        reserve(n);
        m_size = n;
        for (size_t i = 0; i < n; i++) {
            std::construct_at(&m_data[i], val);
        }
    }

    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(),ilist.end());
    }

    Vector &operator=(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    template <std::random_access_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        size_t n = last - first;
        reserve(n);
        m_size = n;
        for (size_t i = 0; i < n; i++) {
            std::construct_at(&m_data[i], *first);
            ++first;
        }
    }

    T *insert(T const *it, T &&val) {
        size_t j = it - m_data;
        reserve(m_size + 1);
        // j ~ m_size => j + 1 ~ m_size + 1
        for (size_t i = m_size; i != j; i--) {
            std::construct_at(&m_data[i], std::move(m_data[i - 1]));
            std::destroy_at(&m_data[i - 1]);
        }
        m_size += 1;
        std::construct_at(&m_data[j], std::move(val));
        return m_data + j;
    }

    T *insert(T const *it, T const &val) {
        size_t j = it - m_data;
        reserve(m_size + 1);
        // j ~ m_size => j + 1 ~ m_size + 1
        for (size_t i = m_size; i != j; i--) {
            std::construct_at(&m_data[i], std::move(m_data[i - 1]));
            std::destroy_at(&m_data[i - 1]);
        }
        m_size += 1;
        std::construct_at(&m_data[j], val);
        return m_data + j;
    }
    template <class ...Args>
    T *insert(T const *it, Args &&...args) {
        size_t j = it - m_data;
        reserve(m_size + 1);
        // j ~ m_size => j + n ~ m_size + n
        for (size_t i = m_size; i != j; i--) {
            std::construct_at(&m_data[i], std::move(m_data[i - 1]));
            std::destroy_at(&m_data[i - 1]);
        }
        m_size += 1;
        // mmove 会考虑指针aliasing
        std::construct_at(&m_data[j], std::forward<Args>(args)...);
        // 在j位置插入
        return m_data + j;
    }

    T *insert(T const *it, size_t n, T const &val) {
        size_t j = it - m_data;
        if (n == 0) [[unlikely]] return const_cast<T *>(it);
        reserve(m_size + n);
        // j ~ m_size => j + n ~ m_size + n
        for (size_t i = m_size; i != j; i--) {
            std::construct_at(&m_data[i + n - 1], std::move(m_data[i - 1]));
            std::destroy_at(&m_data[i-1]);
        }
        m_size += n;
        // mmove 会考虑指针aliasing
        for (size_t i = j; i < j + n; i++) {
            std::construct_at(&m_data[i], val);
        }
        return m_data + j;
    }

    template <std::random_access_iterator InputIt>
    T *insert(T const *it, InputIt first, InputIt last) {
        size_t j = it - m_data;
        size_t n = last - first;
        if (n == 0) [[unlikely]] return const_cast<T *>(it);
        reserve(m_size + n);
        // j ~ m_size => j + n ~ m_size + n
        for (size_t i = m_size; i != j; i--) {
            std::construct_at(&m_data[i + n - 1], std::move(m_data[i - 1]));
            std::destroy_at(&m_data[i - 1]);
        }
        m_size += n;
        // mmove 考虑了指针aliasing
        for (size_t i = j; i != j + n; i++) {
            std::construct_at(&m_data[i], *first);
            ++first;
        }
        return m_data + j;
    }

    T *insert(T const* it, std::initializer_list<T> ilist) {
        return insert(it, ilist.begin(), ilist.end());
    }

    Vector(Vector &&that) noexcept : m_alloc(std::move(that.m_alloc)) {
        m_data = that.m_data;
        m_size = that.m_size;
        m_cap = that.m_cap;
        that.m_data = nullptr;
        that.m_size = 0;
        that.m_cap = 0;
    }

    Vector(Vector &&that, Alloc const &alloc) noexcept : m_alloc(alloc) {
        m_data = that.m_data;
        m_size = that.m_size;
        m_cap = that.m_cap;
        that.m_data = nullptr;
        that.m_size = 0;
        that.m_cap = 0;
    }
    Vector &operator=(Vector &&that) noexcept {
        if (&that == this) [[unlikely]] return *this;
        for (size_t i = 0; i != m_size; i++) {
            std::destroy_at(&m_data[i]);
        }
        if (m_cap != 0) {
            m_alloc.deallocate(m_data, m_cap);
        }
        m_data = that.m_data;
        m_size = that.m_size;
        m_cap = that.m_cap;
        that.m_data = nullptr;
        that.m_size = 0;
        that.m_cap = 0;
        return *this;
    }

    Alloc get_allocator() const noexcept {
        return m_alloc;
    }

    bool operator==(Vector const &that) noexcept {
        return std::equal(begin(), end(), that.begin(), that.end());
    }

    bool operator<=>(Vector const &that) noexcept {
        return std::lexicographical_compare_three_way(begin(), end(), that.begin(), that.end());
    }

    ~Vector() {
        for (size_t i = 0; i != m_size; i++) {
            std::destroy_at(&m_data[i]);
        }
        if (m_cap != 0) {
            m_alloc.deallocate(m_data, m_cap);
        }
    }
};

int main() {
    Vector<int> arr;
    /* arr.reserve(16); */
    auto print_vector = [&] (Vector<int> &arr) {
        for (size_t i = 0; i != arr.size() ; i++)
            printf("arr[%zd]: %d\n", i, arr[i]);
    };
    auto show_vector = [&] (Vector<int> &arr) {
        printf("arr_size: %zd\n", arr.size());
        printf("arr_cap: %zd\n", arr.capacity());
    };
    for (int i = 0; i < 7; i++){
        printf("arr.push_back(%d)\n", i);
        arr.push_back(i);
    }
    arr.erase(arr.begin() + 4);
    arr.assign(5,42);
    print_vector(arr);
    show_vector(arr);
    Vector<int> first = {1, 2};
    arr.insert(arr.begin() + 2, {1, 2});
    print_vector(arr);
    show_vector(arr);
    arr.shrink_to_fit();
    show_vector(arr);
    auto brr = std::move(arr);
    print_vector(brr);
    struct Point {
        uint32_t x,y;
    };
    Vector<Point> p;
    p.emplace_back(1, 2);
    printf("size of Vector:%zd\n",sizeof(Vector<int>));
    return 0;
}
