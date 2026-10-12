#pragma once

namespace fast_io
{

namespace manipulators
{

struct herbceptions_query_information
{
	using manip_tag = ::fast_io::manip_tag_t;
	::std::error_domain_singleton const *domain{};
	::std::size_t code{};
	::std::error_query_information info{};
};

inline constexpr ::fast_io::manipulators::herbceptions_query_information name(::std::error const &e) noexcept
{
	return {e.domain(), e.code(), ::std::error_query_information::name};
}

inline constexpr ::fast_io::manipulators::herbceptions_query_information message(::std::error const &e) noexcept
{
	return {e.domain(), e.code(), ::std::error_query_information::message};
}

inline constexpr ::fast_io::manipulators::herbceptions_query_information name_message(::std::error const &e) noexcept
{
	return {e.domain(), e.code(), ::std::error_query_information::name_message};
}

inline constexpr ::fast_io::manipulators::herbceptions_query_information posix_equivalent_name(::std::error const &e) noexcept
{
	return {::std::error_domain<::std::errc>::domain(), ::std::error_domain<::std::errc>::code(e.to_errc()), ::std::error_query_information::name};
}

inline constexpr ::fast_io::manipulators::herbceptions_query_information posix_equivalent_message(::std::error const &e) noexcept
{
	return {::std::error_domain<::std::errc>::domain(), ::std::error_domain<::std::errc>::code(e.to_errc()), ::std::error_query_information::name_message};
}

inline constexpr ::fast_io::manipulators::herbceptions_query_information posix_equivalent_name_message(::std::error const &e) noexcept
{
	return {::std::error_domain<::std::errc>::domain(), ::std::error_domain<::std::errc>::code(e.to_errc()), ::std::error_query_information::name_message};
}

} // namespace manipulators

namespace details
{
template <typename output>
inline constexpr void herbception_scatter_write_callback(void *cookie, ::std::io_scatter_t const *base, ::std::size_t n) noexcept
{
	using fast_io_io_scatter_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::io_scatter_t const *;
	if constexpr (sizeof(output) <= sizeof(void *) && ::std::is_trivially_copyable_v<output>)
	{
		output otm;
		::fast_io::details::my_memcpy(__builtin_addressof(otm), __builtin_addressof(cookie), sizeof(output));
		FAST_IO_HERBCEPTIONS_TRY
		{
			::fast_io::operations::scatter_write_all_bytes(otm, reinterpret_cast<fast_io_io_scatter_const_may_alias_ptr>(base), n);
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{}
	}
	else
	{
		FAST_IO_HERBCEPTIONS_TRY
		{
			::fast_io::operations::scatter_write_all_bytes(*reinterpret_cast<output *>(cookie), reinterpret_cast<fast_io_io_scatter_const_may_alias_ptr>(base), n);
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{}
	}
}

template <::std::integral char_type>
inline constexpr ::std::error_reporter_encoding compute_herbception_encoding() noexcept
{
	if constexpr (::fast_io::execution_charset_encoding<char_type> == ::fast_io::encoding_scheme::utf_ebcdic)
	{
		static_assert(sizeof(char_type) == sizeof(char), "ebcdic must have sizeof 1");
	}
	else if constexpr (sizeof(char_type) == sizeof(char32_t))
	{
		return ::std::error_reporter_encoding::utf32;
	}
	else if constexpr (sizeof(char_type) == sizeof(char16_t))
	{
		return ::std::error_reporter_encoding::utf16;
	}
	else if constexpr (::fast_io::execution_charset_encoding<char_type> == ::fast_io::encoding_scheme::gb18030)
	{
		return ::std::error_reporter_encoding::gb18030;
	}
	else
	{
		return ::std::error_reporter_encoding::utf8;
	}
}

template <::std::integral char_type>
inline constexpr ::std::error_reporter_encoding herbception_encoding_impl = ::fast_io::details::compute_herbception_encoding<char_type>();

template <typename output>
inline constexpr void print_define_herbception_impl(output otm, ::std::error_domain_singleton const *domain, ::std::size_t code,
													::std::error_reporter_encoding encoding, ::std::error_query_information info)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(domain->do_query_information(
		code, info, encoding, ::std::declval<void *>(),
		::fast_io::details::herbception_scatter_write_callback<output>)))
{
	if (domain == nullptr) [[unlikely]]
	{
		return;
	}
	void *cookie{};
	if constexpr (sizeof(output) <= sizeof(void *) && ::std::is_trivially_copyable_v<output>)
	{
		::fast_io::details::my_memcpy(__builtin_addressof(cookie), __builtin_addressof(otm), sizeof(output));
	}
	else
	{
		cookie = __builtin_addressof(otm);
	}
	domain->do_query_information(code, info, encoding, cookie, ::fast_io::details::herbception_scatter_write_callback<output>);
}
} // namespace details

template <::std::integral char_type, typename output>
inline constexpr void print_define(::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::herbceptions_query_information>, output otm, ::fast_io::manipulators::herbceptions_query_information const &info)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::fast_io::details::print_define_herbception_impl(
		otm, info.domain, info.code, ::fast_io::details::herbception_encoding_impl<char_type>, info.info)))
{
	::fast_io::details::print_define_herbception_impl(otm, info.domain, info.code, ::fast_io::details::herbception_encoding_impl<char_type>, info.info);
}

inline constexpr ::fast_io::manipulators::herbceptions_query_information print_alias_define(::fast_io::io_alias_t, ::std::error const &e) noexcept
{
	return {e.domain(), e.code(), ::std::error_query_information::name_message};
}

} // namespace fast_io
