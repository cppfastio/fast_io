#include<winrt/base.h>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	println(fast_io::wconcat_winrt_hstring(L"qqwdwq",1234,L"asfasf"));
}
