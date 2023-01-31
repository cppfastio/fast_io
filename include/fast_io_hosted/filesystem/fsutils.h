#pragma once

namespace fast_io::details
{

template<std::integral char_type>
inline constexpr char_type const* my_rfind_dot(char_type const* first,char_type const* last) noexcept
{
	for(;first!=last;--last)
	{
		auto fp1{last};
		--fp1;
		if(*fp1==u8'.')
			return last;
	}
	return first;
}

template<bool stem,std::integral return_char_type,std::integral char_type>
requires (sizeof(return_char_type)==sizeof(char_type))
inline constexpr std::conditional_t<stem,
	::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<return_char_type>,
	::fast_io::manipulators::basic_os_c_str_with_known_size<return_char_type>>
	find_dot_and_sep(char_type const* beg_ptr,std::size_t namlen) noexcept
{
	if constexpr(std::same_as<return_char_type,char_type>)
	{
		char_type const* ed_ptr{beg_ptr+namlen};
		char_type const* f{my_rfind_dot(beg_ptr,ed_ptr)};
		std::size_t diff{static_cast<std::size_t>(f-beg_ptr)};
		if(diff<2||(diff==2&&2<namlen&&*beg_ptr==u8'.'))
		{
			if constexpr(stem)
			{
				return {beg_ptr,namlen};
			}
			else
			{
				return {ed_ptr,0};
			}
		}
		if constexpr(stem)
		{
			constexpr std::size_t two{2};
			diff-=two;
			return {beg_ptr,diff};
		}
		else
		{
			--f;
			std::size_t retdiff{static_cast<std::size_t>(ed_ptr-f)};
			return {f,retdiff};
		}
	}
	else
	{
		using return_char_type_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
		= return_char_type const*;
		auto rtval{find_dot_and_sep<stem,char_type,char_type>(beg_ptr,namlen)};
		return {reinterpret_cast<return_char_type_const_may_alias_ptr>(rtval.ptr),rtval.n};
	}
}

}
