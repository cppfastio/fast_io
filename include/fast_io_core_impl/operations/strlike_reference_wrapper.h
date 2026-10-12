#pragma once

namespace fast_io
{

template <::std::integral ch_type, typename T>
struct io_strlike_reference_wrapper
{
	using value_type = T;
	using native_handle_type = value_type *;
	using char_type = ch_type;
	using output_char_type = char_type;
	using pointer = char_type *;
	using const_pointer = char_type const *;
	native_handle_type ptr{};
	// a strlike that can never report allocation/contract failure makes the
	// stream wrapper non-throwing so printing into it needs no try/catch
	using output_nothrow_tag =
		::std::conditional_t<!::fast_io::details::strlike_output_ops_may_throw<ch_type, T>,
							 ::fast_io::io_nothrow_tag, void>;
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{ptr};
		ptr = nullptr;
		return temp;
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return ptr;
	}
};

template <::std::integral char_type, typename T>
[[nodiscard]] inline constexpr io_strlike_reference_wrapper<char_type, T>
output_stream_ref_define(io_strlike_reference_wrapper<char_type, T> bref) noexcept
{
	return bref;
}

template <::std::integral char_type, typename T>
	requires buffer_strlike<char_type, T>
inline constexpr char_type *obuffer_begin(io_strlike_reference_wrapper<char_type, T> bref) noexcept
{
	return strlike_begin(::fast_io::io_strlike_type<char_type, T>, *bref.ptr);
}

template <::std::integral char_type, typename T>
	requires buffer_strlike<char_type, T>
inline constexpr char_type *obuffer_curr(io_strlike_reference_wrapper<char_type, T> bref) noexcept
{
	return strlike_curr(::fast_io::io_strlike_type<char_type, T>, *bref.ptr);
}

template <::std::integral char_type, typename T>
	requires buffer_strlike<char_type, T>
inline constexpr char_type *obuffer_end(io_strlike_reference_wrapper<char_type, T> bref) noexcept
{
	return strlike_end(::fast_io::io_strlike_type<char_type, T>, *bref.ptr);
}

template <::std::integral char_type, typename T>
	requires buffer_strlike<char_type, T>
inline constexpr void obuffer_set_curr(io_strlike_reference_wrapper<char_type, T> bref, char_type *i) noexcept
{
	return strlike_set_curr(::fast_io::io_strlike_type<char_type, T>, *bref.ptr, i);
}

template <::std::integral char_type, typename T>
	requires buffer_strlike<char_type, T>
inline constexpr void obuffer_flush_reserve_define(io_strlike_reference_wrapper<char_type, T> bref, ::std::size_t to_reserve)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::strlike_reserve_may_throw<char_type, T>)
{
	auto &strref{*bref.ptr};
	to_reserve = ::fast_io::details::intrinsics::add_or_overflow_die(static_cast<::std::size_t>(strlike_curr(::fast_io::io_strlike_type<char_type, T>, strref) - strlike_begin(::fast_io::io_strlike_type<char_type, T>, strref)), to_reserve);
	return strlike_reserve(::fast_io::io_strlike_type<char_type, T>, strref, to_reserve);
}

namespace details
{

template <::std::size_t size_char_type>
inline constexpr ::std::size_t cal_new_cap_io_strlike(::std::size_t cap) noexcept
{
	::std::size_t new_cap{};
	if (cap == 0)
	{
		new_cap = 1;
	}
	else
	{
		constexpr ::std::size_t mx_size{SIZE_MAX / size_char_type};
		constexpr ::std::size_t mx_div2{static_cast<::std::size_t>(mx_size / 2u)};
		if (mx_size == cap) [[unlikely]]
		{
			fast_terminate();
		}
		else if (cap >= mx_div2)
		{
			new_cap = mx_size;
		}
		else
		{
			new_cap = cap;
			new_cap <<= 1u;
		}
	}
	return new_cap;
}

} // namespace details

template <::std::integral ch_type, typename T>
	requires buffer_strlike<ch_type, T>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void obuffer_overflow(io_strlike_reference_wrapper<ch_type, T> bref, ch_type ch)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		(::fast_io::auxiliary_strlike<ch_type, T> && ::fast_io::details::strlike_push_back_may_throw<ch_type, T>) ||
		(!::fast_io::auxiliary_strlike<ch_type, T> && ::fast_io::details::strlike_reserve_may_throw<ch_type, T>))
{
	auto &strref{*bref.ptr};
	if constexpr (auxiliary_strlike<ch_type, T>)
	{
		strlike_push_back(::fast_io::io_strlike_type<ch_type, T>, strref, ch);
	}
	else
	{
		auto bptr{strlike_begin(::fast_io::io_strlike_type<ch_type, T>, strref)};
		auto eptr{strlike_end(::fast_io::io_strlike_type<ch_type, T>, strref)};
		auto cap{static_cast<::std::size_t>(eptr - bptr)};
		strlike_reserve(::fast_io::io_strlike_type<ch_type, T>, strref,
						::fast_io::details::cal_new_cap_io_strlike<sizeof(ch_type)>(cap));
		auto curr_ptr{strlike_curr(::fast_io::io_strlike_type<ch_type, T>, strref)};
		*curr_ptr = ch;
		++curr_ptr;
		strlike_set_curr(::fast_io::io_strlike_type<ch_type, T>, strref, curr_ptr);
	}
}

template <::std::integral ch_type, typename T>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void write_all_overflow_define(io_strlike_reference_wrapper<ch_type, T> bref, ch_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		(::fast_io::auxiliary_strlike<ch_type, T> && ::fast_io::details::strlike_append_may_throw<ch_type, T>) ||
		(!::fast_io::auxiliary_strlike<ch_type, T> && ::fast_io::details::strlike_reserve_may_throw<ch_type, T>))
{
	auto &strref{*bref.ptr};
	if constexpr (auxiliary_strlike<ch_type, T>)
	{
		strlike_append(::fast_io::io_strlike_type<ch_type, T>, strref, first, count);
	}
	else
	{
		auto curr{strlike_curr(::fast_io::io_strlike_type<ch_type, T>, strref)};
		::std::size_t const bufferdiff{
			static_cast<::std::size_t>(strlike_end(::fast_io::io_strlike_type<ch_type, T>, strref) - curr)};
		curr = ::fast_io::freestanding::non_overlapped_copy_n(first, bufferdiff, curr);
		first += bufferdiff;
		strlike_set_curr(::fast_io::io_strlike_type<ch_type, T>, strref, curr);
		auto bptr{strlike_begin(::fast_io::io_strlike_type<ch_type, T>, strref)};
		auto eptr{strlike_end(::fast_io::io_strlike_type<ch_type, T>, strref)};
		auto cap{static_cast<::std::size_t>(eptr - bptr)};
		::std::size_t new_cap{::fast_io::details::cal_new_cap_io_strlike<sizeof(ch_type)>(cap)};
		::std::size_t const size_minimum{
			::fast_io::details::intrinsics::add_or_overflow_die(count, cap)};
		if (new_cap < size_minimum)
		{
			new_cap = size_minimum;
		}
		strlike_reserve(::fast_io::io_strlike_type<ch_type, T>, strref, new_cap);
		auto curr_ptr{strlike_curr(::fast_io::io_strlike_type<ch_type, T>, strref)};
		curr_ptr = ::fast_io::freestanding::non_overlapped_copy_n(first, count, curr_ptr);
		strlike_set_curr(::fast_io::io_strlike_type<ch_type, T>, strref, curr_ptr);
	}
}

} // namespace fast_io
