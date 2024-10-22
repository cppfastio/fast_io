#pragma once

namespace fast_io::containers
{

namespace details
{

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	string_model
{
	void *begin_ptr;
	void *curr_ptr;
	void *end_ptr;
};

template <typename T>
struct string_internal
{
	T *begin_ptr;
	T *curr_ptr;
	T *end_ptr;
};

template <typename allocator_type, ::std::integral chtype>
inline constexpr ::fast_io::basic_allocation_least_result<chtype *> string_allocate_init(chtype const *first, ::std::size_t n) noexcept
{
	using typed_allocator_type = typed_generic_allocator_adapter<allocator_type, chtype>;
	::std::size_t np1{n + 1};
	auto res{typed_allocator_type::allocate_at_least(np1)};
	*::fast_io::freestanding::non_overlapped_copy_n(first, n, res.ptr) = 0;
	return res;
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void string_heap_dilate_uncheck(::fast_io::containers::details::string_internal<chtype> &imp, ::std::size_t rsize, chtype *pnullterminate) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;

	::std::size_t const bfsize{static_cast<::std::size_t>(imp.end_ptr - imp.begin_ptr)};
	::std::size_t const strsize{static_cast<::std::size_t>(imp.curr_ptr - imp.begin_ptr)};

	chtype *ptr;
	auto beginptr{imp.begin_ptr};
	bool const is_sso{beginptr == pnullterminate};
	if (beginptr == pnullterminate)
	{
		beginptr = nullptr;
	}
	if constexpr (typed_allocator_type::has_reallocate)
	{
		auto [newptr, newcap] = typed_allocator_type::reallocate_at_least(beginptr, rsize + 1u);
		ptr = newptr;
		rsize = newcap - 1u;
	}
	else
	{
		auto [newptr, newcap] = typed_allocator_type::reallocate_n_at_least(beginptr, bfsize, rsize + 1u);
		ptr = newptr;
		rsize = newcap - 1u;
	}
	if (is_sso)
	{
		*ptr = 0;
	}
	imp = {ptr, ptr + strsize, ptr + rsize};
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void string_push_back_heap_grow_twice(::fast_io::containers::details::string_internal<chtype> &imp, chtype *pnullterminate) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;

	::std::size_t const bfsize{static_cast<::std::size_t>(imp.end_ptr - imp.begin_ptr)};
	::std::size_t const bfsizep1{bfsize + 1u};

	constexpr ::std::size_t mxsz{SIZE_MAX / sizeof(chtype) / 2u};
	if (mxsz < bfsizep1) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
	::std::size_t const bfsizep1mul2{bfsizep1 << 1u};
	return ::fast_io::containers::details::string_heap_dilate_uncheck<allocator_type>(imp, bfsizep1mul2, pnullterminate);
}

} // namespace details

template <::std::integral chtype, typename alloctype>
class
#if __has_cpp_attribute(clang::trivial_abi)
	[[clang::trivial_abi]]
#endif
	basic_string
{
public:
	using allocator_type = alloctype;
	using char_type = chtype;
	using value_type = char_type;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using pointer = char_type *;
	using const_pointer = char_type const *;
	using reference = char_type &;
	using const_reference = char_type const &;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

	::fast_io::containers::details::string_internal<char_type> imp;
	char_type nullterminator;

	constexpr basic_string() noexcept
		: imp{__builtin_addressof(this->nullterminator),
			  __builtin_addressof(this->nullterminator),
			  __builtin_addressof(this->nullterminator)},
		  nullterminator{}
	{
	}

	explicit constexpr basic_string(size_type n) noexcept
	{
		if (!n)
		{
			this->imp = {__builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator), __builtin_addressof(this->nullterminator)};
			this->nullterminator = 0;
		}
		else
		{
			using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
			using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
			auto [ptr, newcap]{typed_allocator_type::allocate_zero_at_least(n + 1u)};
			this->imp = {ptr, ptr, ptr + newcap};
		}
	}

	template <::std::size_t n>
	explicit constexpr basic_string(char_type const (&buffer)[n]) noexcept
	{
		constexpr ::std::size_t nm1{n - 1u};
		if constexpr (!nm1)
		{
			this->imp = {__builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator), __builtin_addressof(this->nullterminator)};
			this->nullterminator = 0;
		}
		else
		{
			static_assert(n != SIZE_MAX);
			auto newres{::fast_io::containers::details::string_allocate_init<allocator_type>(buffer, nm1)};
			auto ptrn{newres.ptr + nm1};
			this->imp = {newres.ptr, ptrn, newres.ptr + newres.count};
		}
	}

	explicit constexpr basic_string(size_type n, char_type ch) noexcept
	{
		if (!n)
		{
			this->imp = {__builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator), __builtin_addressof(this->nullterminator)};
			this->nullterminator = 0;
		}
		else
		{

			using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
			using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
			auto [ptr, cap]{typed_allocator_type::allocate_at_least(n + 1u)};
			this->imp = {ptr, ptr + n, ptr + cap};
			*::fast_io::freestanding::uninitialized_fill(ptr, ptr + n, ch) = 0;
		}
	}

	explicit constexpr basic_string(char_type const *b, char_type const *e) noexcept
	{
		size_type const size{static_cast<size_type>(e - b)};
		if (!size)
		{
			this->imp = {__builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator), __builtin_addressof(this->nullterminator)};
			this->nullterminator = 0;
		}
		else
		{
			using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
			using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
			auto [ptr, cap]{typed_allocator_type::allocate_at_least(size + 1u)};
			this->imp = {ptr, ptr + size, ptr + cap};
			::fast_io::freestanding::non_overlapped_copy_n(b, size, ptr);
			ptr[size] = 0;
		}
	}

	constexpr const_pointer c_str() const noexcept
	{
		return imp.begin_ptr;
	}

	constexpr pointer data() noexcept
	{
		return imp.begin_ptr;
	}

	constexpr const_pointer data() const noexcept
	{
		return imp.begin_ptr;
	}

	constexpr bool is_empty() const noexcept
	{
		return imp.begin_ptr == imp.curr_ptr;
	}

	constexpr bool empty() const noexcept
	{
		return imp.begin_ptr == imp.curr_ptr;
	}

	constexpr size_type size() const noexcept
	{
		return static_cast<size_type>(imp.curr_ptr - imp.begin_ptr);
	}
	constexpr size_type size_bytes() const noexcept
	{
		return static_cast<size_type>(imp.curr_ptr - imp.begin_ptr) * sizeof(value_type);
	}

	constexpr size_type capacity() const noexcept
	{
		return static_cast<size_type>(imp.end_ptr - imp.begin_ptr);
	}

	constexpr size_type capacity_bytes() const noexcept
	{
		return static_cast<size_type>(imp.end_ptr - imp.begin_ptr) * sizeof(value_type);
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type n{SIZE_MAX / sizeof(value_type)};
		return n;
	}
	static inline constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type n{SIZE_MAX / sizeof(value_type) * sizeof(value_type)};
		return n;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr reference back() noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return curr_ptr[-1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr const_reference back() const noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return curr_ptr[-1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr reference front() noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *begin_ptr;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr const_reference front() const noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *begin_ptr;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr reference back_unchecked() noexcept
	{
		return imp.curr_ptr[-1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr const_reference back_unchecked() const noexcept
	{
		return imp.curr_ptr[-1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr reference front_unchecked() noexcept
	{
		return *imp.begin_ptr;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr const_reference front_unchecked() const noexcept
	{
		return *imp.begin_ptr;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr const_reference operator[](size_type pos) const noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (static_cast<size_type>(curr_ptr - begin_ptr) <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr reference operator[](size_type pos) noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (static_cast<size_type>(curr_ptr - begin_ptr) <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr const_reference index_unchecked(size_type pos) const noexcept
	{
		return imp.begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr reference index_unchecked(size_type pos) noexcept
	{
		return imp.begin_ptr[pos];
	}

private:
	constexpr void moveconstructorcommon(basic_string &&other) noexcept
	{
		if (other.imp.begin_ptr == __builtin_addressof(other.nullterminator))
		{
			this->imp = {__builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator)};
			this->nullterminator = 0;
		}
		else
		{
			this->imp = other.imp;
			other.imp = {__builtin_addressof(other.nullterminator),
						 __builtin_addressof(other.nullterminator),
						 __builtin_addressof(other.nullterminator)};
			other.nullterminator = 0;
		}
	}

public:
	constexpr basic_string(basic_string &&other) noexcept
	{
		this->moveconstructorcommon(::std::move(other));
	}

	constexpr basic_string &operator=(basic_string &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		this->destroy();
		this->moveconstructorcommon(::std::move(other));
		return *this;
	}

	constexpr basic_string(basic_string const &) noexcept = delete;
	constexpr basic_string &operator=(basic_string const &) noexcept = delete;

	constexpr void clear() noexcept
	{
		*(this->imp.curr_ptr = this->imp.begin_ptr) = 0;
	}

	constexpr void clear_destroy() noexcept
	{
		this->destroy();
		this->imp = {__builtin_addressof(this->nullterminator),
					 __builtin_addressof(this->nullterminator),
					 __builtin_addressof(this->nullterminator)};
		this->nullterminator = 0;
	}

	constexpr void push_back_unchecked(char_type ch) noexcept
	{
		*this->imp.curr_ptr = ch;
		*++this->imp.curr_ptr = 0;
	}

private:
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	constexpr void grow_twice() noexcept
	{
		::fast_io::containers::details::string_push_back_heap_grow_twice<allocator_type>(this->imp, __builtin_addressof(this->nullterminator));
	}

public:
	constexpr const_iterator cbegin() const noexcept
	{
		return this->imp.begin_ptr;
	}

	constexpr const_iterator cend() const noexcept
	{
		return this->imp.curr_ptr;
	}

	constexpr const_iterator begin() const noexcept
	{
		return this->imp.begin_ptr;
	}

	constexpr const_iterator end() const noexcept
	{
		return this->imp.curr_ptr;
	}

	constexpr iterator begin() noexcept
	{
		return this->imp.begin_ptr;
	}

	constexpr iterator end() noexcept
	{
		return this->imp.curr_ptr;
	}

	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(this->imp.curr_ptr);
	}

	constexpr const_iterator crend() const noexcept
	{
		return const_reverse_iterator(this->imp.begin_ptr);
	}

	constexpr const_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(this->imp.curr_ptr);
	}

	constexpr const_iterator rend() const noexcept
	{
		return const_reverse_iterator(this->imp.begin_ptr);
	}

	constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(this->imp.curr_ptr);
	}

	constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator(this->imp.begin_ptr);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr void push_back(char_type ch) noexcept
	{
		if (this->imp.curr_ptr == this->imp.end_ptr) [[unlikely]]
		{
			this->grow_twice();
		}
		*this->imp.curr_ptr = ch;
		*++this->imp.curr_ptr = 0;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr void pop_back() noexcept
	{
		if (this->imp.curr_ptr == this->imp.begin_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		*--this->imp.curr_ptr = 0;
	}

	constexpr void pop_back_unchecked() noexcept
	{
		*--this->imp.curr_ptr = 0;
	}

	constexpr void reserve(size_type new_cap) noexcept
	{
		auto begin_ptr{this->imp.begin_ptr};
		if (new_cap <= static_cast<size_type>(imp.end_ptr - begin_ptr))
		{
			return;
		}
		::fast_io::containers::details::string_heap_dilate_uncheck<allocator_type>(this->imp, new_cap, __builtin_addressof(this->nullterminator));
	}

#if 0
	constexpr void shrink_to_fit() noexcept
	{
		if(this->imp.curr_ptr==this->imp.end_ptr)
		{
			return;
		}
	}
#endif

private:
	constexpr void destroy() noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		if (beginptr != __builtin_addressof(this->nullterminator))
		{
			using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
			using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
			typed_allocator_type::deallocate_n(beginptr, static_cast<size_type>(static_cast<size_type>(this->imp.end_ptr - beginptr) + 1u));
		}
	}

public:
	constexpr ~basic_string()
	{
		this->destroy();
	}
};

#if 0
template <::std::integral chtype, typename allocator1>
inline constexpr basic_string<chtype, allocator1> * uninitialized_relocate_define(basic_string<chtype, allocator1> *first, basic_string<chtype, allocator1> *last, basic_string<chtype, allocator1> *dest)
{
	::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const*>(first),
		reinterpret_cast<::std::byte const*>(last),
		reinterpret_cast<::std::byte *>(dest));
	for(auto it{first};it!=last;++it)
	{
		if(it->imp.begin_ptr==__builtin_addressof(it->nullterminator))
		{
			dest->imp = {__builtin_addressof(dest->nullterminator),
				__builtin_addressof(dest->nullterminator),
				__builtin_addressof(dest->nullterminator)};
		}
		++dest;
	}
	return dest;
}
#endif
template <::std::integral chtype, typename allocator1, typename allocator2>
constexpr bool operator==(basic_string<chtype, allocator1> const &lhs, basic_string<chtype, allocator2> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

#if __cpp_lib_three_way_comparison >= 201907L

template <::std::integral chtype, typename allocator1, typename allocator2>
constexpr auto operator<=>(basic_string<chtype, allocator1> const &lhs, basic_string<chtype, allocator2> const &rhs) noexcept
{
	return ::std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), ::std::compare_three_way{});
}

#endif

template <::std::integral chtype, typename alloctype>
inline constexpr ::fast_io::basic_io_scatter_t<chtype>
print_alias_define(io_alias_t, basic_string<chtype, alloctype> const &str) noexcept
{
	return {str.imp.begin_ptr, static_cast<::std::size_t>(str.imp.curr_ptr - str.imp.begin_ptr)};
}

template <::std::integral char_type, typename allocator_type>
inline constexpr auto
strlike_construct_define(io_strlike_type_t<char_type, basic_string<char_type, allocator_type>>,
						 char_type const *first, char_type const *last)
{
	return basic_string<char_type, allocator_type>(first, last);
}

template <::std::integral char_type, typename allocator_type>
inline constexpr auto strlike_construct_single_character_define(
	io_strlike_type_t<char_type, basic_string<char_type, allocator_type>>, char_type ch)
{
	return basic_string<char_type, allocator_type>(1, ch);
}

template <::std::integral chtype, typename alloctype>
inline constexpr chtype *strlike_begin(::fast_io::io_strlike_type_t<chtype, basic_string<chtype, alloctype>>, basic_string<chtype, alloctype> &str) noexcept
{
	return str.imp.begin_ptr;
}

template <::std::integral chtype, typename alloctype>
inline constexpr chtype *strlike_curr(::fast_io::io_strlike_type_t<chtype, basic_string<chtype, alloctype>>, basic_string<chtype, alloctype> &str) noexcept
{
	return str.imp.curr_ptr;
}

template <::std::integral chtype, typename alloctype>
inline constexpr chtype *strlike_end(::fast_io::io_strlike_type_t<chtype, basic_string<chtype, alloctype>>, basic_string<chtype, alloctype> &str) noexcept
{
	return str.imp.end_ptr;
}

template <::std::integral chtype, typename alloctype>
inline constexpr void strlike_set_curr(::fast_io::io_strlike_type_t<chtype, basic_string<chtype, alloctype>>, basic_string<chtype, alloctype> &str, chtype *p) noexcept
{
	*(str.imp.curr_ptr = p) = 0;
}

template <::std::integral chtype, typename alloctype>
inline constexpr void strlike_reserve(::fast_io::io_strlike_type_t<chtype, basic_string<chtype, alloctype>>, basic_string<chtype, alloctype> &str, ::std::size_t n) noexcept
{
	str.reserve(n);
}

template <::std::integral chtype, typename alloctype>
inline constexpr ::fast_io::io_strlike_reference_wrapper<chtype, basic_string<chtype, alloctype>> io_strlike_ref(::fast_io::io_alias_t, basic_string<chtype, alloctype> &str) noexcept
{
	return {__builtin_addressof(str)};
}

} // namespace fast_io::containers
