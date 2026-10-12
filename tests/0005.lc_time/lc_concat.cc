// locale-aware concat — lc_concat/lc_concatln through lc_ctx
#include <fast_io.h>
#include <fast_io_i18n.h>

using namespace fast_io::io;

int main() try
{
	auto const *loc{::fast_io::l10n::load_l10n(u8"de_DE.UTF-8")};
	auto const ts{::fast_io::posix_statx_timestamp64{1742044245, 0}};
	auto s{::fast_io::lc_concat(
		loc, ::fast_io::mnp::chvw(u8'x'), ::fast_io::mnp::chvw(u8' '),
		1234567890, ::fast_io::mnp::chvw(u8' '),
		::fast_io::mnp::d_t_fmt(ts), ::fast_io::mnp::chvw(u8' '),
		::fast_io::mnp::boolalpha(true))};
	::fast_io::println(s);
	auto ln{::fast_io::lc_concatln(loc, 42, ::fast_io::mnp::chvw(u8' '), 120)};
	::fast_io::println(ln);
	// all-plain args defer to basic_general_concat — same result,
	// no locale machinery involved
	auto plain{::fast_io::lc_concat(loc, 42, ::fast_io::mnp::chvw(u8' '), 120)};
	::fast_io::println(plain);
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
