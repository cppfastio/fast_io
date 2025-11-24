#include <fast_io_dsal/list.h>
#include <fast_io.h>

struct X {
    X() {
        ::fast_io::println("default");
    }

    X(X const&) {
        ::fast_io::println("copy");
    }

    X(X&&) {
        ::fast_io::println("move");
    }

    ~X() {
        ::fast_io::println("destruct");
    }

    X& operator=(X const&) {
        ::fast_io::println("copy assign");
        return *this;
    }

    X& operator=(X&&) {
        ::fast_io::println("move assign");
        return *this;
    }
};

int main() {
    // TODO copy X here, can it be fixed? (caused by std::initilizer_list)
    ::fast_io::list<X> const l1{X{}};

    return 0;
}
