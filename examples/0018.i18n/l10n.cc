#include<fast_io.h>
#include<fast_io_i18n.h>

int main()
{
	fast_io::l10n loc("");
	println(imbue(loc,fast_io::c_stdout()),"Hello World ",-1248721947124LL," ",fast_io::mnp::boolalpha(true));
}