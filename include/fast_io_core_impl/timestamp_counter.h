#pragma once

namespace fast_io
{

inline auto current_processor_timestamp_counter() noexcept
{
#if defined(__has_builtin)
#if __has_builtin(__builtin_ia32_rdtsc)
	return __builtin_ia32_rdtsc();
#else
	return static_cast<std::uint_least64_t>(0);
#endif
#elif defined(_MSC_VER) && defined(_M_IX86)
	return __rdtsc();
#else
	return static_cast<std::uint_least64_t>(0);
#endif
}

using current_processor_timestamp_counter_t = decltype(current_processor_timestamp_counter());

}