#include<winrt/base.h>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	winrt::hstring hstr;
	scan(fast_io::mnp::strlike_get(hstr));
	println(hstr);
}
