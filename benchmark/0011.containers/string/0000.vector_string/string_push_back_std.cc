#include<string>
#include<fast_io_dsal/vector.h>
#include<fast_io_driver/timer.h>

int main()
{
	::fast_io::vector<::std::u8string> vec;
	::std::size_t total_sum{};
	{
		::fast_io::timer t(u8"string_push_back_std");
		for(::std::size_t i{};i!=1000000u;++i)
		{
			total_sum+=vec.emplace_back(u8"hello world\n").size();
		}
	}
	::fast_io::io::println(total_sum);
}