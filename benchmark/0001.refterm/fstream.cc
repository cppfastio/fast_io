#include<fast_io.h>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>
#include<fast_io_device.h>
inline void test()
{
	fast_io::nt_file wf(fast_io::io_dup,fast_io::nt_stdout());
	fast_io::filebuf_file fbf(std::move(wf),fast_io::open_mode::out);
	std::ofstream fout;
	*fout.rdbuf()=std::move(*fbf.fb);
	for(std::size_t i{};i!=1000000;++i)
		fout<<i<<'\n';
}

int main()
{
	fast_io::u8obuf_file timer_obf(u"fstream.txt");
	auto t0{posix_clock_gettime(fast_io::posix_clock_id::monotonic)};
	test();
	print(timer_obf,posix_clock_gettime(fast_io::posix_clock_id::monotonic)-t0,u8"s\n");
}
