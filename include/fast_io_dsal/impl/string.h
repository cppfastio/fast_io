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
inline constexpr ::std::size_t string_sso_size_val{(64 - sizeof(::fast_io::containers::details::string_internal<T>)) / sizeof(T)};

template <typename T>
inline constexpr ::std::size_t string_sso_size{::fast_io::containers::details::string_sso_size_val<T>};

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
	::std::size_t np1{n + 1u};
	auto ptr{typed_allocator_type::allocate(np1)};
	*::fast_io::details::non_overlapped_copy_n(first, n, ptr) = 0;
	return ptr;
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void string_heap_grow_twice(::fast_io::containers::details::string_internal<chtype> &imp) noexcept
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
	auto ptr{typed_allocator_type::reallocate_n(imp.begin_ptr, bfsizep1, bfsizep1mul2)};
	;
	imp = {ptr, ptr + bfsize, ptr + bfsizep1mul2};
}

template <typename allocator_type>
inline void string_stack_to_heap_grow_twice_common(::fast_io::containers::details::string_model *imp, void const *first,
												   ::std::size_t ssobytes, ::std::size_t sz) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	::std::size_t const twobytes{ssobytes << 1u};
	::std::size_t const twobytesm1{twobytes - sz};
	void *ptr{untyped_allocator_type::allocate(twobytes)};
	::fast_io::details::my_memcpy(ptr, first, ssobytes);
	*imp = {ptr, static_cast<::std::byte *>(ptr) + static_cast<::std::size_t>(twobytes - sz),
			static_cast<::std::byte *>(ptr) + static_cast<::std::size_t>(twobytes - sz)};
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void string_stack_to_heap_grow_twice(::fast_io::containers::details::string_internal<chtype> &imp, chtype const *first) noexcept
{
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
	constexpr ::std::size_t ssosize{::fast_io::containers::details::string_sso_size<chtype>};
	if consteval
	{
		constexpr ::std::size_t twosize{ssosize * 2u};
		constexpr ::std::size_t twosizem1{twosize - 1u};
		auto ptr{typed_allocator_type::allocate(twosize)};
		auto ptrcurr{::fast_io::details::non_overlapped_copy_n(first, ssosize, ptr)};
		imp = {ptr, ptrcurr - 1, ptr + twosizem1};
	}
	else
	{
		constexpr ::std::size_t ssobytes{sizeof(chtype) * ssosize};
		::fast_io::containers::details::string_stack_to_heap_grow_twice_common(__builtin_addressof(imp), first, ssobytes, sizeof(chtype));
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
		: imp{ssobuffer.buffer, ssobuffer.buffer, ssobuffer.buffer + ::fast_io::containers::details::string_sso_size<char_type> - 1}
	{
		*ssobuffer.buffer = 0;
	}

#if 0
	explicit constexpr basic_string(size_type n, ::fast_io::for_overwrite_t) noexcept
	{

	}

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
		if (this->imp.begin_ptr == ssobuffer.buffer)
		{
			::fast_io::containers::details::string_stack_to_heap_grow_twice<allocator_type>(this->imp);
		}
		else
		{
			::fast_io::containers::details::string_heap_grow_twice<allocator_type>(this->imp);
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
		*this->imp.curr_ptr = 0;
		--this->imp.curr_ptr;
	}

	constexpr void pop_back_unchecked() noexcept
	{
		*this->imp.curr_ptr = 0;
		--this->imp.curr_ptr;
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

	constexpr ~basic_string()
	{
		auto beginptr{this->imp.begin_ptr};
		if (beginptr != ssobuffer.buffer)
		{
			using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
			using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
			typed_allocator_type::deallocate_n(beginptr, static_cast<size_type>(static_cast<size_type>(this->imp.end_ptr - beginptr) + 1u));
		}
	}
};

} // namespace fast_io::containers
