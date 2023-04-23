#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	using namespace fast_io::char_category;
	using namespace fast_io::mnp;
	println(fast_io::u8c_stdout(),
		u8"！\t",boolalpha(is_c_fullwidth(U'！')),u8"\n"
		u8"!\t",boolalpha(is_c_fullwidth(U'!')),u8"\n"
		u8"＀\t",boolalpha(is_c_fullwidth(U'＀')),u8"\n"
		u8"｠\t",boolalpha(is_c_fullwidth(U'｠')),u8"\n"
		u8"！\t",to_c_halfwidth(U'！'));
}