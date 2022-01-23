#pragma once

namespace fast_io::freestanding
{

template<typename T,std::size_t N>
requires (sizeof(N)!=0)
struct array
{
	using value_type = T;
	using pointer=T*;
	using const_pointer=T const*;
	using reference=T&;
	using const_reference=T const&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	T element[N];
	inline constexpr T* data() noexcept
	{
		return element;
	}
	inline constexpr T const* data() const noexcept
	{
		return element;
	}
	inline constexpr T* begin() noexcept
	{
		return element;
	}
	inline constexpr T* end() noexcept
	{
		return element+N;
	}
	inline static constexpr bool empty() noexcept
	{
		return false;
	}
	inline static constexpr std::size_t size() noexcept
	{
		return N;
	}
	inline constexpr T const* begin() const noexcept
	{
		return element;
	}
	inline constexpr T const* end() const noexcept
	{
		return element+N;
	}
	inline constexpr T const* cbegin() const noexcept
	{
		return element;
	}
	inline constexpr T const* cend() const noexcept
	{
		return element+N;
	}
	inline constexpr T const& front() const noexcept
	{
		return *element;
	}
	inline constexpr T& front() noexcept
	{
		return *element;
	}
	inline constexpr T const& back() const noexcept
	{
		return element[N-1];
	}
	inline constexpr T& back() noexcept
	{
		return element[N-1];
	}
	inline constexpr T const& operator[](std::size_t i) const noexcept
	{
		return element[i];
	}
	inline constexpr T& operator[](std::size_t i) noexcept
	{
		return element[i];
	}
};

}
