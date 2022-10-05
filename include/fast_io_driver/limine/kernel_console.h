#pragma once

namespace fast_io::limine
{

template<std::integral ch_type>
requires (sizeof(ch_type)==1)
struct basic_kernel_console
{
	using char_type = ch_type;
	using function_ptr_type = void(*)(void const*, size_t) noexcept;
	function_ptr_type func_ptr{};
};

template<std::integral ch_type>
inline constexpr basic_kernel_console<ch_type> io_value_handle(basic_kernel_console<ch_type> con) noexcept
{
	return con;
}

using kernel_console = basic_kernel_console<char>;
using u8kernel_console = basic_kernel_console<char8_t>;

template<std::integral char_type,::std::contiguous_iterator Iter>
inline void write(basic_kernel_console<char_type> console,Iter first,Iter last) noexcept
{
	(console.func_ptr)(::std::to_address(first),static_cast<std::size_t>(last-first)*sizeof(*first));
}

namespace details
{

inline void kconsole_scatter_write_impl(::fast_io::limine::kernel_console::function_ptr_type func_ptr,io_scatter_t const* pscatters,std::size_t n) noexcept
{
	for(std::size_t i{};i!=n;++i)
	{
		auto scatter{pscatters[i]};
		(func_ptr)(scatter.base,scatter.len);
	}
}

}

template<std::integral char_type>
inline void scatter_write(basic_kernel_console<char_type> console,io_scatters_t scatters) noexcept
{
	::fast_io::limine::details::kconsole_scatter_write_impl(console.func_ptr,scatters.base,scatters.len);
}

}
