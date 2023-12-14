#include <fast_io_dsal/vector.h>

consteval auto test() {
    return fast_io::vector{2, 3, 4}[2];
}

int main() {
    test();
}