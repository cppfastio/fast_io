#include <fast_io.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/string.h>

int main()
{
	fast_io::string str("helloworld");
	fast_io::string_view vw(str.substrvw(3, 4));
	using namespace ::fast_io::io;
	println("before replace:", str, "\n", vw);
	str.replace_index(1, 5, vw);
	println("after replace:", str, "\n", vw); // asan cannot detect this, but my secrect tool can lol
}
