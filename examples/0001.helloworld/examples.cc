#include <string>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>

using namespace ::std::string_literals;

struct point
{
	int x{};
	int y{};
};

template <typename char_type, typename output>
void print_define(::fast_io::io_reserve_type_t<char_type, point>, output out, point const &p)
{
	if constexpr (::std::is_same_v<char_type, char>)
	{
		::fast_io::println(out, "(", p.x, ",", p.y, ")");
	}
}

int main()
{
	::fast_io::println("Hello world!");
	::fast_io::string str{"string"};
	::fast_io::println(str);
	::fast_io::println(::fast_io::os_c_str(str.data()));
	::fast_io::println(::fast_io::pointervw(&str));
	::fast_io::println(::fast_io::rgvw(str, ","));
	::fast_io::println(::fast_io::chvw('c'));
	::fast_io::println('c'); // output : 99
	::fast_io::println(point{2, 5});

	::std::string s;
	::fast_io::scan(::fast_io::line_get(s));
	::fast_io::println(s);

	return 0;
}
