#include<winrt/base.h>
#include<fast_io.h>

int main()
{
	winrt::hstring hstr;
	scan(fast_io::mnp::strlike_get(hstr));
	println(hstr);
}
