
#pragma once

namespace fast_io::crypto
{

template<std::size_t block_size>
struct pkcs7_padding
{
    auto operator()(std::span<std::byte const> inp, std::size_t remaining_length)
    {
        std::byte pad(static_cast<std::byte>(static_cast<std::uint_least8_t>(remaining_length & 0xFF)));
        ::fast_io::freestanding::array<std::byte, block_size> text{};
		details::my_copy(inp.begin(), inp.end(), text.data());
        for (std::size_t i(block_size - 1); i >= block_size - remaining_length; --i)
            text[i] = pad;
        return text;
    }
};
};
