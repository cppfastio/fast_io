#include<fast_io.h>
#include<fast_io_i18n.h>

int main()
{
	fast_io::native_l10n loc(u8"");
	println(imbue(loc,fast_io::c_stdout()),loc);
}