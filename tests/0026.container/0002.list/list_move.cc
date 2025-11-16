
#include <fast_io_dsal/list.h>

int main() {
    ::fast_io::list<int> l1{};
    if (!l1.is_empty()) {
        ::fast_io::fast_terminate();
    }

    ::fast_io::list<int> l2(::std::move(l1));
    if (!l2.is_empty()) {
        ::fast_io::fast_terminate();
    }

    ::fast_io::list<int> l3{};
    l3.emplace_back(1);
    if (l3.is_empty()) {
        ::fast_io::fast_terminate();
    }

    ::fast_io::list<int> l4(::std::move(l3));
    if (l4.is_empty()) {
        ::fast_io::fast_terminate();
    }

    return 0;
}
