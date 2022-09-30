#pragma once

namespace fast_io
{

namespace manipulators
{

template<typename T>
struct basic_str_get_all
{
	using manip_tag = manip_tag_t;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	T reference;
	std::size_t n;
};

template<typename T>
inline constexpr auto str_get_all(T&& reference,std::size_t n) noexcept
{
	return basic_str_get_all<decltype(io_strlike_ref(io_alias,reference))>{io_strlike_ref(io_alias,reference),n};
}

}

namespace details
{
struct str_get_all_context
{
	bool coping{};
};

template<::fast_io::freestanding::forward_iterator Iter,typename T>
inline constexpr parse_result<Iter> scan_context_define_str_get_all_buffer_common_impl(Iter first,Iter last,
	io_strlike_reference_wrapper<::fast_io::freestanding::iter_value_t<Iter>,T> output,std::size_t n)
{
	auto bg{obuffer_begin(output)};
	auto curr{obuffer_curr(output)};
	std::size_t elements{static_cast<std::size_t>(curr-bg)};
	std::size_t remain_characters{static_cast<std::size_t>(n-elements)};
	std::size_t itdiff{static_cast<std::size_t>(last-first)};
	std::size_t to_copy{remain_characters};
	bool not_enough{itdiff<remain_characters};
	if(not_enough)
	{
		to_copy=itdiff;
	}
	auto firstthen{first+to_copy};
	non_overlapped_copy_n(first,to_copy,curr);
	obuffer_set_curr(output,curr+to_copy);
	return {firstthen,(not_enough?(parse_code::partial):(parse_code::ok))};
}

template<::fast_io::freestanding::forward_iterator Iter,typename T>
inline constexpr parse_result<Iter> scan_context_define_str_get_all_buffer_strlike_impl(Iter first,Iter last,
	io_strlike_reference_wrapper<::fast_io::freestanding::iter_value_t<Iter>,T> output,std::size_t n,bool& ctx)
{
	if(!ctx)
	{
		obuffer_set_curr(output,obuffer_begin(output));
		strlike_reserve(io_strlike_type<::fast_io::freestanding::iter_value_t<Iter>,T>,*output.ptr,n);
		ctx=true;
	}
	return scan_context_define_str_get_all_buffer_common_impl(first,last,output,n);
}

template<::fast_io::freestanding::forward_iterator Iter,typename T>
inline constexpr parse_result<Iter> scan_context_define_str_get_all_general_strlike_impl(Iter first,Iter last,
	io_strlike_reference_wrapper<::fast_io::freestanding::iter_value_t<Iter>,T> output,
	std::size_t n,::fast_io::details::basic_concat_buffer<::fast_io::freestanding::iter_value_t<Iter>>& ctx)
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	auto ret{scan_context_define_str_get_all_buffer_common_impl(first,last,io_strlike_ref(ctx),n)};
	if(ret.code==parse_code::ok)
	{
		*output.ptr=strlike_construct_define(io_strlike_type<char_type,T>,ctx.buffer_begin,ctx.buffer_curr);;
	}
	return ret;
}
}

template<::std::integral char_type,typename T>
inline constexpr io_type_t<::std::conditional_t<::fast_io::buffer_strlike<char_type,T>,::fast_io::details::str_get_all_context,
	::fast_io::details::basic_concat_buffer<char_type>>>
	scan_context_type(io_reserve_type_t<char_type,
		::fast_io::manipulators::basic_str_get_all<io_strlike_reference_wrapper<char_type,T>>>) noexcept
{
	return {};
}

template<::fast_io::freestanding::forward_iterator Iter,typename ctxtype,typename T>
inline constexpr parse_result<Iter> scan_context_define(
	io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,
	::fast_io::manipulators::basic_str_get_all<io_strlike_reference_wrapper<::fast_io::freestanding::iter_value_t<Iter>,T>>>,
	ctxtype& ctx,
	Iter first,Iter last,
	::fast_io::manipulators::basic_str_get_all<io_strlike_reference_wrapper<::fast_io::freestanding::iter_value_t<Iter>,T>> str)
{
	if constexpr(::fast_io::buffer_strlike<::fast_io::freestanding::iter_value_t<Iter>,T>)
	{
		return ::fast_io::details::scan_context_define_str_get_all_buffer_strlike_impl(first,last,str.reference,str.n,ctx.coping);
	}
	else
	{
		return ::fast_io::details::scan_context_define_str_get_all_general_strlike_impl(first,last,str.reference,str.n,ctx);
	}
}

template<std::integral char_type,typename ctxtype,typename T>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_str_get_all<io_strlike_reference_wrapper<char_type,T>>>,
	ctxtype&,
	::fast_io::manipulators::basic_str_get_all<io_strlike_reference_wrapper<char_type,T>>)
{
	return parse_code::end_of_file;
}

}
