
#pragma once

namespace fast_io::crypto
{

template <::std::size_t block_size>
struct zero_padding
{
	inline constexpr auto operator()(::std::span<::std::byte const> inp, ::std::size_t remaining_length)
	{
		::fast_io::freestanding::array<::std::byte, block_size> text{};
		details::my_copy(inp.begin(), inp.end(), text.data());
		return text;
	}
};
}; // namespace fast_io::crypto
