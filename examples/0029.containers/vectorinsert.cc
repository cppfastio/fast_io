#include <fast_io.h>
#include <fast_io_dsal/vector.h>

using namespace fast_io::io;

#include <vector>
#include <sstream> // for some iterator that does not satisfy forward_iterator
#include <iterator>

struct test_t
{
    constexpr test_t() noexcept : content{} {}
    constexpr test_t(std::size_t i) noexcept : content{i} {}
    constexpr test_t(test_t const &other) noexcept : content{other.content} {}
    constexpr test_t &operator=(test_t const &other) noexcept
    {
        content = other.content;
        return *this;
    }
    constexpr test_t(test_t &&other) noexcept : content{other.content} {}
    constexpr test_t &operator=(test_t &&other) noexcept
    {
        content = other.content;
        return *this;
    }
    constexpr ~test_t() noexcept {}
    std::size_t content;
};

template <typename char_type, typename out_type>
inline constexpr void print_define(fast_io::io_reserve_type_t<char_type, test_t>, out_type out,
                                   test_t const &t) noexcept
{
    fast_io::operations::print_freestanding<false>(out, t.content);
}

int main()
{
    std::vector v{1, 2, 3, 4, 5};
    std::istringstream s;
    std::vector v1{3, 4, 5};
    fast_io::vector v2{3, 4, 5};
    println("-----");

    v1.insert(v1.begin(), 3);
    v2.insert(v2.begin(), 3);
    println(fast_io::mnp::rgvw(v1, " "));
    println(fast_io::mnp::rgvw(v2, " "));
    println("-----");

    v1.insert(v1.begin() + 1, {3, 4, 5});
    v2.insert(v2.begin() + 1, {3, 4, 5});
    println(fast_io::mnp::rgvw(v1, " "));
    println(fast_io::mnp::rgvw(v2, " "));
    println("-----");

    v1.insert(v1.begin(), v.begin(), v.end());
    v2.insert(v2.begin(), v.begin(), v.end());
    println(fast_io::mnp::rgvw(v1, " "));
    println(fast_io::mnp::rgvw(v2, " "));
    println("-----");

    s = std::istringstream{"1 2 3 4 5"};
    v1.insert(v1.end() - 1, std::istream_iterator<int>{s}, std::istream_iterator<int>{});
    s = std::istringstream{"1 2 3 4 5"};
    v2.insert(v2.end() - 1, std::istream_iterator<int>{s}, std::istream_iterator<int>{});
    println(fast_io::mnp::rgvw(v1, " "));
    println(fast_io::mnp::rgvw(v2, " "));
    println("-----");

    std::vector v3{test_t{3}, test_t{4}, test_t{5}};
    fast_io::vector v4{test_t{3}, test_t{4}, test_t{5}};
    println("-----");

    v3.insert(v3.begin(), 3);
    v4.insert(v4.begin(), 3);
    println(fast_io::mnp::rgvw(v3, " "));
    println(fast_io::mnp::rgvw(v4, " "));
    println("-----");

    v3.insert(v3.begin() + 1, {3, 4, 5});
    v4.insert(v4.begin() + 1, {3, 4, 5});
    println(fast_io::mnp::rgvw(v3, " "));
    println(fast_io::mnp::rgvw(v4, " "));
    println("-----");

    v3.insert(v3.begin(), v.begin(), v.end());
    v4.insert(v4.begin(), v.begin(), v.end());
    println(fast_io::mnp::rgvw(v3, " "));
    println(fast_io::mnp::rgvw(v4, " "));
    println("-----");

    s = std::istringstream{"1 2 3 4 5"};
    v3.insert(v3.end() - 1, std::istream_iterator<int>{s}, std::istream_iterator<int>{});
    s = std::istringstream{"1 2 3 4 5"};
    v4.insert(v4.end() - 1, std::istream_iterator<int>{s}, std::istream_iterator<int>{});
    println(fast_io::mnp::rgvw(v3, " "));
    println(fast_io::mnp::rgvw(v4, " "));
    println("-----");
}
