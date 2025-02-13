#pragma once

#include "rtti_hack/impl.h"
#include "fp_hack/impl.h"

namespace fast_io
{

template <typename T>
#if !defined(_MSC_VER) || defined(__clang__)
	requires ::std::derived_from<T, ::std::basic_streambuf<typename T::char_type, typename T::traits_type>>
#endif
class basic_general_streambuf_io_observer
{
public:
	using streambuf_type = T;
	using char_type = typename streambuf_type::char_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using traits_type = typename streambuf_type::traits_type;
	using native_handle_type = streambuf_type *;
	native_handle_type fb{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return fb;
	}
	inline constexpr native_handle_type release() noexcept
	{
		native_handle_type temp{fb};
		fb = nullptr;
		return temp;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return fb;
	}
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(_MSVC_STL_UPDATE)
	template <c_family family>
	inline explicit operator basic_c_family_io_observer<family, char_type>() const noexcept
	{
		return basic_c_io_observer<char_type>{details::streambuf_hack::fp_hack(fb)};
	}
#if !defined(__AVR__)
	template <posix_family fam>
	inline explicit operator basic_posix_family_io_observer<fam, char_type>() const noexcept
	{
		return static_cast<basic_posix_family_io_observer<fam, char_type>>(static_cast<basic_c_io_observer<char_type>>(*this));
	}
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
	template <win32_family fam>
	inline explicit operator basic_win32_family_io_observer<fam, char_type>() const noexcept
	{
		return static_cast<basic_win32_family_io_observer<fam, char_type>>(static_cast<basic_posix_io_observer<char_type>>(*this));
	}
	template <nt_family fam>
	inline explicit operator basic_nt_family_io_observer<fam, char_type>() const noexcept
	{
		return static_cast<basic_nt_family_io_observer<fam, char_type>>(
			static_cast<basic_posix_io_observer<char_type>>(*this));
	}
#endif
#endif
#endif
};


template <typename T>
inline constexpr bool operator==(basic_general_streambuf_io_observer<T> a,
								 basic_general_streambuf_io_observer<T> b) noexcept
{
	return a.fb == b.fb;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <typename T>
inline constexpr auto operator<=>(basic_general_streambuf_io_observer<T> a,
								  basic_general_streambuf_io_observer<T> b) noexcept
{
	return a.fb <=> b.fb;
}
#endif

template <::std::integral CharT, typename Traits = ::std::char_traits<CharT>>
using basic_streambuf_io_observer = basic_general_streambuf_io_observer<::std::basic_streambuf<CharT, Traits>>;

using streambuf_io_observer = basic_streambuf_io_observer<char>;
#if (!defined(_LIBCPP_VERSION)) || _LIBCPP_HAS_WIDE_CHARACTERS
using wstreambuf_io_observer = basic_streambuf_io_observer<wchar_t>;
#endif

#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(_MSVC_STL_UPDATE)

#if 0
template<::std::integral ch_type,typename Traits>
requires zero_copy_input_stream<basic_c_io_observer<ch_type>>
inline constexpr decltype(auto) zero_copy_in_handle(basic_filebuf_io_observer<ch_type,Traits> h)
{
	return zero_copy_in_handle(static_cast<basic_c_io_observer<ch_type>>(h));
}
template<::std::integral ch_type,typename Traits>
requires zero_copy_output_stream<basic_c_io_observer<ch_type>>
inline constexpr decltype(auto) zero_copy_out_handle(basic_filebuf_io_observer<ch_type,Traits> h)
{
	return zero_copy_out_handle(static_cast<basic_c_io_observer<ch_type>>(h));
}
#endif

#if !defined(_LIBCPP_HAS_NO_FILESYSTEM) || defined(_LIBCPP_HAS_FSTREAM)

template <::std::integral CharT, typename Traits = ::std::char_traits<CharT>>
using basic_filebuf_io_observer = basic_general_streambuf_io_observer<::std::basic_filebuf<CharT, Traits>>;
using filebuf_io_observer = basic_filebuf_io_observer<char>;
#if (!defined(_LIBCPP_VERSION)) || _LIBCPP_HAS_WIDE_CHARACTERS
using wfilebuf_io_observer = basic_filebuf_io_observer<wchar_t>;
#endif

template <::std::integral ch_type, typename traits_type>
	requires requires(basic_c_io_observer<ch_type> piob) { status(piob); }
inline constexpr auto status(basic_streambuf_io_observer<ch_type, traits_type> ciob)
{
	return status(static_cast<basic_c_io_observer<ch_type>>(ciob));
}

template <::std::integral ch_type, typename traits_type>
	requires requires(basic_c_io_observer<ch_type> piob) { status(piob); }
inline constexpr auto status(basic_filebuf_io_observer<ch_type, traits_type> ciob)
{
	return status(static_cast<basic_c_io_observer<ch_type>>(ciob));
}

template <::std::integral ch_type, typename traits_type>
inline ::std::size_t file_size(basic_filebuf_io_observer<ch_type, traits_type> ciob)
{
	return ::fast_io::details::posix_loader_get_file_size(details::fp_to_fd(details::streambuf_hack::fp_hack(ciob.fb)));
}

template <::std::integral char_type, typename traits_type>
inline constexpr posix_at_entry at(basic_filebuf_io_observer<char_type, traits_type> other) noexcept
{
	return posix_at_entry{details::fp_to_fd(details::streambuf_hack::fp_hack(other.fb))};
}
#endif

#endif
} // namespace fast_io

#include "bp_hack/impl.h"

#include "basicops.h"

#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(_MSVC_STL_UPDATE)
#include "preadwrite.h"
#endif
