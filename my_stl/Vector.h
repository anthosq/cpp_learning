
#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <utility>
#include <initializer_list>






// follow the cppreference

template <class T, class Alloc = std::allocator<T>>
struct Vector {
    using value_type = T;
    using allocator_type = Alloc;
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
    

    [[no_unique_address]] Alloc m_alloc;
    Vector() noexcept {
        m_data = nullptr;
        m_size = 0;
        m_cap = 0;
    }

    Vector(std::initializer_list<T> ilist, Alloc const &alloc = Alloc())
    : Vector(ilist.begin(), ilist.end(), alloc) {}

    explicit Vector(size_t n, Alloc const &alloc = Alloc())
    : m_alloc(alloc) {
        m_data = m_alloc.allocate(n);
        m_cap = m_size = n;
        for (size_t i = 0; i != n; i++) {
            std::construct_at(&m_data[i]);
        }
    }

    Vector(size_t n, T const &val, Alloc const &alloc = Alloc())
    : m_alloc(alloc) {
        m_data = m_alloc.allocate(n);
        m_cap = m_size = n;
        for (size_t i = 0; i != n; i++) {
            std::construct_at(&m_data[i], val);
        }
    }

    template <std::random_access_iterator InputIt>
    Vector(InputIt first, InputIt last, Alloc const &alloc = Alloc())
    : m_alloc(alloc) {
        size_t n = last - first;
        m_data = m_alloc.allocate(n);
        m_cap = m_size = n;
        for (size_t i = 0; i != n; i++) {
            std::construct_at(&m_data[i], *first);
            ++first;
        }
    }

    void clear() noexcept {
        for (size_t i = 0; i != m_size; i++) {
            std::destroy_at(&m_data[i]);
        }
        m_size = 0;
    }

    void resize(size_t n) {}



    T *m_data;
    size_t m_size;
    size_t m_cap;
};
