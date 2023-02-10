#pragma once

namespace fast_io
{

namespace details
{

struct cannot_output_type
{
inline explicit constexpr cannot_output_type() noexcept = default;
};

}

template<typename T>
inline constexpr decltype(auto) io_print_alias(T&& t) noexcept
{
	using no_cvref_t=std::remove_cvref_t<T>;
	if constexpr(::std::is_function_v<no_cvref_t>)
	{
		return ::fast_io::details::cannot_output_type{};
	}
	else if constexpr(alias_printable<no_cvref_t>)
		return print_alias_define(io_alias,::std::forward<T>(t));
	else
		return ::std::forward<T>(t);
}

template<std::integral char_type,typename T>
inline constexpr auto io_print_forward(T&& t) noexcept
{
	using no_cvref_t=std::remove_cvref_t<T>;
	if constexpr(::std::is_function_v<no_cvref_t>)
	{
		return ::fast_io::details::cannot_output_type{};
	}
	else if constexpr(status_io_print_forwardable<char_type,T>)
		return status_io_print_forward(io_alias_type<char_type>,::std::forward<T>(t));
	else if constexpr(std::is_trivially_copyable_v<no_cvref_t>&&sizeof(no_cvref_t)<=sizeof(std::size_t)*2)
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
	if constexpr(::std::is_function_v<no_ref_t>)
	{
		return ::fast_io::details::cannot_output_type{};
	}
	else if constexpr(alias_scannable<no_ref_t>)
		return scan_alias_define(io_alias,::std::forward<T>(t));
	else if constexpr(manipulator<no_ref_t>)
		return t;
	else
		return parameter<no_ref_t&>{t};
}

}
