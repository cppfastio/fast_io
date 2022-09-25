#pragma once

namespace fast_io::details
{

template<std::integral ch_type>
struct basic_concat_buffer
{
	using char_type = ch_type;
	static inline constexpr std::size_t buffer_size{2048u/sizeof(ch_type)};
	char_type *buffer_begin,*buffer_curr,*buffer_end;
	char_type stack_buffer[buffer_size];
	constexpr basic_concat_buffer() noexcept:
		buffer_begin{stack_buffer},
		buffer_curr{stack_buffer},
		buffer_end{stack_buffer+buffer_size}
	{}
	basic_concat_buffer(basic_concat_buffer const&)=delete;
	basic_concat_buffer& operator=(basic_concat_buffer const&)=delete;
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	~basic_concat_buffer()
	{
		if(buffer_begin!=stack_buffer)[[unlikely]]
		{
			deallocate_iobuf_space<false,ch_type>(buffer_begin,static_cast<std::size_t>(buffer_end-buffer_begin));
		}
	}
};

template<std::integral char_type>
inline constexpr char_type* strlike_begin(::fast_io::io_strlike_type_t<char_type,::fast_io::details::basic_concat_buffer<char_type>>,::fast_io::details::basic_concat_buffer<char_type>& str) noexcept
{
	return str.buffer_begin;
}

template<std::integral char_type>
inline constexpr char_type* strlike_curr(::fast_io::io_strlike_type_t<char_type,::fast_io::details::basic_concat_buffer<char_type>>,::fast_io::details::basic_concat_buffer<char_type>& str) noexcept
{
	return str.buffer_curr;
}

template<std::integral char_type>
inline constexpr char_type* strlike_end(::fast_io::io_strlike_type_t<char_type,::fast_io::details::basic_concat_buffer<char_type>>,::fast_io::details::basic_concat_buffer<char_type>& str) noexcept
{
	return str.buffer_end;
}

template<std::integral char_type>
inline constexpr void strlike_set_curr(::fast_io::io_strlike_type_t<char_type,::fast_io::details::basic_concat_buffer<char_type>>,::fast_io::details::basic_concat_buffer<char_type>& str,char_type* p) noexcept
{
	str.buffer_curr=p;
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void basic_concat_buffer_strlike_reserve_cold_impl(::fast_io::details::basic_concat_buffer<char_type>& str,std::size_t n,std::size_t df) noexcept
{
	auto old_buffer_begin_ptr{str.buffer_begin};
	bool onstack{old_buffer_begin_ptr==str.stack_buffer};
	auto newptr{allocate_iobuf_space<char_type>(n)};
	std::size_t const elements{static_cast<std::size_t>(str.buffer_curr-str.buffer_begin)};
	auto newcurr_ptr{non_overlapped_copy_n(old_buffer_begin_ptr,elements,newptr)};
	str.buffer_curr=newcurr_ptr;
	if(!onstack)
	{
		deallocate_iobuf_space<false,char_type>(old_buffer_begin_ptr,df);
	}
	str.buffer_begin=newptr;
	str.buffer_end=newptr+n;
}

template<std::integral char_type>
inline constexpr void basic_concat_buffer_strlike_reserve_impl(::fast_io::details::basic_concat_buffer<char_type>& str,std::size_t n) noexcept
{
	std::size_t df{static_cast<std::size_t>(str.buffer_end-str.buffer_begin)};
	if(df<n)[[unlikely]]
		basic_concat_buffer_strlike_reserve_cold_impl(str,n,df);
}

template<std::integral char_type>
inline constexpr void strlike_reserve(::fast_io::io_strlike_type_t<char_type,::fast_io::details::basic_concat_buffer<char_type>>,::fast_io::details::basic_concat_buffer<char_type>& str,std::size_t n) noexcept
{
	basic_concat_buffer_strlike_reserve_impl(str,n);
}

template<std::integral char_type>
inline constexpr std::size_t strlike_sso_size(::fast_io::io_strlike_type_t<char_type,::fast_io::details::basic_concat_buffer<char_type>>) noexcept
{
	return ::fast_io::details::basic_concat_buffer<char_type>::buffer_size;
}

template<std::integral char_type>
inline constexpr io_strlike_reference_wrapper<char_type,::fast_io::details::basic_concat_buffer<char_type>> io_strlike_ref(io_alias_t,::fast_io::details::basic_concat_buffer<char_type>& str) noexcept
{
	return {__builtin_addressof(str)};
}

}
