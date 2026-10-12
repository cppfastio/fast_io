// std::chrono names — localized weekday/month under imbue
#include <fast_io.h>
#include <fast_io_i18n.h>
#include <chrono>

using namespace fast_io::io;

int main() try
{
	auto const *de{::fast_io::l10n::load_l10n(u8"de_DE.UTF-8")};
	auto const *ja{::fast_io::l10n::load_l10n(u8"ja_JP.UTF-8")};
	::std::chrono::sys_days const sd{::std::chrono::year{2025} /
									 ::std::chrono::March / 15};
	::fast_io::println(::fast_io::imbue(de, ::fast_io::c_stdout()),
		::std::chrono::weekday{sd}, ::fast_io::mnp::chvw(u8' '),
		::std::chrono::March);
	::fast_io::println(::fast_io::imbue(ja, ::fast_io::c_stdout()),
		::std::chrono::weekday{sd}, ::fast_io::mnp::chvw(u8' '),
		::std::chrono::March);
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
