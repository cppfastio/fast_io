#pragma once

namespace fast_io::intrinsics
{

template <typename T>
struct simd_vector<T, 0>
{
	using value_type = T;
	inline constexpr T const *data() const noexcept
	{
		return nullptr;
	}
	inline constexpr T *data() noexcept
	{
		return nullptr;
	}
	inline static constexpr ::std::size_t size() noexcept
	{
		return 0;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#endif
	inline void load(void const *) noexcept
	{
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#endif
	inline void store(void *) noexcept
	{
	}

	inline static constexpr bool empty() noexcept
	{
		return true;
	}
	inline static constexpr ::std::size_t max_size() noexcept
	{
		constexpr ::std::size_t v{static_cast<::std::size_t>(-1) / sizeof(value_type)};
		return v;
	}
};

} // namespace fast_io::intrinsics
