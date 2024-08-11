#include <fast_io_dsal/string_view.h>
#include <fast_io.h>

int main()
{
	::fast_io::string_view stvw("hello \0world\n");
	::fast_io::io::println(stvw,
						   stvw == "hello world\n", "\n",
						   "hello world\n" == stvw, "\n",
						   stvw <=> "hello world\n", "\n",
						   "hello world\n" <=> stvw, "\n",
						   stvw.starts_with("hello"), "\n",
						   stvw.ends_with("\0world\n"));
}