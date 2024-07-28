#include <cstdio>
#include <deque>


void authenticateUser() {
}

template<class Container, class Index>
decltype(auto)
authAndAccess(Container& c, Index i) {
    authenticateUser();
    return c[i];
}

int main() {
    std::deque<int> d{1,2,3,4,5,6,7};
    authAndAccess(d, 5) = 10;
    printf("%d\n",d[5]);
    return 0;
}
