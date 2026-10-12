// caller-supplied blob pointer — a sandbox loads the file itself and
// hands the lc_locale* straight to imbue; load_l10n is not involved
#include <fast_io.h>
#include <fast_io_i18n.h>
#include <fast_io_device.h>
#include <cstdlib>
#include <cstring>

using namespace fast_io::io;

int main()
try
{
	char const *dir{::std::getenv("FAST_IO_L10N_PATH")};
	if (dir == nullptr)
	{
		::fast_io::perr("FAST_IO_L10N_PATH unset — skipped\n");
		return 0;
	}
	::fast_io::native_file_loader loader{
		::fast_io::u8concat_fast_io(
			::fast_io::u8string_view{
				reinterpret_cast<char8_t const *>(dir), ::std::strlen(dir)},
			u8"/de_DE.UTF-8.bin")};
	auto const *loc{reinterpret_cast<::fast_io::l10n::lc_locale const *>(
		loader.data())};
	::fast_io::println(::fast_io::imbue(loc, ::fast_io::c_stdout()), 1234567890);
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
