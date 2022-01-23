#pragma once

#if defined(_GLIBCXX_STRING_VIEW) || defined(_LIBCPP_STRING_VIEW) || defined(_STRING_VIEW_)

namespace fast_io::freestanding
{
using ::std::char_traits;
using ::std::basic_string_view;
using ::std::string_view;
using wstring_view=::std::basic_string_view<wchar_t>;
using ::std::u8string_view;
using ::std::u16string_view;
using ::std::u32string_view;
}
#else
namespace fast_io::freestanding
{

/*
dummy char_traits which do not do anything
*/
template<typename T>
struct char_traits
{};

template<std::integral ch_type,typename traits=char_traits<ch_type>>
class basic_string_view
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
	char_type const* ptr{};
	size_type n{};
	constexpr basic_string_view() noexcept=default;
	constexpr basic_string_view(basic_string_view const& other) noexcept = default;
	constexpr basic_string_view(char_type const* s, size_type count):ptr(s),n(count){}
	constexpr basic_string_view(char_type const* s):ptr(s),n(::fast_io::cstr_len(s)){}
	template<contiguous_iterator Iter>
	requires std::same_as<iter_value_t<Iter>,char>
	constexpr basic_string_view(Iter first, Iter last):ptr(to_address(first)),n(static_cast<size_type>(last-first)){}
	constexpr basic_string_view& operator=(basic_string_view const& view) noexcept = default;
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
	constexpr std::size_t find(char_type ch) const noexcept
	{
		for(std::size_t i{};i!=n;++i)
			if(ptr[i]==ch)
				return i;
		return npos;
	}
};

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u8string_view = basic_string_view<char8_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

}
#endif
