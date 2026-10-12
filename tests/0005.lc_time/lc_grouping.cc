// locale-aware integer grouping — de_DE '.' and hi_IN '3,2'
#include <fast_io.h>
#include <fast_io_i18n.h>

using namespace fast_io::io;

int main() try
{
	auto const *de{::fast_io::l10n::load_l10n(u8"de_DE.UTF-8")};
	auto const *hi{::fast_io::l10n::load_l10n(u8"hi_IN.UTF-8")};
	auto const *en{::fast_io::l10n::load_l10n(u8"en_US.UTF-8")};
	for (::std::uint_least64_t v : {1ull, 12ull, 123ull, 1234ull, 12345ull,
									1234567890ull, 1242141242124ull})
	{
		::fast_io::println(::fast_io::imbue(de, ::fast_io::c_stdout()), v);
	}
	::fast_io::println(::fast_io::imbue(hi, ::fast_io::c_stdout()), 12345678u);
	::fast_io::println(::fast_io::imbue(en, ::fast_io::c_stdout()), 1234567890u);
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
