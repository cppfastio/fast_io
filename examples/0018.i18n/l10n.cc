#include<fast_io.h>
#include<fast_io_i18n.h>

using namespace fast_io::io;

int main()
{
	fast_io::native_l10n loc(u8"");
	println(imbue(loc,fast_io::c_stdout()),"Hello World ",-1248721947124LL," ",fast_io::mnp::boolalpha(true));
}