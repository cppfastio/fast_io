// posix_clock_gettime under imbue — de_DE and ja_JP
#include <fast_io.h>
#include <fast_io_i18n.h>

using namespace fast_io::io;

int main() try
{
	auto const *de{::fast_io::l10n::load_l10n(u8"de_DE.UTF-8")};
	auto const *ja{::fast_io::l10n::load_l10n(u8"ja_JP.UTF-8")};
	auto const v{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::println(::fast_io::imbue(de, ::fast_io::c_stdout()),
		::fast_io::mnp::d_t_fmt(v));
	::fast_io::println(::fast_io::imbue(ja, ::fast_io::c_stdout()),
		::fast_io::mnp::era_d_t_fmt(v));
	// unimbued — POSIX default
	::fast_io::println(::fast_io::mnp::d_t_fmt(v));
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
