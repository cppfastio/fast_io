#pragma once

#include "common.h"

namespace fast_io
{

namespace containers
{
template <typename T, typename allocator>
class deque;

namespace details
{
struct deque_controller_common;
}

} // namespace containers

namespace details
{

template <::std::integral char_type, typename T>
inline constexpr char_type *pr_rsv_define_deque_debug_impl(char_type *iter, T const &val) noexcept
{
	if constexpr (::std::same_as<char_type, char>)
	{
		iter = ::fast_io::details::copy_string_literal("fast_io deque controller:\n\tfront_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tfront_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tfront_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tfront_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_end_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tback_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tback_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tback_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tback_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_end_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tcontroller_block.controller_start_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tcontroller_block.controller_start_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tcontroller_block.controller_after_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal("\n\tcontroller_block.controller_after_ptr:", iter);
		return ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_ptr));
	}
	else if constexpr (::std::same_as<char_type, wchar_t>)
	{
		iter = ::fast_io::details::copy_string_literal(L"fast_io deque controller:\n\tfront_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tfront_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tfront_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tfront_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_end_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tback_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tback_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tback_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tback_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_end_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tcontroller_block.controller_start_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tcontroller_block.controller_start_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tcontroller_block.controller_after_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal(L"\n\tcontroller_block.controller_after_ptr:", iter);
		return ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_ptr));
	}
	else if constexpr (::std::same_as<char_type, char16_t>)
	{
		iter = ::fast_io::details::copy_string_literal(u"fast_io deque controller:\n\tfront_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tfront_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tfront_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tfront_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_end_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tback_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tback_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tback_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tback_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_end_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tcontroller_block.controller_start_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tcontroller_block.controller_start_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tcontroller_block.controller_after_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal(u"\n\tcontroller_block.controller_after_ptr:", iter);
		return ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_ptr));
	}
	else if constexpr (::std::same_as<char_type, char32_t>)
	{
		iter = ::fast_io::details::copy_string_literal(U"fast_io deque controller:\n\tfront_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tfront_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tfront_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tfront_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_end_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tback_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tback_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tback_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tback_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_end_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tcontroller_block.controller_start_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tcontroller_block.controller_start_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tcontroller_block.controller_after_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal(U"\n\tcontroller_block.controller_after_ptr:", iter);
		return ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_ptr));
	}
	else
	{
		iter = ::fast_io::details::copy_string_literal(u8"fast_io deque controller:\n\tfront_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tfront_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tfront_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tfront_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.front_end_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tback_block.begin_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.begin_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tback_block.curr_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.curr_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tback_block.controller_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_block.controller_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tback_end_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.back_end_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tcontroller_block.controller_start_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tcontroller_block.controller_start_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_start_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tcontroller_block.controller_after_reserved_ptr:", iter);
		iter = ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_reserved_ptr));
		iter = ::fast_io::details::copy_string_literal(u8"\n\tcontroller_block.controller_after_ptr:", iter);
		return ::fast_io::pr_rsv_to_iterator_unchecked(iter, ::fast_io::mnp::pointervw(val.controller_block.controller_after_ptr));
	}
}

} // namespace details

namespace manipulators
{
template <::std::integral char_type, typename T, typename allocator>
inline constexpr ::std::size_t print_reserve_size(::fast_io::io_reserve_type_t<char_type,
																			   ::fast_io::manipulators::debug_view_t<::fast_io::containers::deque<T, allocator> const &>>) noexcept
{
	constexpr ::std::size_t ptrlen{::fast_io::pr_rsv_size<char_type,
														  decltype(::fast_io::mnp::pointervw(::std::declval<void *>()))>};
	constexpr ::std::size_t literal_counts{12u};
	constexpr std::size_t literal_total{
		sizeof(
			u8"fast_io deque controller:\n\tfront_block.begin_ptr:"
			u8"\n\tfront_block.curr_ptr:"
			u8"\n\tfront_block.controller_ptr:"
			u8"\n\tfront_end_ptr:"
			u8"\n\tback_block.begin_ptr:"
			u8"\n\tback_block.curr_ptr:"
			u8"\n\tback_block.controller_ptr:"
			u8"\n\tback_end_ptr:"
			u8"\n\tcontroller_block.controller_start_ptr:"
			u8"\n\tcontroller_block.controller_start_reserved_ptr:"
			u8"\n\tcontroller_block.controller_after_reserved_ptr:"
			u8"\n\tcontroller_block.controller_after_ptr:") -
		1u};
	return literal_total + literal_counts * ptrlen;
}

template <::std::integral char_type, typename T, typename allocator>
inline constexpr char_type *print_reserve_define(
	::fast_io::io_reserve_type_t<char_type, ::fast_io::manipulators::debug_view_t<::fast_io::containers::deque<T, allocator> const &>>,
	char_type *ptr, ::fast_io::manipulators::debug_view_t<::fast_io::containers::deque<T, allocator> const &> val) noexcept
{
	FAST_IO_IF_CONSTEVAL
	{
		return ::fast_io::details::pr_rsv_define_deque_debug_impl(ptr, val.reference.controller);
	}
	else
	{
		return ::fast_io::details::pr_rsv_define_deque_debug_impl(
			ptr,
			*reinterpret_cast<
				::fast_io::containers::details::deque_controller_common const *>(
				__builtin_addressof(val.reference.controller)));
	}
}

} // namespace manipulators

} // namespace fast_io
