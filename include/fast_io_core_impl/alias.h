#pragma once

namespace fast_io
{

template<typename T>
inline constexpr decltype(auto) io_print_alias(T&& t) noexcept
{
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
		return print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t));
	else
		return ::fast_io::freestanding::forward<T>(t);
}

template<std::integral char_type,typename T>
inline constexpr auto io_print_forward(T&& t) noexcept
{
	using no_cvref_t=std::remove_cvref_t<T>;
	if constexpr(status_io_print_forwardable<char_type,T>)
		return status_io_print_forward(io_alias_type<char_type>,::fast_io::freestanding::forward<T>(t));
	else if constexpr(std::is_trivially_copyable_v<no_cvref_t>&&sizeof(no_cvref_t)<=
#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
	8
#else
	sizeof(std::size_t)*2
#endif
	)
		return static_cast<no_cvref_t>(t);
	else
		return parameter<std::remove_reference_t<T> const&>{t};
}

template<std::integral char_type,typename T>
inline constexpr auto io_scan_forward(T t) noexcept
{
	if constexpr(status_io_scan_forwardable<char_type,T>)
		return status_io_scan_forward(io_alias_type<char_type>,t);
	else
		return t;
}

template<typename T>
inline constexpr auto io_scan_alias(T&& t) noexcept
{
	using no_ref_t = std::remove_reference_t<T>;
	if constexpr(alias_scanable<no_ref_t>)
		return scan_alias_define(io_alias,::fast_io::freestanding::forward<T>(t));
	else if constexpr(manipulator<no_ref_t>)
		return t;
	else
		return parameter<no_ref_t&>{t};
}

}