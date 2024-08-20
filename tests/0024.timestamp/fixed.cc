#define FAST_IO_SANITIZE_IO_BUFFER
#include<fast_io.h>

inline void test_val(::fast_io::unix_timestamp ts)
{
	using namespace ::fast_io::mnp;
	using namespace ::fast_io::io;
	println("val:",ts,"\n",
		fixed(ts,0),"\n",
		fixed(ts,1),"\n",
		fixed(ts,2),"\n",
		fixed(ts,3),"\n",
		fixed(ts,4),"\n",
		fixed(ts,::std::numeric_limits<::std::uint_least64_t>::digits10-3),"\n",
		fixed(ts,::std::numeric_limits<::std::uint_least64_t>::digits10-2),"\n",
		fixed(ts,::std::numeric_limits<::std::uint_least64_t>::digits10-1),"\n",
		fixed(ts,::std::numeric_limits<::std::uint_least64_t>::digits10),"\n",
		fixed(ts,::std::numeric_limits<::std::uint_least64_t>::digits10+1),"\n",
		fixed(ts,::std::numeric_limits<::std::uint_least64_t>::digits10+2),"\n"
		fixed(ts,(::std::numeric_limits<::std::uint_least64_t>::digits10)*2));
}

int main()
{
	using namespace ::fast_io::mnp;
	using namespace ::fast_io::io;
	test_val({0,0});
	test_val({0,::fast_io::uint_least64_subseconds_per_second-1u});
	test_val({0,::fast_io::uint_least64_subseconds_per_second>>1u});
	test_val({0,(::fast_io::uint_least64_subseconds_per_second/10u)>>1u});
	test_val({::std::numeric_limits<::std::int_least64_t>::max(),::fast_io::uint_least64_subseconds_per_second-1u});
	test_val({::std::numeric_limits<::std::int_least64_t>::max(),::fast_io::uint_least64_subseconds_per_second>>1u});
	test_val({::std::numeric_limits<::std::int_least64_t>::max(),(::fast_io::uint_least64_subseconds_per_second/10u)>>1u});
	test_val({::std::numeric_limits<::std::int_least64_t>::min(),::fast_io::uint_least64_subseconds_per_second-1u});
	test_val({::std::numeric_limits<::std::int_least64_t>::min(),::fast_io::uint_least64_subseconds_per_second>>1u});
	test_val({::std::numeric_limits<::std::int_least64_t>::min(),(::fast_io::uint_least64_subseconds_per_second/10u)>>1u});
	test_val(::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime));
	test_val({::std::numeric_limits<::std::int_least64_t>::min(),::fast_io::uint_least64_subseconds_per_second>>1u});
}
