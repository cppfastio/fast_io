#pragma once

namespace fast_io::freestanding
{

#ifdef __cpp_lib_ranges_to_container
using ::std::from_range_t;
using ::std::from_range;
#else
struct from_range_t
{
	explicit constexpr from_range_t() noexcept = default;
};
inline constexpr ::fast_io::freestanding::from_range_t from_range{};
#endif

} // namespace fast_io::freestanding
