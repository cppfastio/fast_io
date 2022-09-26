#pragma once

namespace fast_io
{

template<std::ranges::contiguous_range rg,typename containe=::fast_io::containers::vector<std::size_t,::fast_io::native_thread_local_allocator>>
requires std::integral<std::ranges::range_value_t<rg>>
inline constexpr auto auto_indent(rg&& r)
{
	if constexpr(std::is_array_v<std::remove_cvref_t<rg>>)
		return ::fast_io::details::calculate_auto_width_result<containe>(
			std::ranges::data(r),std::ranges::data(r)+(std::ranges::size(r)-1)
		);
	else
		return ::fast_io::details::calculate_auto_width_result<containe>(
			std::ranges::data(r),std::ranges::data(r)+std::ranges::size(r));
}

}
