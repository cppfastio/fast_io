// Japanese era programs — era_d_t_fmt/era_d_fmt/era_t_fmt
#include <fast_io.h>
#include <fast_io_i18n.h>

using namespace fast_io::io;

int main() try
{
	auto const *loc{::fast_io::l10n::load_l10n(u8"ja_JP.UTF-8")};
	auto const ts{::fast_io::posix_statx_timestamp64{1742044245, 0}};
	::fast_io::println(::fast_io::imbue(loc, ::fast_io::c_stdout()),
		::fast_io::mnp::d_t_fmt(ts));
	::fast_io::println(::fast_io::imbue(loc, ::fast_io::c_stdout()),
		::fast_io::mnp::era_d_t_fmt(ts));
	::fast_io::println(::fast_io::imbue(loc, ::fast_io::c_stdout()),
		::fast_io::mnp::era_d_fmt(ts));
	::fast_io::println(::fast_io::imbue(loc, ::fast_io::c_stdout()),
		::fast_io::mnp::era_t_fmt(ts));
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
