#pragma once

namespace fast_io::details
{

template<typename T>
concept cxx_std_filesystem_pseudo_concept = requires(T t)
{
	typename T::value_type;
	{t.native().c_str()}->std::same_as<typename T::value_type const*>;
	{t.native().size()}->std::unsigned_integral;
	{t.root_name()};
};

template<typename char_type,typename Func>
concept api_common_has_size_overload = requires(Func callback,char_type const* cstr,std::size_t size)
{
	callback(cstr,size);
};

template<typename allocator_type,
	std::integral os_char_type,typename Func>
inline auto api_common_copy_append0_strvw_case_impl(os_char_type const* filename_strvw_str,std::size_t filename_strvw_len,Func callback)
{
	std::size_t const filename_strvw_len_p1{::fast_io::details::intrinsics::add_or_overflow_die(filename_strvw_len,1)};
	::fast_io::details::buffer_alloc_arr_ptr<os_char_type,false,allocator_type> temp(filename_strvw_len_p1);
	auto ptr{temp.ptr};
	non_overlapped_copy_n(filename_strvw_str,filename_strvw_len,ptr);
	ptr[filename_strvw_len]=0;
	if constexpr(::fast_io::details::api_common_has_size_overload<os_char_type,Func>)
		return callback(ptr,filename_strvw_len);
	else
		return callback(ptr);
}

}
