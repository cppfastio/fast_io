#pragma once

namespace fast_io
{

template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr auto strlike_construct_define(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,char_type const* first,char_type const* last)
{
	return ::std::basic_string<char_type,traits_type,allocator_type>(first,last);
}

template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr auto strlike_construct_single_character_define(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,char_type ch)
{
	return ::std::basic_string<char_type,traits_type,allocator_type>(1,ch);
}

#if (!defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(_GLIBCXX_USE_CXX11_ABI))

template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr char_type* strlike_begin(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,::std::basic_string<char_type,traits_type,allocator_type>& str) noexcept
{
	return str.data();
}

template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr char_type* strlike_curr(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,::std::basic_string<char_type,traits_type,allocator_type>& str) noexcept
{
	return str.data()+str.size();
}

template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr char_type* strlike_end(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,::std::basic_string<char_type,traits_type,allocator_type>& str) noexcept
{
	return str.data()+str.capacity();
}
#if __cpp_lib_string_resize_and_overwrite >= 202110L
namespace details
{

template<std::integral char_type,typename size_type>
struct empty_string_set_ptr
{
	std::size_t realsize{};
	inline constexpr std::size_t operator()(char_type const*,size_type) noexcept
	{
		return realsize;
	}
};

}
#endif

template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr void strlike_set_curr(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,::std::basic_string<char_type,traits_type,allocator_type>& str,char_type* p)
{
#if (__cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L) && (__cpp_lib_string_resize_and_overwrite >= 202110L || __cpp_constexpr_dynamic_alloc >= 201907L)
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(std::is_constant_evaluated())
#endif
	{
		auto old_ptr{str.data()};
		std::size_t const sz{static_cast<std::size_t>(p-str.data())};
#if __cpp_lib_string_resize_and_overwrite >= 202110L
		
		str.resize_and_overwrite(sz,
			::fast_io::details::empty_string_set_ptr<char_type,typename ::std::basic_string<char_type,traits_type,allocator_type>::size_type>{sz});
#else
		auto curr_ptr{str.data()+str.size()};
		if(p<curr_ptr)
		{
			str.resize(sz);
		}
		else if(curr_ptr<p)
		{
			std::size_t const oldsz{str.size()};
			std::size_t const diff{static_cast<std::size_t>(p-curr_ptr)};
			::fast_io::details::local_operator_new_array_ptr<char_type> buffer(diff);
			for(std::size_t i{};i!=diff;++i)
				buffer[i]=curr_ptr[i];
			str.append(diff,0);
			auto newp{str.data()+oldsz};
			for(std::size_t i{};i!=diff;++i)
				newp[i]=buffer[i];
		}
#endif
		if(old_ptr!=str.data())
			::fast_io::fast_terminate();
	}
	else
#endif
	{
		traits_type::assign(*p, char_type());
		::fast_io::details::string_hack::set_end_ptr(str,p);
	}
}

template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr void strlike_reserve(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,::std::basic_string<char_type,traits_type,allocator_type>& str,std::size_t n)
{
	str.reserve(n);
}
#endif
template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr void strlike_append(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,::std::basic_string<char_type,traits_type,allocator_type>& str,char_type const* first,char_type const* last)
{
	str.append(first,last);
}

template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr void strlike_push_back(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>,::std::basic_string<char_type,traits_type,allocator_type>& str,char_type ch)
{
	str.push_back(ch);
}
#if (!defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(_GLIBCXX_USE_CXX11_ABI))
template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr std::size_t strlike_sso_size(io_strlike_type_t<char_type,::std::basic_string<char_type,traits_type,allocator_type>>) noexcept
{
	return ::fast_io::details::string_hack::local_capacity<::std::basic_string<char_type,traits_type,allocator_type>>();
}
#endif
template<std::integral char_type,typename traits_type,typename allocator_type>
inline constexpr io_strlike_reference_wrapper<char_type,::std::basic_string<char_type,traits_type,allocator_type>> io_strlike_ref(io_alias_t,::std::basic_string<char_type,traits_type,allocator_type>& str) noexcept
{
	return {__builtin_addressof(str)};
}


template<std::integral CharT,typename Traits = std::char_traits<CharT>,typename Allocator = std::allocator<CharT>>
using basic_ostring_ref = io_strlike_reference_wrapper<CharT,::std::basic_string<CharT,Traits,Allocator>>;
using ostring_ref = basic_ostring_ref<char>;
using wostring_ref = basic_ostring_ref<wchar_t>;
using u8ostring_ref = basic_ostring_ref<char8_t>;
using u16ostring_ref = basic_ostring_ref<char16_t>;
using u32ostring_ref = basic_ostring_ref<char32_t>;

static_assert(value_based_stream<ostring_ref>);
}
