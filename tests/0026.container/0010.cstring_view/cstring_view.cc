#include <fast_io_dsal/cstring_view.h>
#include <fast_io.h>

int main()
{
	static_assert(!::std::constructible_from<::fast_io::cstring_view, ::fast_io::string_view>);
	static_assert(::std::constructible_from<::fast_io::string_view, ::fast_io::cstring_view>);
	::fast_io::cstring_view e;
	::fast_io::cstring_view stvw("hello \0worldwoo");
	::fast_io::string_view msv(stvw);
	char buffer[40];
	::fast_io::io::println(stvw, ::fast_io::mnp::os_c_str(e.c_str()), "\n",
						   stvw == msv,
						   stvw == "hello world\n", "\n",
						   "hello world\n" == stvw, "\n",
						   stvw <=> "hello world\n", "\n",
						   "hello world\n" <=> stvw, "\n",
						   stvw.starts_with("hello"), "\n",
						   stvw.ends_with("\0world\n"), "\n",
						   stvw.substrvw(5), "\n",
						   stvw.substrvw(5, 3), "\n",
						   stvw.find_character('w'), "\n",
						   stvw.find_character('a'), "\n",
						   stvw.find("\0world"), "\n",
						   stvw.rfind("wo"), "\n",
						   stvw.contains("world"), "\n",
						   ::fast_io::string_view(buffer, stvw.copy(buffer, 6, 2)), "\n",
						   stvw.rfind_character('o'), "\n",
						   stvw.rfind_not_character('o'), "\n",
						   stvw.compare_three_way(0, 4, "hell", 4));
}
