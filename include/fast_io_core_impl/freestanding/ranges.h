#pragma once

namespace fast_io::freestanding
{

#ifdef __cpp_lib_ranges_to_container
using ::std::from_range_t;
using ::std::from_range;
#else
struct from_range_t
{
	inline explicit constexpr from_range_t() noexcept = default;
};
inline constexpr ::fast_io::freestanding::from_range_t from_range{};
#endif

struct from_hasher_t
{
	inline explicit constexpr from_hasher_t() noexcept = default;
};
inline constexpr ::fast_io::freestanding::from_hasher_t from_hasher{};

} // namespace fast_io::freestanding

namespace fast_io
{
using ::fast_io::freestanding::from_hasher_t;
using ::fast_io::freestanding::from_hasher;
} // namespace fast_io
