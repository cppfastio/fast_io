#include <fast_io_dsal/tuple.h>
#include <fast_io.h>
#include <type_traits>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

consteval void test_init() noexcept {
    constexpr ::fast_io::tuple t{1, 2};
    static_assert(::fast_io::get<0>(t) == 1);
    static_assert(::fast_io::get<1>(::fast_io::tuple{1, 2}) == 2);
    static_assert(::fast_io::is_tuple<decltype(t)>);
    static_assert(!::fast_io::is_tuple<int>);
    static_assert(::std::is_trivially_copy_constructible_v<decltype(t)>);
    static_assert(::std::is_trivially_move_constructible_v<decltype(t)>);
}

consteval void test_get() noexcept {
    constexpr auto t = ::fast_io::tuple{1., 2, 3.f};
    static_assert(::fast_io::get<double>(t) == 1.);
    static_assert(::fast_io::get<int>(t) == 2);
    static_assert(::fast_io::get<float>(t) == 3.f);
    static_assert(::fast_io::get<double>(::fast_io::tuple{1., 2, 3.f}) == 1.);
    static_assert(::fast_io::get<int>(::fast_io::tuple{1., 2, 3.f}) == 2);
    static_assert(::fast_io::get<float>(::fast_io::tuple{1., 2, 3.f}) == 3.f);
}

inline void test_structured_binding() noexcept {
    ::fast_io::tuple t{1, 2};
    auto const& [a, b] = t;
    if (a != 1 || b != 2) {
        ::fast_io::fast_terminate();
    }
}

int main() noexcept {
    test_structured_binding();
    return 0;
}
