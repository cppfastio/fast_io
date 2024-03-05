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

template <typename T>
inline constexpr ::std::size_t string_sso_size_val{40 * sizeof(T) + sizeof(::fast_io::containers::details::string_internal<T>)};

template <typename T>
inline constexpr ::std::size_t string_sso_size{((string_sso_size_val<T> % 16 == 0 ? (string_sso_size_val<T> / 16) : (string_sso_size_val<T> / 16 + 1)) * 16 - sizeof(::fast_io::containers::details::string_internal<T>)) / sizeof(T)};

template <typename T>
inline constexpr ::std::size_t string_sso_sizem1{::fast_io::containers::details::string_sso_size<T> - 1u};

template <typename T, ::std::size_t n>
struct string_sso_buffer
{
	T buffer[n];
};

template <typename allocator_type, ::std::integral chtype>
inline constexpr chtype *string_allocate_init(chtype const *first, ::std::size_t n) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<allocator_type, chtype>;
	::std::size_t np1{n + sizeof(chtype)};
	auto ptr{typed_allocator_type::allocate(np1)};
	*::fast_io::details::non_overlapped_copy_n(first, n, ptr) = 0;
	return ptr;
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void string_push_back_heap_grow_twice(::fast_io::containers::details::string_internal<chtype> &imp) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;

	::std::size_t const bfsize{static_cast<::std::size_t>(imp.end_ptr - imp.begin_ptr)};
	::std::size_t const strsize{static_cast<::std::size_t>(imp.curr_ptr - imp.begin_ptr)};
	::std::size_t const bfsizep1{bfsize + 1u};

	constexpr ::std::size_t mxsz{SIZE_MAX / sizeof(chtype) / 2u};
	if (mxsz < bfsizep1) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
	::std::size_t const bfsizep1mul2{bfsizep1 << 1u};
	chtype *ptr;
	if constexpr (typed_allocator_type::has_reallocate)
	{
		ptr = typed_allocator_type::reallocate(imp.begin_ptr, bfsizep1mul2);
	}
	else
	{
		ptr = typed_allocator_type::reallocate_n(imp.begin_ptr, bfsizep1, bfsizep1mul2);
	}
	imp = {ptr, ptr + strsize, ptr + (bfsizep1mul2 - 1u)};
}

template <typename allocator_type>
inline void string_push_back_stack_to_heap_grow_twice_common(::fast_io::containers::details::string_model *imp, void const *first,
												   ::std::size_t ssobytes, ::std::size_t chsz) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	::std::size_t const twobytes{ssobytes << 1u};
	void *ptr{untyped_allocator_type::allocate(twobytes)};
	::fast_io::details::my_memcpy(ptr, first, ssobytes);
	*imp = {ptr, static_cast<::std::byte *>(ptr) + (ssobytes - chsz),
			static_cast<::std::byte *>(ptr) + (twobytes - chsz)};
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void string_push_back_stack_to_heap_grow_twice(::fast_io::containers::details::string_internal<chtype> &imp, chtype const *first) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
	constexpr ::std::size_t ssosize{::fast_io::containers::details::string_sso_size<chtype>};

#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		constexpr ::std::size_t twosize{ssosize * 2u};
		auto ptr{typed_allocator_type::allocate(twosize)};
		::fast_io::details::non_overlapped_copy_n(first, ssosize, ptr);
		imp = {ptr, ptr + (ssosize - 1u), ptr + (twosize - 1u)};
	}
	else
	{
		constexpr ::std::size_t ssobytes{sizeof(chtype) * ssosize};
		::fast_io::containers::details::string_push_back_stack_to_heap_grow_twice_common<allocator_type>(reinterpret_cast<::fast_io::containers::details::string_model *>(__builtin_addressof(imp)), first, ssobytes, sizeof(chtype));
	}
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void string_heap_dilate_uncheck(::fast_io::containers::details::string_internal<chtype> &imp, ::std::size_t rsize) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;

	::std::size_t const bfsize{static_cast<::std::size_t>(imp.end_ptr - imp.begin_ptr)};
	::std::size_t const strsize{static_cast<::std::size_t>(imp.curr_ptr - imp.begin_ptr)};

	chtype *ptr;
	if constexpr (typed_allocator_type::has_reallocate)
	{
		ptr = typed_allocator_type::reallocate(imp.begin_ptr, rsize + 1u);
	}
	else
	{
		ptr = typed_allocator_type::reallocate_n(imp.begin_ptr, bfsize, rsize + 1u);
	}
	imp = {ptr, ptr + strsize, ptr + rsize};
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void string_stack_to_heap_dilate_uncheck(::fast_io::containers::details::string_internal<chtype> &imp, chtype const *first, ::std::size_t rsize) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
	constexpr ::std::size_t ssosize{::fast_io::containers::details::string_sso_size<chtype>};
	constexpr auto chsz{sizeof(chtype)};
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		auto const strsize{static_cast<::std::size_t>(imp.curr_ptr - imp.begin_ptr)};
		auto ptr{typed_allocator_type::allocate(rsize + 1u)};
		::fast_io::details::non_overlapped_copy_n(first, ssosize, ptr);
		imp = {ptr, ptr + strsize, ptr + rsize};
	}
	else
	{
		auto ptr{typed_allocator_type::allocate(rsize + 1)};
		::fast_io::details::my_memcpy(ptr, first, ssosize);
		auto const strsize{static_cast<::std::size_t>(imp.curr_ptr - imp.begin_ptr)};
		imp = {ptr, ptr + strsize, ptr + rsize};
	}
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
	::fast_io::containers::details::string_sso_buffer<char_type, ::fast_io::containers::details::string_sso_size<char_type>> ssobuffer;

	constexpr basic_string() noexcept
		: imp{ssobuffer.buffer, ssobuffer.buffer, ssobuffer.buffer + ::fast_io::containers::details::string_sso_sizem1<char_type>}
	{
		*ssobuffer.buffer = 0;
	}

#if 0
	explicit constexpr basic_string(size_type n) noexcept
	{
	}

	explicit constexpr basic_string(size_type n, char_type ch) noexcept
	{		
	}
#endif

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

	constexpr size_type capacity() const noexcept
	{
		return static_cast<size_type>(imp.end_ptr - imp.begin_ptr);
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type n{SIZE_MAX / sizeof(value_type)};
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

	template <::std::size_t n>
	explicit constexpr basic_string(char_type const (&buffer)[n]) noexcept
	{
		constexpr ::std::size_t nm1{n - 1u};
		if constexpr (nm1 < (::fast_io::containers::details::string_sso_size<char_type>))
		{
			*::fast_io::details::non_overlapped_copy_n(buffer, nm1, ssobuffer.buffer) = 0;
			this->imp = {ssobuffer.buffer, ssobuffer.buffer + nm1, ssobuffer.buffer + ::fast_io::containers::details::string_sso_sizem1<char_type>};
		}
		else
		{
			static_assert(n != SIZE_MAX);
			auto ptr{::fast_io::containers::details::string_allocate_init<allocator_type>(buffer, nm1)};
			auto ptrn{ptr + nm1};
			this->imp = {ptr, ptrn, ptrn};
		}
	}

private:
	constexpr void moveconstructorcommon(basic_string &&other) noexcept
	{
		if (other.imp.begin_ptr == other.ssobuffer.buffer)
		{
			this->ssobuffer = other.ssobuffer;
			this->imp.begin_ptr = this->ssobuffer.buffer;
			this->imp.curr_ptr = this->ssobuffer.buffer + (other.imp.curr_ptr - other.imp.begin_ptr);
			this->imp.end_ptr = this->ssobuffer.buffer + ::fast_io::containers::details::string_sso_sizem1<char_type>;
			other.imp.curr_ptr = other.ssobuffer.buffer;
		}
		else
		{
			this->imp = other.imp;
			other.imp.end_ptr = (other.imp.curr_ptr = other.imp.begin_ptr = other.ssobuffer.buffer) + ::fast_io::containers::details::string_sso_sizem1<char_type>;
		}
		*other.ssobuffer.buffer = 0;
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
		if (this->imp.begin_ptr == ssobuffer.buffer) [[unlikely]]
		{
			::fast_io::containers::details::string_push_back_stack_to_heap_grow_twice<allocator_type>(this->imp, this->ssobuffer.buffer);
		}
		else [[likely]]
		{
			::fast_io::containers::details::string_push_back_heap_grow_twice<allocator_type>(this->imp);
		}
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

	constexpr void push_back(char_type ch) noexcept
	{
		if (this->imp.curr_ptr == this->imp.end_ptr) [[unlikely]]
		{
			this->grow_twice();
		}
		*this->imp.curr_ptr = ch;
		*++this->imp.curr_ptr = 0;
	}

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
		if (begin_ptr == ssobuffer.buffer)
		{
			::fast_io::containers::details::string_stack_to_heap_dilate_uncheck<allocator_type>(this->imp, this->ssobuffer.buffer, new_cap);
		}
		else
		{
			::fast_io::containers::details::string_heap_dilate_uncheck<allocator_type>(this->imp, new_cap);
		}
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
		if (beginptr != ssobuffer.buffer)
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

template <::std::integral chtype, typename alloctype>
inline constexpr ::fast_io::basic_io_scatter_t<chtype>
print_alias_define(io_alias_t, basic_string<chtype, alloctype> const &str) noexcept
{
	return {str.imp.begin_ptr, static_cast<::std::size_t>(str.imp.curr_ptr - str.imp.begin_ptr)};
}

#if 0

template <::std::integral char_type, typename allocator_type>
inline constexpr auto
strlike_construct_define(io_strlike_type_t<char_type, ::fast_io::basic_string<char_type, allocator_type>>,
						 char_type const *first, char_type const *last)
{
	return ::fast_io::basic_string<char_type, allocator_type>(first, last);
}

template <::std::integral char_type, typename allocator_type>
inline constexpr auto strlike_construct_single_character_define(
	io_strlike_type_t<char_type, ::fast_io::basic_string<char_type, allocator_type>>, char_type ch)
{
	return ::fast_io::basic_string<char_type, allocator_type>(1, ch);
}

#endif

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
inline constexpr ::std::size_t strlike_sso_size(::fast_io::io_strlike_type_t<chtype, basic_string<chtype, alloctype>>) noexcept
{
	return ::fast_io::containers::details::string_sso_sizem1<chtype>;
}

template <::std::integral chtype, typename alloctype>
inline constexpr ::fast_io::io_strlike_reference_wrapper<chtype, basic_string<chtype, alloctype>> io_strlike_ref(::fast_io::io_alias_t, basic_string<chtype, alloctype> &str) noexcept
{
	return {__builtin_addressof(str)};
}

} // namespace fast_io::containers
