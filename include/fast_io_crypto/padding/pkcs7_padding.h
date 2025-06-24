
#pragma once

namespace fast_io::crypto
{

template <::std::size_t block_size>
	requires(block_size > static_cast<::std::size_t>(0u))
struct pkcs7_padding
{
	inline constexpr auto operator()(::std::span<::std::byte const> inp, ::std::size_t remaining_length)
	{
		if (remaining_length >= block_size) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		::std::byte pad(static_cast<::std::byte>(static_cast<::std::uint_least8_t>(remaining_length & 0xFF)));
		::fast_io::freestanding::array<::std::byte, block_size> text{};
		details::my_copy(inp.begin(), inp.end(), text.data());
		for (::std::size_t i(block_size - static_cast<::std::size_t>(1u)); i >= block_size - remaining_length; --i)
		{
			text[i] = pad;
		}
		return text;
	}
};
}; // namespace fast_io::crypto
