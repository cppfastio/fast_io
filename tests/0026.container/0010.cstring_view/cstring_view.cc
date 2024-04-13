#include <fast_io_dsal/cstring_view.h>
#include <fast_io.h>

int main()
{
	::fast_io::cstring_view stvw("hello \0worldwo");
	char buffer[40];
	::fast_io::io::println(stvw,
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
						   ::fast_io::string_view(buffer, stvw.copy(buffer, 6, 2))
#ifdef __cpp_lib_ranges_find_last
							   ,
						   "\n",
						   stvw.rfind_character('o')
#endif
	);
}
