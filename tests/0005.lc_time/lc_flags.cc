// l10n_load_flags::ignore_system_settings — Windows skips the registry
// (POSIX is unaffected — env is the normal mechanism, not an override)
#include <fast_io.h>
#include <fast_io_i18n.h>

using namespace fast_io::io;

int main()
try
{
	auto const *a{::fast_io::l10n::load_l10n(u8"de_DE.UTF-8")};
	auto const *b{::fast_io::l10n::load_l10n(
		u8"de_DE", ::fast_io::l10n::l10n_load_flags::ignore_system_settings)};
	::fast_io::print(a == b ? "same\n" : "diff\n");
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
