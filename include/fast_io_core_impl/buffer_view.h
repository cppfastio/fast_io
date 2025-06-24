#pragma once

namespace fast_io
{

template <::std::integral ch_type>
struct basic_ibuffer_view
{
	using char_type = ch_type;
	using input_char_type = char_type;
	char_type const *begin_ptr{};
	char_type const *curr_ptr{};
	char_type const *end_ptr{};
	inline constexpr basic_ibuffer_view() noexcept = default;
	template <::std::contiguous_iterator Iter>
		requires ::std::same_as<::std::remove_cvref_t<::std::iter_value_t<Iter>>, char_type>
	inline constexpr basic_ibuffer_view(Iter first, Iter last) noexcept
		: begin_ptr{::std::to_address(first)}, curr_ptr{begin_ptr}, end_ptr{::std::to_address(last)}
	{
	}
	template <::std::ranges::contiguous_range rg>
		requires(::std::same_as<::std::ranges::range_value_t<rg>, char_type> &&
				 !::std::is_array_v<::std::remove_cvref_t<rg>>)
	inline explicit constexpr basic_ibuffer_view(rg &r) noexcept
		: basic_ibuffer_view(::std::ranges::cbegin(r), ::std::ranges::cend(r))
	{
	}
	inline constexpr void clear() noexcept
	{
		curr_ptr = end_ptr;
	}
};

template <::std::integral char_type>
struct basic_ibuffer_view_ref
{
	using input_char_type = typename basic_ibuffer_view<char_type>::input_char_type;
	using native_handle_type = basic_ibuffer_view<char_type> *;
	native_handle_type ptr{};
};

#if 0
template<::std::integral ch_type,::std::contiguous_iterator Iter>
requires ::std::same_as<::std::iter_value_t<Iter>,ch_type>
[[nodiscard]] inline constexpr Iter read(basic_ibuffer_view<ch_type>& view,Iter first,Iter last) noexcept
{
	auto diff{last-first};
	auto view_diff{view.end_ptr-view.curr_ptr};
	if(view_diff<diff)
		diff=view_diff;
	auto it{::fast_io::details::non_overlapped_copy_n(view.curr_ptr,static_cast<::std::size_t>(view_diff),first)};
	view.curr_ptr+=diff;
	return it;
}
#endif

template <::std::integral ch_type>
inline constexpr basic_ibuffer_view_ref<ch_type> input_stream_ref_define(basic_ibuffer_view<ch_type> &other) noexcept
{
	return {__builtin_addressof(other)};
}

template <::std::integral ch_type>
inline constexpr basic_ibuffer_view_ref<ch_type> input_stream_ref_define(basic_ibuffer_view<ch_type> &&other) noexcept
{
	return {__builtin_addressof(other)};
}

template <::std::integral ch_type>
inline constexpr basic_ibuffer_view_ref<ch_type> input_bytes_stream_ref_define(basic_ibuffer_view<ch_type> &other) noexcept
{
	return {__builtin_addressof(other)};
}

template <::std::integral ch_type>
inline constexpr basic_ibuffer_view_ref<ch_type> input_bytes_stream_ref_define(basic_ibuffer_view<ch_type> &&other) noexcept
{
	return {__builtin_addressof(other)};
}

template <::std::integral ch_type>
[[nodiscard]] inline constexpr ch_type const *ibuffer_begin(basic_ibuffer_view_ref<ch_type> view) noexcept
{
	return view.ptr->begin_ptr;
}

template <::std::integral ch_type>
[[nodiscard]] inline constexpr ch_type const *ibuffer_curr(basic_ibuffer_view_ref<ch_type> view) noexcept
{
	return view.ptr->curr_ptr;
}

template <::std::integral ch_type>
[[nodiscard]] inline constexpr ch_type const *ibuffer_end(basic_ibuffer_view_ref<ch_type> view) noexcept
{
	return view.ptr->end_ptr;
}

template <::std::integral ch_type>
inline constexpr void ibuffer_set_curr(basic_ibuffer_view_ref<ch_type> view, ch_type const *ptr) noexcept
{
	view.ptr->curr_ptr = ptr;
}

template <::std::integral ch_type>
[[nodiscard]] inline constexpr bool ibuffer_underflow(basic_ibuffer_view_ref<ch_type>) noexcept
{
	return false;
}

template <::std::integral ch_type>
inline constexpr bool ibuffer_underflow_never(basic_ibuffer_view_ref<ch_type>) noexcept
{
	return true;
}

template <::std::integral ch_type>
struct basic_obuffer_view
{
	using char_type = ch_type;
	using output_char_type = char_type;
	char_type *begin_ptr{}, *curr_ptr{}, *end_ptr{};
	inline constexpr basic_obuffer_view() noexcept = default;
	template <::std::contiguous_iterator Iter>
		requires ::std::same_as<::std::iter_value_t<Iter>, char_type>
	inline constexpr basic_obuffer_view(Iter first, Iter last) noexcept
		: begin_ptr{::std::to_address(first)}, curr_ptr{begin_ptr}, end_ptr{::std::to_address(last)}
	{
	}
	template <::std::ranges::contiguous_range rg>
		requires(::std::same_as<::std::ranges::range_value_t<rg>, char_type> &&
				 !::std::is_array_v<::std::remove_cvref_t<rg>>)
	inline explicit constexpr basic_obuffer_view(rg &r) noexcept
		: basic_obuffer_view(::std::ranges::begin(r), ::std::ranges::end(r))
	{
	}
	inline constexpr void clear() noexcept
	{
		curr_ptr = begin_ptr;
	}
	inline constexpr char_type const *cbegin() const noexcept
	{
		return begin_ptr;
	}
	inline constexpr char_type const *cend() const noexcept
	{
		return curr_ptr;
	}
	inline constexpr char_type const *begin() const noexcept
	{
		return begin_ptr;
	}
	inline constexpr char_type const *end() const noexcept
	{
		return curr_ptr;
	}
	inline constexpr char_type *begin() noexcept
	{
		return begin_ptr;
	}
	inline constexpr char_type *end() noexcept
	{
		return curr_ptr;
	}

	inline constexpr char_type const *data() const noexcept
	{
		return begin_ptr;
	}
	inline constexpr char_type *data() noexcept
	{
		return begin_ptr;
	}

	inline constexpr ::std::size_t size() const noexcept
	{
		return static_cast<::std::size_t>(curr_ptr - begin_ptr);
	}

	inline constexpr ::std::size_t capacity() const noexcept
	{
		return static_cast<::std::size_t>(end_ptr - begin_ptr);
	}
};

template <::std::integral char_type>
struct basic_obuffer_view_ref
{
	using output_char_type = typename basic_obuffer_view<char_type>::output_char_type;
	using native_handle_type = basic_obuffer_view<char_type> *;
	native_handle_type ptr{};
};

#if 0
template<::std::integral ch_type,::std::contiguous_iterator Iter>
requires ::std::same_as<::std::iter_value_t<Iter>,ch_type>
inline constexpr void write(basic_obuffer_view<ch_type>& view,Iter first,Iter last) noexcept
{
	auto diff{last-first};
	auto view_diff{view.end_ptr-view.curr_ptr};
	if(view_diff<diff)
		fast_terminate();
	view.curr_ptr=::fast_io::details::non_overlapped_copy_n(first,static_cast<::std::size_t>(diff),view.curr_ptr);
}
#endif

template <::std::integral ch_type>
inline constexpr basic_obuffer_view_ref<ch_type> output_stream_ref_define(basic_obuffer_view<ch_type> &other) noexcept
{
	return {__builtin_addressof(other)};
}

template <::std::integral ch_type>
inline constexpr basic_obuffer_view_ref<ch_type> output_stream_ref_define(basic_obuffer_view<ch_type> &&other) noexcept
{
	return {__builtin_addressof(other)};
}

template <::std::integral ch_type>
inline constexpr basic_obuffer_view_ref<ch_type> output_bytes_stream_ref_define(basic_obuffer_view<ch_type> &other) noexcept
{
	return {__builtin_addressof(other)};
}

template <::std::integral ch_type>
inline constexpr basic_obuffer_view_ref<ch_type> output_bytes_stream_ref_define(basic_obuffer_view<ch_type> &&other) noexcept
{
	return {__builtin_addressof(other)};
}

template <::std::integral ch_type>
[[nodiscard]] inline constexpr ch_type *obuffer_begin(basic_obuffer_view_ref<ch_type> view) noexcept
{
	return view.ptr->begin_ptr;
}

template <::std::integral ch_type>
[[nodiscard]] inline constexpr ch_type *obuffer_curr(basic_obuffer_view_ref<ch_type> view) noexcept
{
	return view.ptr->curr_ptr;
}

template <::std::integral ch_type>
[[nodiscard]] inline constexpr ch_type *obuffer_end(basic_obuffer_view_ref<ch_type> view) noexcept
{
	return view.ptr->end_ptr;
}

template <::std::integral ch_type>
inline constexpr void obuffer_set_curr(basic_obuffer_view_ref<ch_type> view, ch_type *ptr) noexcept
{
	view.ptr->curr_ptr = ptr;
}

template <::std::integral ch_type>
inline constexpr void obuffer_overflow(basic_obuffer_view_ref<ch_type>, ch_type) noexcept
{
	fast_terminate();
}

template <::std::integral ch_type>
inline constexpr bool obuffer_overflow_never(basic_obuffer_view<ch_type>) noexcept
{
	return true;
}

using ibuffer_view = basic_ibuffer_view<char>;
using wibuffer_view = basic_ibuffer_view<wchar_t>;
using u8ibuffer_view = basic_ibuffer_view<char8_t>;
using u16ibuffer_view = basic_ibuffer_view<char16_t>;
using u32ibuffer_view = basic_ibuffer_view<char32_t>;
using obuffer_view = basic_obuffer_view<char>;
using wobuffer_view = basic_obuffer_view<wchar_t>;
using u8obuffer_view = basic_obuffer_view<char8_t>;
using u16obuffer_view = basic_obuffer_view<char16_t>;
using u32obuffer_view = basic_obuffer_view<char32_t>;
} // namespace fast_io
