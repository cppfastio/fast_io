#pragma once

namespace fast_io::containers
{
template <::std::integral char_type>
class basic_cstring_view;

template <::std::integral char_type>
class basic_string_view
{
public:
	using value_type = char_type;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using reference = value_type &;
	using const_reference = value_type const &;
	using pointer = value_type *;
	using const_pointer = value_type const *;
	using const_iterator = const_pointer;
	using iterator = const_iterator;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;

	inline constexpr basic_string_view() noexcept = default;

	inline constexpr basic_string_view(::std::nullptr_t) = delete;

	inline explicit constexpr basic_string_view(const_pointer p, size_type s) noexcept
		: ptr{p}, n{s}
	{}

	inline constexpr basic_string_view(basic_cstring_view<char_type> csv) noexcept
		: ptr{csv.data()}, n{csv.size()}
	{
	}

	template <::std::size_t N>
	inline constexpr basic_string_view(char_type const (&buffer)[N]) noexcept
	{
		constexpr ::std::size_t nm1{N - 1u};
		ptr = buffer;
		n = nm1;
	}

	template <::std::ranges::contiguous_range rg>
		requires(::std::same_as<::std::ranges::range_value_t<rg>, char_type> && !::std::is_array_v<::std::remove_cvref_t<rg>> &&
				 !::std::is_rvalue_reference_v<rg>)
	inline explicit constexpr basic_string_view(::fast_io::freestanding::from_range_t, rg &&r) noexcept
		: ptr{::std::ranges::cdata(r)}, n{::std::ranges::size(r)}
	{
	}

	inline constexpr basic_string_view(::fast_io::manipulators::basic_os_c_str<char_type> osstr) noexcept
		: ptr(osstr.ptr), n(::fast_io::cstr_len(osstr.ptr))
	{}
	inline constexpr basic_string_view(::fast_io::manipulators::basic_os_c_str_with_known_size<char_type> osstr) noexcept
		: ptr(osstr.ptr), n(osstr.n)
	{}
	inline constexpr basic_string_view(::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type> osstr) noexcept
		: ptr(osstr.ptr), n(osstr.n)
	{}

	inline constexpr basic_string_view(basic_string_view const &) noexcept = default;
	inline constexpr basic_string_view &operator=(basic_string_view const &) noexcept = default;

	const_pointer ptr{};
	size_type n{};

	[[nodiscard]] inline constexpr bool is_empty() const noexcept
	{
		return !n;
	}

	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return !n;
	}

	[[nodiscard]] inline constexpr size_type size() const noexcept
	{
		return n;
	}

	[[nodiscard]] inline static constexpr size_type max_size() noexcept
	{
		constexpr size_type mxsz{SIZE_MAX / sizeof(value_type)};
		return mxsz;
	}

	[[nodiscard]] inline constexpr size_type size_bytes() const noexcept
	{
		return n * sizeof(value_type);
	}

	[[nodiscard]] inline constexpr size_type max_size_bytes() const noexcept
	{
		constexpr size_type mxsz{SIZE_MAX / sizeof(value_type) * sizeof(value_type)};
		return mxsz;
	}

	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return const_iterator(ptr);
	}

	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return const_iterator(ptr);
	}

	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return const_iterator(ptr + n);
	}

	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return const_iterator(ptr + n);
	}

	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(ptr + n);
	}

	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(ptr + n);
	}

	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(ptr);
	}

	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(ptr);
	}
	[[nodiscard]]
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_pointer
	data() const noexcept
	{
		return ptr;
	}

	inline constexpr void swap(basic_string_view &other) noexcept
	{
		basic_string_view const tmp{other};
		other = *this;
		*this = tmp;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference
	operator[](size_type idx) const noexcept
	{
		if (n <= idx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return ptr[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference
	index_unchecked(size_type idx) const noexcept
	{
		return ptr[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference
	back() const noexcept
	{
		if (!n) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		size_type const nm1{n - 1};
		return ptr[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference
	back_unchecked() const noexcept
	{
		size_type const nm1{n - 1};
		return ptr[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference
	front() const noexcept
	{
		if (!n) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *ptr;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference
	front_unchecked() const noexcept
	{
		return *ptr;
	}

	inline constexpr bool starts_with(basic_string_view sv) const noexcept
	{
		if (sv.n <= n)
		{
			return ::std::equal(ptr, ptr + sv.n, sv.ptr, sv.ptr + sv.n);
		}
		return false;
	}
	inline constexpr bool starts_with_character(value_type ch) const noexcept
	{
		return !n && ch == *ptr;
	}

	inline constexpr bool ends_with(basic_string_view sv) const noexcept
	{
		if (sv.n <= n)
		{
			return ::std::equal(ptr + (n - sv.n), ptr + n, sv.ptr, sv.ptr + sv.n);
		}
		return false;
	}
	inline constexpr bool ends_with_character(value_type ch) const noexcept
	{
		return !n && ch == ptr[n - 1u];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr void remove_suffix(size_type svn) noexcept
	{
		if (n <= svn) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		this->n -= svn;
	}

	inline constexpr void remove_suffix_unchecked(size_type svn) noexcept
	{
		this->n -= svn;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr void remove_prefix(size_type svn) noexcept
	{
		if (n <= svn) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		this->ptr += svn;
		this->n -= svn;
	}

	inline constexpr void remove_prefix_unchecked(size_type svn) noexcept
	{
		this->ptr += svn;
		this->n -= svn;
	}
	inline constexpr bool contains(basic_string_view vw) const noexcept
	{
		auto ed{this->ptr + this->n};
		return ::std::search(this->ptr, ed, vw.ptr, vw.ptr + vw.n) != ed;
	}
	inline constexpr bool contains_character(char_type ch) const noexcept
	{
		auto ed{this->ptr + this->n};
		return ::std::find(this->ptr, ed, ch) != ed;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr basic_string_view subview_front(size_type count) const noexcept
	{
		if (this->n < count) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return basic_string_view(this->ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr basic_string_view subview_front_unchecked(size_type count) const noexcept
	{
		return basic_string_view(this->ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr basic_string_view subview_back(size_type count) const noexcept
	{
		if (this->n < count) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return basic_string_view(this->ptr + (this->n - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr basic_string_view subview_back_unchecked(size_type count) const noexcept
	{
		return basic_string_view(this->ptr + (this->n - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr basic_string_view subview(size_type pos, size_type count = ::fast_io::containers::npos) const noexcept
	{
		if (this->n < pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		size_type const val{this->n - pos};
		if (val < count)
		{
			if (count != ::fast_io::containers::npos) [[unlikely]]
			{
				::fast_io::fast_terminate();
			}
			count = val;
		}
		return basic_string_view(this->ptr + pos, count);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr basic_string_view subview_unchecked(size_type pos, size_type count = ::fast_io::containers::npos) const noexcept
	{
		size_type const val{this->n - pos};
		if (count == ::fast_io::containers::npos)
		{
			count = val;
		}
		return basic_string_view(this->ptr + pos, count);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr size_type copy(char_type *dest, size_type count, size_type pos = 0) const noexcept
	{
		size_type const thisn{this->n};
		if (thisn < pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return this->copy_unchecked(dest, count, pos);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr size_type copy_unchecked(char_type *dest, size_type count, size_type pos = 0) const noexcept
	{
		size_type const thisn{this->n};
		size_type const val{thisn - pos};
		if (val < count)
		{
			count = val;
		}
		auto start{this->ptr + pos};
		::std::copy(start, start + count, dest);
		return count;
	}

private:
	template <size_type situation>
	inline constexpr size_type find_character_common(char_type ch, size_type pos) const noexcept
	{
		static_assert(situation < 4);
		size_type thisn{this->n};
		if (thisn <= pos)
		{
			return ::fast_io::containers::npos;
		}
		auto bg{this->ptr};
		auto start{bg + pos};
		auto ed{bg + thisn};
		const_pointer it;
		if constexpr (situation == 0)
		{
			it = ::std::find(start, ed, ch);
		}
		else if constexpr (situation == 1)
		{
			it = ::fast_io::freestanding::find_not(start, ed, ch);
		}
		else if constexpr (situation == 2)
		{
			it = ::fast_io::freestanding::find_last(start, ed, ch);
		}
		else if constexpr (situation == 3)
		{
			it = ::fast_io::freestanding::find_last_not(start, ed, ch);
		}
		if (it == ed)
		{
			return ::fast_io::containers::npos;
		}
		return static_cast<size_type>(it - bg);
	}
	template <size_type situation>
	inline constexpr size_type find_common(const_pointer s, size_type pos, size_type count) const noexcept
	{
		static_assert(situation < 2);
		size_type thisn{this->n};
		if (count == 0)
		{
			return pos <= thisn ? pos : ::fast_io::containers::npos;
		}
		if (thisn <= pos)
		{
			return ::fast_io::containers::npos;
		}
		auto bg{this->ptr};
		auto start{bg + pos};
		auto ed{bg + thisn};
		auto sed{s + count};
		const_pointer it;
		if constexpr (situation == 0)
		{
			it = ::std::search(start, ed, s, sed);
		}
		else if constexpr (situation == 1)
		{
			it = ::std::find_end(start, ed, s, sed);
		}
		if (it == ed)
		{
			return ::fast_io::containers::npos;
		}
		return static_cast<size_type>(it - bg);
	}
	template <size_type situation>
	inline constexpr size_type find_first_last_common(const_pointer s, size_type pos, size_type count) const noexcept
	{
		static_assert(situation < 4);
		auto bg{this->ptr};
		auto start{bg + pos};
		auto ed{bg + this->n};
		auto sed{s + count};
		const_pointer it;
		if constexpr (situation == 0)
		{
			it = ::std::find_first_of(start, ed, s, sed);
		}
		else if constexpr (situation == 1)
		{
			it = ::fast_io::freestanding::find_first_not_of(start, ed, s, sed);
		}
		else if constexpr (situation == 2)
		{
			it = ::fast_io::freestanding::find_last_of(start, ed, s, sed);
		}
		else if constexpr (situation == 3)
		{
			it = ::fast_io::freestanding::find_last_not_of(start, ed, s, sed);
		}
		if (it == ed)
		{
			return ::fast_io::containers::npos;
		}
		return static_cast<size_type>(it - bg);
	}

public:
	inline constexpr size_type find_character(char_type ch, size_type pos = 0) const noexcept
	{
		return this->find_character_common<0>(ch, pos);
	}
	inline constexpr size_type find_not_character(char_type ch, size_type pos = 0) const noexcept
	{
		return this->find_character_common<1>(ch, pos);
	}
	inline constexpr size_type rfind_character(char_type ch, size_type pos = 0) const noexcept
	{
		return this->find_character_common<2>(ch, pos);
	}
	inline constexpr size_type rfind_not_character(char_type ch, size_type pos = 0) const noexcept
	{
		return this->find_character_common<3>(ch, pos);
	}
	inline constexpr size_type find(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return this->find_common<0>(s, pos, count);
	}
	inline constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept
	{
		return this->find(v.ptr, pos, v.n);
	}
	inline constexpr size_type rfind(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return this->find_common<1>(s, pos, count);
	}
	inline constexpr size_type rfind(basic_string_view v, size_type pos = 0) const noexcept
	{
		return this->rfind(v.ptr, pos, v.n);
	}
	inline constexpr size_type find_first_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return this->find_first_last_common<0>(s, pos, count);
	}
	inline constexpr size_type find_first_of(basic_string_view s, size_type pos) const noexcept
	{
		return this->find_first_of(s.ptr, pos, s.n);
	}
	inline constexpr size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return this->find_first_last_common<1>(s, pos, count);
	}
	inline constexpr size_type find_first_not_of(basic_string_view s, size_type pos) const noexcept
	{
		return this->find_first_not_of(s.ptr, pos, s.n);
	}
	inline constexpr size_type find_last_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return this->find_first_last_common<2>(s, pos, count);
	}
	inline constexpr size_type find_last_of(basic_string_view s, size_type pos) const noexcept
	{
		return this->find_last_of(s.ptr, pos, s.n);
	}
	inline constexpr size_type find_last_not_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return this->find_first_last_common<3>(s, pos, count);
	}
	inline constexpr size_type find_last_not_of(basic_string_view s, size_type pos) const noexcept
	{
		return this->find_last_not_of(s.ptr, pos, s.n);
	}
	inline constexpr auto compare_three_way(size_type pos1, size_type count1, const_pointer s, size_type count2) const noexcept
	{
		return this->subview(pos1, count1) <=> basic_string_view(s, count2);
	}
	inline constexpr auto compare_three_way_unchecked(size_type pos1, size_type count1, const_pointer s, size_type count2) const noexcept
	{
		return this->subview_unchecked(pos1, count1) <=> basic_string_view(s, count2);
	}
	inline constexpr auto compare_three_way(size_type pos1, size_type count1, basic_string_view other, size_type pos2, size_type count2) const noexcept
	{
		return this->subview(pos1, count1) <=> other.subview(pos2, count2);
	}
	inline constexpr auto compare_three_way_unchecked(size_type pos1, size_type count1, basic_string_view other, size_type pos2, size_type count2) const noexcept
	{
		return this->subview_unchecked(pos1, count1) <=> other.subview_unchecked(pos2, count2);
	}
	inline constexpr void clear() noexcept
	{
		this->ptr = 0;
		this->n = 0;
	}
	template <typename charcate>
		requires requires(charcate traits, const_pointer beginptr, const_pointer currptr) {
			{ traits.find_not(beginptr, currptr) } -> ::std::convertible_to<const_pointer>;
			{ traits.find_last_not(beginptr, currptr) } -> ::std::convertible_to<const_pointer>;
		}
	inline constexpr basic_string_view trim_subview(charcate traits) const noexcept
	{
		auto beginptr{this->ptr};
		auto currptr{this->ptr + this->n};
		auto frontit{traits.find_not(beginptr, currptr)};
		auto backit{traits.find_last_not(frontit, currptr)};
		return basic_string_view(frontit, static_cast<size_type>(backit - frontit));
	}
	inline constexpr basic_string_view trim_subview_c_space() noexcept
	{
		return trim_subview(::fast_io::char_category::c_space{});
	}
	template <typename charcate>
		requires requires(charcate traits, const_pointer beginptr, const_pointer currptr) {
			{ traits.find_not(beginptr, currptr) } -> ::std::convertible_to<const_pointer>;
		}
	inline constexpr basic_string_view trim_prefix_subview(charcate traits) const noexcept
	{
		auto beginptr{this->ptr};
		auto currptr{this->ptr + this->n};
		auto frontit{traits.find_not(beginptr, currptr)};
		return basic_string_view(frontit, static_cast<size_type>(currptr - frontit));
	}
	inline constexpr basic_string_view trim_prefix_subview_c_space() const noexcept
	{
		return trim_prefix_subview(::fast_io::char_category::c_space{});
	}

	template <typename charcate>
		requires requires(charcate traits, const_pointer beginptr, const_pointer currptr) {
			{ traits.find_last_not(beginptr, currptr) } -> ::std::convertible_to<const_pointer>;
		}
	inline constexpr basic_string_view trim_suffix_subview(charcate traits) const noexcept
	{
		auto beginptr{this->ptr};
		auto currptr{this->ptr + this->n};
		auto backit{traits.find_last_not(beginptr, currptr)};
		return basic_string_view(beginptr, static_cast<size_type>(backit - beginptr));
	}
	inline constexpr basic_string_view trim_suffix_subview_c_space() const noexcept
	{
		return trim_suffix_subview(::fast_io::char_category::c_space{});
	}
};

template <::std::integral char_type>
inline constexpr void swap(::fast_io::containers::basic_string_view<char_type> &a, ::fast_io::containers::basic_string_view<char_type> &b) noexcept
{
	a.swap(b);
}

template <::std::integral char_type>
inline constexpr ::fast_io::basic_io_scatter_t<char_type> print_alias_define(::fast_io::io_alias_t, ::fast_io::containers::basic_string_view<char_type> str) noexcept
{
	return {str.ptr, str.n};
}

template <::std::integral char_type>
inline constexpr bool operator==(::fast_io::containers::basic_string_view<char_type> a, ::fast_io::containers::basic_string_view<char_type> b) noexcept
{
	return ::std::equal(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n);
}

template <::std::integral char_type, ::std::size_t n>
inline constexpr bool operator==(::fast_io::containers::basic_string_view<char_type> a, char_type const (&buffer)[n]) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::equal(a.ptr, a.ptr + a.n, buffer, buffer + nm1);
}

template <::std::integral char_type, ::std::size_t n>
inline constexpr bool operator==(char_type const (&buffer)[n], ::fast_io::containers::basic_string_view<char_type> a) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::equal(buffer, buffer + nm1, a.ptr, a.ptr + a.n);
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral char_type>
inline constexpr auto operator<=>(::fast_io::containers::basic_string_view<char_type> a, ::fast_io::containers::basic_string_view<char_type> b) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n, ::std::compare_three_way{});
}

template <::std::integral char_type, ::std::size_t n>
inline constexpr auto operator<=>(::fast_io::containers::basic_string_view<char_type> a, char_type const (&buffer)[n]) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, buffer, buffer + nm1, ::std::compare_three_way{});
}

template <::std::integral char_type, ::std::size_t n>
inline constexpr auto operator<=>(char_type const (&buffer)[n], ::fast_io::containers::basic_string_view<char_type> a) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::fast_io::freestanding::lexicographical_compare_three_way(buffer, buffer + nm1, a.ptr, a.ptr + a.n, ::std::compare_three_way{});
}

#endif

} // namespace fast_io::containers
