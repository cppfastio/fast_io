#include<fast_io.h>
#include<fast_io_i18n.h>

int main()
{
	fast_io::l10n loc("");
	println(imbue(loc,fast_io::c_stdout()),"rigstr\t",fast_io::mnp::internal("asf",10),"\n"
		"lft\t",fast_io::mnp::left(-10000,10),"\n"
		"mid\t",fast_io::mnp::middle(-10000,10),"\n"
		"rig\t",fast_io::mnp::right(-10000,10),"\n"
		"int\t",fast_io::mnp::internal(-10000,10),"\n"
		"lefch\t",fast_io::mnp::left(-10000,10,'+'),"\n"
		"midch\t",fast_io::mnp::middle(-10000,10,'+'),"\n"
		"rigch\t",fast_io::mnp::right(-10000,10,'+'),"\n"
		"intch\t",fast_io::mnp::internal(-10000,10,'+'));
}
