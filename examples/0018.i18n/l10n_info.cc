#include <fast_io.h>
#include <fast_io_i18n.h>

using namespace fast_io::io;

int main()
try
{
	auto const *loc{::fast_io::l10n::load_l10n(u8"")};
	println(imbue(loc, fast_io::c_stdout()), ::fast_io::mnp::lc_dump(loc));
}
catch throws(::std::error e)
{
	perrln(e);
	return 1;
}