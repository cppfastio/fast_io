#pragma once

namespace fast_io
{

struct allocation_mmap_options
{
	bool write_back{};
	inline explicit constexpr allocation_mmap_options() noexcept = default;
	inline constexpr allocation_mmap_options(::fast_io::mmap_prot, ::fast_io::mmap_flags flagsv) noexcept
	{
		::std::uint_least32_t exclusiveflags{static_cast<::std::uint_least32_t>(flagsv & ::fast_io::mmap_flags::map_type)};
		if (exclusiveflags == 3u)
		{
			exclusiveflags = 1u;
		}
		if (exclusiveflags == 1u)
		{
			this->write_back = true;
		}
	}
};

} // namespace fast_io
