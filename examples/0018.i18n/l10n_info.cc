#include<fast_io.h>
#include<fast_io_i18n.h>

using namespace fast_io::io;

int main()
try
{
	fast_io::native_l10n loc(u8"");
	println(imbue(loc,fast_io::c_stdout()),loc);
}
catch(fast_io::error e)
{
	perrln(e);
	return 1;
}