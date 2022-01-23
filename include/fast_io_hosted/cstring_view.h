#pragma once

namespace fast_io
{
struct null_terminated_t
{
explicit inline constexpr null_terminated_t() noexcept = default;
};
inline constexpr null_terminated_t null_terminated{};

template<typename ch_type>
class basic_cstring_view
{
public:
	using char_type = ch_type;
	using size_type = std::size_t;
	using ptrdiff_t = std::ptrdiff_t;
	using pointer = ch_type const*;
	using const_pointer = ch_type const*;
	using iterator = ch_type const*;
	using const_iterator = ch_type const*;
	using reference = char_type const&;
	using const_reference = char_type const&;
	using difference_type = std::ptrdiff_t;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	const_pointer ptr{};
	size_type n{};

	constexpr basic_cstring_view() noexcept=default;
	constexpr basic_cstring_view(decltype(nullptr)) noexcept=delete;
	template<std::size_t len>
	requires (len!=0)
	constexpr basic_cstring_view(ch_type const (&cstr)[len]) noexcept:ptr{cstr},n{len-1}
	{
	}
	template<typename T>
	requires (std::convertible_to<T,const_pointer>&&!std::is_array_v<T>)
	constexpr basic_cstring_view(T const& cstr) noexcept:ptr{cstr},n{cstr_len(cstr)}
	{
	}
	constexpr basic_cstring_view(null_terminated_t, const_pointer c, size_type len) noexcept:ptr(c),n(len){}

	constexpr basic_cstring_view(null_terminated_t, decltype(nullptr), size_type len) noexcept=delete;
#if  !defined(_LIBCPP_VERSION)
	template<std::ranges::contiguous_range crg>
	requires (std::same_as<std::ranges::range_value_t<crg>, ch_type>&&requires(crg cont)
	{
		{cont.c_str()}->std::same_as<const_pointer>;
	})
	constexpr basic_cstring_view(crg const& cont):ptr{std::ranges::data(cont)},n{std::ranges::size(cont)}{}
#endif

      template<::fast_io::freestanding::contiguous_iterator It, std::sized_sentinel_for<It> End>
	requires std::same_as<::fast_io::freestanding::iter_value_t<It>, ch_type>
	  && (!std::convertible_to<End, size_type>)
	constexpr basic_cstring_view(null_terminated_t, It first, End last):
		ptr{::fast_io::freestanding::to_address(first)},n{static_cast<std::size_t>(last-first)}{}
	constexpr const_pointer c_str() const noexcept
	{
		return ptr;
	}
	constexpr const_pointer data() const noexcept
	{
		return ptr;
	}
	constexpr size_type size() const noexcept
	{
		return n;
	}
	constexpr bool empty() const noexcept
	{
		return !n;
	}
	constexpr const_iterator cbegin() const noexcept
	{
		return ptr;
	}
	constexpr const_iterator begin() const noexcept
	{
		return ptr;
	}
	constexpr const_iterator cend() const noexcept
	{
		return ptr+n;
	}
	constexpr const_iterator end() const noexcept
	{
		return ptr+n;
	}
	inline static constexpr std::size_t npos = static_cast<std::size_t>(-1);
	constexpr std::size_t max_size() const noexcept
	{
		return static_cast<std::size_t>(npos)/sizeof(ch_type);
	}
	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{ptr+n};
	}
	constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{ptr+n};
	}
	constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{ptr};
	}
	constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{ptr};
	}
	constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{ptr};
	}
	constexpr const_reference front() const noexcept
	{
		return *ptr;
	}
	constexpr reference front() noexcept
	{
		return *ptr;
	}
	constexpr const_reference back() const noexcept
	{
		return ptr[n-1];
	}
	constexpr reference back() noexcept
	{
		return ptr[n-1];
	}
	constexpr reference operator[](size_type size) noexcept
	{
		return ptr[size];
	}
	constexpr const_reference operator[](size_type size) const noexcept
	{
		return ptr[size];
	}
	constexpr std::size_t rfind(char_type ch) const noexcept
	{
		for(std::size_t i{n};i--;)
			if(ptr[i]==ch)
				return i;
		return npos;
	}
};

#if __cpp_lib_three_way_comparison >= 201907L
namespace details
{

template<std::integral ch_type>
inline constexpr bool cstring_view_cmp_eq(ch_type const* a,std::size_t alen,ch_type const* b,std::size_t blen) noexcept
{
	if(alen!=blen)[[likely]]
		return false;
	for(std::size_t i{};i!=alen;++i)
		if(a[i]!=b[i])
			return false;
	return true;
}

template<std::integral ch_type>
inline constexpr auto cstring_view_cmp_threeway(ch_type const* a,std::size_t alen,ch_type const* b,std::size_t blen) noexcept
{
	auto len_cmp_res{alen<=>blen};
	if(len_cmp_res==0)[[likely]]
		return len_cmp_res;
	for(std::size_t i{};i!=alen;++i)
	{
		auto r{a[i]<=>b[i]};
		if(r==0)
			return r;
	}
	return ;
}

}

template<std::integral ch_type>
inline constexpr bool operator==(basic_cstring_view<ch_type> a,basic_cstring_view<ch_type> b) noexcept
{
	return ::fast_io::details::cstring_view_cmp_eq(a.ptr,a.n,b.ptr,b.n);
}

template<std::integral ch_type>
inline constexpr int operator<=>(basic_cstring_view<ch_type> a,basic_cstring_view<ch_type> b) noexcept
{
	return ::fast_io::details::cstring_view_cmp_threeway(a.ptr,a.n,b.ptr,b.n);
}

#endif

using cstring_view = basic_cstring_view<char>;
using wcstring_view = basic_cstring_view<wchar_t>;
using u8cstring_view = basic_cstring_view<char8_t>;
using u16cstring_view = basic_cstring_view<char16_t>;
using u32cstring_view = basic_cstring_view<char32_t>;

template<typename T>
concept constructible_to_path =
	std::constructible_from<cstring_view,T>||
	std::constructible_from<wcstring_view,T>||
	std::constructible_from<u8cstring_view,T>||
	std::constructible_from<u16cstring_view,T>||
	std::constructible_from<u32cstring_view,T>;
}
