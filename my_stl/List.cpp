#include <initializer_list>
#include <iostream>
#include <iterator>
#include <list>

/* template <class T> */
/* struct ListBaseNode { */
/*     ListBaseNode *m_next; */
/*     ListBaseNode *m_prev; */
/*  */
/*     inline T &value(); */
/*     inline T const &value() const; */
/* }; */
/*  */
/* template <class T> */
/* struct ListValueNode : ListBaseNode<T> { */
/*     union { */
/*         T m_value; */
/*     }; */
/* }; */



template <class T,class Alloc = std::allocator<T>>
struct List {
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;

    struct ListNode {
        T m_value;
        ListNode *m_next;
        ListNode *m_prev;
    };

    ListNode *m_head;

    List() noexcept {
        m_head = nullptr;
    }

    explicit List(size_t n) {
        if (n == 0) {
            m_head = nullptr;
            return;
        }
        ListNode *prev = new ListNode{};
        m_head = prev;
        m_head->m_value = T();
        for (size_t i = 1; i < n; i++) {
            ListNode *node = new ListNode{};
            prev->m_next = node;
            node->m_prev = prev;
            prev = node;
        }
        prev->m_next = m_head;
        m_head->m_prev = prev;
        // 循环链表，这样back可以实现为O(1)
    }

    explicit List(size_t n, T const &val) {
        if (n == 0) {
            m_head = nullptr;
            return;
        }
        ListNode *prev = new ListNode;
        m_head = prev;
        m_head->m_value = val;
        for (size_t i = 1; i < n; i++) {
            ListNode *node = new ListNode;
            prev->m_next = node;
            node->m_prev = prev;
            node->m_value = val;
            prev = node;
        }
        prev->m_next = m_head;
        m_head->m_prev = prev;
    }

    // C++20
    /* template <class InputIt> */
    /*     requires std::random_access_iterator<InputIt> */
    template <std::input_iterator InputIt>
    List(InputIt first, InputIt last) {
        if (first == last) {
            m_head = nullptr;
            return;
        }
        ListNode *prev = new ListNode{};
        m_head = prev;
        m_head->m_value = *first;
        ++first;
        while (first != last) {
            ListNode *node = new ListNode;
            prev->m_next = node;
            node->m_prev = prev;
            node->m_value = *first;
            prev = node;
            ++first;
        }
        prev->m_next = m_head;
        m_head->m_prev = prev;
    }
    // 链表使用bidirectional_iterator, 构造时只用输入迭代器就可以了
    // 如果想要使用it+n, 则使用std::advance(it, n);代替
    // input_iterator = *it it++ ++it it!=it it==it
    // output_iterator = *it=val it++ ++it it!=it it==it
    // forward_iterator = *it *it=val it++ ++it it!=it it==it
    // bidirectional_iterator = *it *it=val it++ ++it it-- --it it!=it it==it
    // random_access_iterator = *it *it=val it[n] it[n]=val it++ ++it it-- --it it+=n it-=n it+n it-n it!=it it==it

    List(std::initializer_list<T> ilist)
    : List(ilist.begin(), ilist.end()) {}

    template <class Visitor>
    void foreach(Visitor visitor) {
        if (m_head == nullptr) return;
        ListNode *curr = m_head;
        visitor(curr->m_value);
        curr = curr->m_next;
        while(curr != m_head) {
            visitor(curr->m_value);
            curr = curr->m_next;
        }
    }

    struct iterator {
        ListNode *curr;
        iterator &operator++() {
            curr = curr->m_next;
            return *this;
        }

        iterator operator++(int) noexcept {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        iterator &operator--() {
            curr = curr->m_prev;
            return *this;
        }

        iterator operator--(int) noexcept {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        T &operator*() {
            return curr->m_value;
        }

        bool operator!=(iterator const &that) const noexcept {
            return curr != that.curr;
        }

        bool operator==(iterator const &that) const noexcept {
            return !(*this != that);
        }

    };

    struct const_iterator {
        ListNode *curr;
        
        iterator &operator++() {
            curr = curr->m_next;
            return *this;
        }

        T &operator*() {
            return curr->m_value;
        }
    };

    iterator begin() {
        return iterator{m_head};
    }

    iterator end() {
        return iterator{m_head};
    }

    bool empty() noexcept {
        return m_head!=nullptr;
    }

    T &back() noexcept {
        return m_head->prev->m_value;
    }

    T &front() noexcept {
        return m_head->m_value;
    }

    T const &back() const noexcept {
        return m_head->prev->m_value;
    }

    T const &front() const noexcept {
        return m_head->m_value;
    }
};


int main() {
    List<int> arr{1, 2, 3, 4};
    size_t i = 0;
    arr.foreach([&] (int &val) {
        printf("arr[%zd] = %d\n", i, val);
        ++i;
    });
    i = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        int &val = *it;
        printf("arr[%zd] = %d\n", i, val);
    }
    return 0;
}
