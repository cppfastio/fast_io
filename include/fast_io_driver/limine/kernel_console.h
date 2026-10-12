#pragma once

namespace fast_io::limine
{

template <::std::integral ch_type>
	requires(sizeof(ch_type) == 1)
struct basic_kernel_console
{
	using char_type = ch_type;
	using output_char_type = char_type;
	using function_ptr_type = void (*)(void const *, size_t) noexcept;
	function_ptr_type func_ptr{};
};

template <::std::integral ch_type>
inline constexpr basic_kernel_console<ch_type> io_value_handle(basic_kernel_console<ch_type> con) noexcept
{
	return con;
}

using kernel_console = basic_kernel_console<char>;
using u8kernel_console = basic_kernel_console<char8_t>;

template <::std::integral char_type>
inline void write_all_overflow_define(basic_kernel_console<char_type> console, char_type const *first,
									  ::std::size_t count) noexcept
{
	(console.func_ptr)(first, count * sizeof(char_type));
}

namespace details
{

inline void kconsole_scatter_write_impl(::fast_io::limine::kernel_console::function_ptr_type func_ptr,
										io_scatter_t const *pscatters, ::std::size_t n) noexcept
{
	for (::std::size_t i{}; i != n; ++i)
	{
		auto scatter{pscatters[i]};
		(func_ptr)(scatter.base, scatter.len);
	}
}

} // namespace details

template <::std::integral char_type>
inline void scatter_write_all_overflow_define(basic_kernel_console<char_type> console,
											  ::fast_io::basic_io_scatter_t<char_type> const *pscatters,
											  ::std::size_t n) noexcept
{
	::fast_io::limine::details::kconsole_scatter_write_impl(
		console.func_ptr, reinterpret_cast<::fast_io::io_scatter_t const *>(pscatters), n);
}

} // namespace fast_io::limine
