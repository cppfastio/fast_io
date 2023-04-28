#pragma once
#include<string_view>
namespace fast_io
{

inline std::uint_least64_t current_tsc() noexcept
{
#if defined(_MSC_VER)&&!defined(__clang__)
	return __rdtsc();
#elif __has_builtin(__builtin_ia32_rdtsc)
	return __builtin_ia32_rdtsc();
#else
	return 0;
#endif
}

struct tsc_timer
{
	std::u8string_view s;
	std::uint_least64_t t0;
	#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
	#endif
	explicit tsc_timer(std::u8string_view strvw):s(strvw),t0(current_tsc()){}
	tsc_timer(tsc_timer const &)=delete;
	tsc_timer& operator=(tsc_timer const &)=delete;
	#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
	#endif
	~tsc_timer()
	{
		::fast_io::io::perr(fast_io::u8err(),s, u8":", current_tsc()-t0,u8" cycles\n");
	}
};

}
