#include<random>
#include<fast_io_dsal/vector.h>

inline constexpr auto generate_vals() noexcept
{
	::fast_io::freestanding::array<::std::uint_least64_t,
		::std::numeric_limits<::std::uint_least64_t>::digits10+2> tb;
	::std::uint_least64_t v{1};
	tb[0]=0;
	for(::std::size_t i{1},n{tb.size()-1};i!=n;++i)
	{
		v*=UINT32_C(10);
		tb[i]=v;
	}
	tb.back()=0;
	return tb;
}

inline constexpr std::size_t N{UINT32_C(10000000)};

inline constexpr ::fast_io::vector<::std::uint_least64_t> generate_random_data()
{
	constexpr auto tb{generate_vals()};
	::fast_io::vector<::std::uint_least64_t> vec(N);
	::std::mt19937_64 eng;//we need deterministic output
	::std::uniform_int_distribution<::std::size_t> dis(1,tb.size()-1);
	
	for(::std::size_t i{};i!=N;++i)
	{
		::std::size_t pos{dis(eng)};
		::std::uniform_int_distribution<::std::uint_least64_t> dis(tb[pos-1],tb[pos]-UINT64_C(1));
		vec[i]=dis(eng);
	}
	return vec;
}
