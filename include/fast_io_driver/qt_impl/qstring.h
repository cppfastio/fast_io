#pragma once

namespace fast_io
{

class qt_error
{
public:
	QString qstr;
	template<typename... Args>
	requires std::constructible_from<QString,Args...>
	qt_error(Args&& ...args):qstr(::std::forward<Args>(args)...){}

	QChar* data() noexcept
	{
		return qstr.data();
	}
	QChar const* data() const noexcept
	{
		return qstr.data();
	}
	std::size_t size() const noexcept
	{
		return static_cast<std::size_t>(qstr.size());
	}
};

namespace details
{

template<typename T>
concept qt_qstring_view_like_impl = std::same_as<std::remove_cvref_t<T>,QString>||std::same_as<std::remove_cvref_t<T>,qt_error>;

template<typename T>
concept qt_convertible_to_qstring_impl = requires(T const& e)
{
	{e.toString()}->std::same_as<QString>;
};

}

template<typename... Args>
requires std::constructible_from<QString,Args...>
inline void throw_qt_error([[maybe_unused]] Args&& ...args)
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw qt_error(::std::forward<Args>(args)...);
#endif
#else
	fast_terminate();
#endif
}

template<std::integral char_type,typename T>
requires (details::qt_qstring_view_like_impl<T>)
constexpr std::conditional_t<sizeof(char_type)==sizeof(char16_t),basic_io_scatter_t<char_type>,manipulators::code_cvt_t<encoding_scheme::execution_charset,encoding_scheme::execution_charset,char16_t>> status_io_print_forward(io_alias_type_t<char_type>,T const& hstr) noexcept
{
	if constexpr(sizeof(char_type)==sizeof(char16_t))
	{
		using general_utf16_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type const*;
		return {reinterpret_cast<general_utf16_may_alias_ptr>(hstr.data()),static_cast<std::size_t>(hstr.size())};
	}
	else
	{
		using char16_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char16_t const*;
		return {{reinterpret_cast<char16_may_alias_ptr>(hstr.data()),static_cast<std::size_t>(hstr.size())}};
	}
}

template<typename T>
requires (details::qt_convertible_to_qstring_impl<T>)
inline QString print_alias_define(io_alias_t,T const& hstr) noexcept
{
	return {hstr.toString()};
}

inline QString strlike_construct_define(io_strlike_type_t<char16_t,QString>,char16_t const* first,char16_t const* last)
{
	std::size_t diff{static_cast<std::size_t>(last-first)};
	if constexpr(sizeof(unsigned)<sizeof(std::size_t))
	{
		constexpr unsigned max_sz{INT_MAX/sizeof(char16_t)};
		if(max_sz<diff)
			::fast_io::fast_terminate();
	}
	using qchar_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= QChar const*;
	return QString(reinterpret_cast<qchar_may_alias_ptr>(first),static_cast<int>(static_cast<unsigned>(diff)));
}

inline QString strlike_single_character_construct_define(io_strlike_type_t<char16_t,QString>,char16_t ch)
{
	return QString(ch);
}

template<typename... Args>
inline QString u16concat_qt_qstring(Args&& ...args)
{
	return ::fast_io::basic_general_concat<false,char16_t,QString>(::std::forward<Args>(args)...);
}

template<typename... Args>
inline QString u16concatln_qt_qstring(Args&& ...args)
{
	return ::fast_io::basic_general_concat<true,char16_t,QString>(::std::forward<Args>(args)...);
}

inline constexpr io_strlike_reference_wrapper<char16_t,QString> io_strlike_ref(::fast_io::io_alias_t,QString& hstr) noexcept
{
	return {__builtin_addressof(hstr)};
}

namespace manipulators
{

template<typename T>
inline auto qt_as_filename(T const& hstr) noexcept
{
	using char16_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= char16_t const*;
	if constexpr(::std::same_as<::std::remove_cvref_t<T>,QString>)
	{
		return ::fast_io::manipulators::basic_os_c_str_with_known_size<char16_t>{reinterpret_cast<char16_may_alias_const_ptr>(hstr.data()),static_cast<std::size_t>(hstr.size())};
	}
	else
	{
		return ::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char16_t>{reinterpret_cast<char16_may_alias_const_ptr>(hstr.data()),static_cast<std::size_t>(hstr.size())};
	}
}

}

}
