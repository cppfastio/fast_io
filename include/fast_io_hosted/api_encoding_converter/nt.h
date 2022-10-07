#pragma once

namespace fast_io
{
template<typename Allocator>
struct basic_nt_api_encoding_converter
{
	using allocator_type = Allocator;
	using char_type = char16_t;
	std::size_t buffer_capacity;
	char_type* buffer_data;
	char_type* buffer_data_end;
	template<std::integral from_char_type>
	requires (sizeof(from_char_type)!=2)
#if __cpp_constexpr >=201907L && __cpp_constexpr_dynamic_alloc >= 201907L && __cpp_lib_is_constant_evaluated >=201811L
	constexpr
#endif
	basic_nt_api_encoding_converter(from_char_type const* p,std::size_t sz) noexcept:
		buffer_capacity(details::cal_decorated_reserve_size<sizeof(from_char_type),sizeof(char_type)>(sz)),
		buffer_data(details::allocate_iobuf_space<char_type,allocator_type>(details::intrinsics::add_or_overflow_die(buffer_capacity,1))),
		buffer_data_end(details::codecvt::general_code_cvt_full(p,p+sz,buffer_data))
	{
		*buffer_data_end=0;
	}
	template<std::integral from_char_type>
	requires (sizeof(from_char_type)==2)
#if __cpp_constexpr >=201907L && __cpp_constexpr_dynamic_alloc >= 201907L && __cpp_lib_is_constant_evaluated >=201811L
	constexpr
#endif
	basic_nt_api_encoding_converter(from_char_type const* p,std::size_t sz) noexcept:
		buffer_capacity(sz),
		buffer_data(details::allocate_iobuf_space<char_type,allocator_type>(::fast_io::details::intrinsics::add_or_overflow_die(sz,1))),
		buffer_data_end(details::codecvt::general_code_cvt_full(p,p+sz,buffer_data))
	{
		*buffer_data_end=0;
	}
	inline constexpr char_type const* c_str() const noexcept
	{
		return buffer_data;
	}
#if defined(_WIN32) || defined(__CYGWIN__)
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	inline char16_t const* native_c_str() const noexcept
	{
		return reinterpret_cast<char16_t const*>(buffer_data);
	}
#endif
	inline constexpr std::size_t size() const noexcept
	{
		return static_cast<std::size_t>(buffer_data_end-buffer_data);
	}
	inline constexpr std::size_t capacity() const noexcept
	{
		return buffer_capacity;
	}
	basic_nt_api_encoding_converter(basic_nt_api_encoding_converter const&)=delete;
	basic_nt_api_encoding_converter& operator=(basic_nt_api_encoding_converter const&)=delete;
#if __cpp_constexpr >=201907L && __cpp_constexpr_dynamic_alloc >= 201907L && __cpp_lib_is_constant_evaluated >=201811L
	constexpr
#endif
	~basic_nt_api_encoding_converter()
	{
		details::deallocate_iobuf_space<false,char_type,allocator_type>(buffer_data,buffer_capacity+1);
	}
};
#if (defined(_WIN32) || defined(__CYGWIN__)) && !defined(__WINE__)
using nt_api_encoding_converter = basic_nt_api_encoding_converter<generic_allocator_adapter<::fast_io::nt_rtlallocateheap_allocator>>;
#endif
}