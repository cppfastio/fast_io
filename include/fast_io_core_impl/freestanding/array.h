#pragma once

namespace fast_io::freestanding
{

template <typename T, ::std::size_t N>
	requires(N != 0)
struct array
{
	using value_type = T;
	using pointer = T *;
	using const_pointer = T const *;
	using reference = T &;
	using const_reference = T const &;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	T element[N];
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T *data() noexcept
	{
		return element;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T const *data() const noexcept
	{
		return element;
	}
	inline constexpr T *begin() noexcept
	{
		return element;
	}
	inline constexpr T *end() noexcept
	{
		return element + N;
	}
	inline static constexpr bool empty() noexcept
	{
		return false;
	}
	inline static constexpr ::std::size_t size() noexcept
	{
		return N;
	}
	inline constexpr T const *begin() const noexcept
	{
		return element;
	}
	inline constexpr T const *end() const noexcept
	{
		return element + N;
	}
	inline constexpr T const *cbegin() const noexcept
	{
		return element;
	}
	inline constexpr T const *cend() const noexcept
	{
		return element + N;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T const &front() const noexcept
	{
		return *element;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T &front() noexcept
	{
		return *element;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T const &back() const noexcept
	{
		constexpr size_type nm1{N - 1};
		return element[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T &back() noexcept
	{
		constexpr size_type nm1{N - 1};
		return element[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T const &front_unchecked() const noexcept
	{
		return *element;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T &front_unchecked() noexcept
	{
		return *element;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T const &back_unchecked() const noexcept
	{
		constexpr size_type nm1{N - 1};
		return element[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T &back_unchecked() noexcept
	{
		constexpr size_type nm1{N - 1};
		return element[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T const &operator[](::std::size_t i) const noexcept
	{
		return element[i];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T &operator[](::std::size_t i) noexcept
	{
		return element[i];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T const &index_unchecked(::std::size_t i) const noexcept
	{
		return element[i];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr T &index_unchecked(::std::size_t i) noexcept
	{
		return element[i];
	}
};

} // namespace fast_io::freestanding
