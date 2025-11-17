#include <fast_io_dsal/list.h>

int main() {
    ::fast_io::list<int> const l{};
    if (!l.is_empty()) {
        ::fast_io::fast_terminate();
    }
    if (l.is_single()) {
        ::fast_io::fast_terminate();
    }
    if (l.has_multiple()) {
        ::fast_io::fast_terminate();
    }

    ::fast_io::list<int> const l2{1};
    if (l2.is_empty()) {
        ::fast_io::fast_terminate();
    }
    if (!l2.is_single()) {
        ::fast_io::fast_terminate();
    }
    if (l2.has_multiple()) {
        ::fast_io::fast_terminate();
    }

    ::fast_io::list<int> const l3{1, 2};
    if (l3.is_empty()) {
        ::fast_io::fast_terminate();
    }
    if (l3.is_single()) {
        ::fast_io::fast_terminate();
    }
    if (!l3.has_multiple()) {
        ::fast_io::fast_terminate();
    }

    ::fast_io::list<int> l4{1, 2, 3};
    if (l4.is_empty()) {
        ::fast_io::fast_terminate();
    }
    if (l4.is_single()) {
        ::fast_io::fast_terminate();
    }
    if (!l4.has_multiple()) {
        ::fast_io::fast_terminate();
    }

    return 0;
}
