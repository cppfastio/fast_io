#include<fast_io_dsal/string.h>
#include<fast_io_driver/timer.h>

int main()
{
	::fast_io::u8string str;
	::std::size_t total_sum{};
	{
		::fast_io::timer t(u8"fast_io");
		for(::std::size_t i{};i!=10000000u;++i)
		{
			str.push_back(u8'z');
			total_sum+=str.size();
		}
	}
	::fast_io::io::println(total_sum);
}
