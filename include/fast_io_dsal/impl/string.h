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
	// n is not possible to SIZE_MAX since that would overflow the memory which is not possible
	::std::size_t np1{n + 1};
	auto [ptr, allocn]{typed_allocator_type::allocate_at_least(np1)};
	*::fast_io::freestanding::non_overlapped_copy_n(first, n, ptr) = 0;
	return {ptr, static_cast<::std::size_t>(allocn - 1u)};
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
			constexpr size_type mx{::std::numeric_limits<size_type>::max()};
			if (n == mx)
			{
				::fast_io::fast_terminate();
			}
			auto [ptr, newcap]{typed_allocator_type::allocate_zero_at_least(n + 1u)};
			this->imp = {ptr, ptr, ptr + static_cast<size_type>(newcap - 1u)};
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
			constexpr size_type mx{::std::numeric_limits<size_type>::max()};
			if (n == mx)
			{
				::fast_io::fast_terminate();
			}
			auto [ptr, cap]{typed_allocator_type::allocate_at_least(n + 1u)};
			this->imp = {ptr, ptr + n, ptr + static_cast<size_type>(cap - 1u)};
			*::fast_io::freestanding::uninitialized_fill(ptr, ptr + n, ch) = 0;
		}
	}

private:
	constexpr void construct_impl(char_type const *otherptr, size_type othern) noexcept
	{
		if (!othern)
		{
			this->imp = {__builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator), __builtin_addressof(this->nullterminator)};
			this->nullterminator = 0;
		}
		else
		{
			using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
			using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
			auto newres{::fast_io::containers::details::string_allocate_init<allocator_type>(otherptr, othern)};
			auto ptrn{newres.ptr + othern};
			this->imp = {newres.ptr, ptrn, newres.ptr + newres.count};
		}
	}

public:
	explicit constexpr basic_string(::fast_io::containers::basic_string_view<char_type> othervw) noexcept
	{
		this->construct_impl(othervw.data(), othervw.size());
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

	constexpr basic_string(basic_string const &other) noexcept
	{
		auto otherbegin{other.imp.begin_ptr};
		auto othercurr{other.imp.curr_ptr};
		if (otherbegin == othercurr)
		{
			this->imp = {__builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator), __builtin_addressof(this->nullterminator)};
			this->nullterminator = 0;
			return;
		}
		::std::size_t const stringlength{static_cast<::std::size_t>(othercurr - otherbegin)};
		auto [ptr, allocn] = ::fast_io::containers::details::string_allocate_init<allocator_type>(otherbegin, stringlength);
		this->imp = {ptr, ptr + stringlength, ptr + allocn};
	}

private:
	constexpr void assign_impl(char_type const *otherptr, ::std::size_t othern) noexcept
	{
		auto thisbegin{this->imp.begin_ptr};
		auto thisend{this->imp.end_ptr};
		::std::size_t const thiscap{static_cast<::std::size_t>(thisend - thisbegin)};
		if (thiscap < othern)
		{
			this->destroy();
			auto [ptr, allocn] = ::fast_io::containers::details::string_allocate_init<allocator_type>(otherptr, othern);
			this->imp.end_ptr = (this->imp.begin_ptr = thisbegin = ptr) + allocn;
		}
		*(this->imp.curr_ptr = ::fast_io::freestanding::my_copy_n(otherptr, othern, thisbegin)) = 0;
	}

public:
	constexpr void assign(::fast_io::containers::basic_string_view<char_type> myview) noexcept
	{
		this->assign_impl(myview.data(), myview.size());
	}
	constexpr void assign_characters(size_type n, char_type ch) noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto endptr{this->imp.end_ptr};
		size_type const thiscap{static_cast<size_type>(endptr - beginptr)};
		if (thiscap < n)
		{
			constexpr size_type mx{::std::numeric_limits<size_type>::max()};
			if (n == mx)
			{
				::fast_io::fast_terminate();
			}
			this->destroy();
			size_type const np1{static_cast<size_type>(n + 1u)};
			using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
			using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
			auto [ptr, allocn]{typed_allocator_type::allocate_at_least(np1)};
			this->imp.end_ptr = (this->imp.begin_ptr = beginptr = ptr) + static_cast<size_type>(allocn - 1u);
		}
		*(this->imp.curr_ptr = ::fast_io::freestanding::uninitialized_fill_n(beginptr, n, ch)) = 0;
	}
	constexpr void assign_characters(size_type n) noexcept
	{
		this->assign_characters(n, 0);
	}
	constexpr void assign_with_character(char_type ch) noexcept
	{
		this->assign_characters(1u, ch);
	}
	constexpr basic_string &operator=(basic_string const &other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		this->assign_impl(other.imp.begin_ptr, static_cast<::std::size_t>(other.imp.curr_ptr - other.imp.begin_ptr));
		return *this;
	}

private:
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	constexpr void append_cold_impl(char_type const *otherptr, size_type othern) noexcept
	{
		using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
		using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, chtype>;
		constexpr size_type mx{::std::numeric_limits<size_type>::max()};
		constexpr size_type mxdiv2{::std::numeric_limits<size_type>::max() / 2u};
		constexpr size_type mxm1{static_cast<size_type>(mx - 1u)};

		auto beginptr{this->imp.begin_ptr}, currptr{this->imp.curr_ptr}, endptr{this->imp.end_ptr};
		bool const is_sso{this->imp.begin_ptr == __builtin_addressof(this->nullterminator)};
		size_type thissize{static_cast<size_type>(currptr - beginptr)};
		size_type thiscap{static_cast<size_type>(endptr - beginptr)};
		size_type newsize{thissize + othern};

		size_type bigcap;
		if (mxdiv2 <= thiscap)
		{
			bigcap = mxm1;
		}
		else
		{
			bigcap = static_cast<size_type>(thiscap << 1u);
		}
		size_type newcap{newsize};
		if (newcap < bigcap)
		{
			newcap = bigcap;
		}
		if (newcap == mx)
		{
			::fast_io::fast_terminate();
		}
		size_type const newcapp1{static_cast<size_type>(newcap + 1u)};
		auto [ptr, allocn]{typed_allocator_type::allocate_at_least(newcapp1)};
		this->imp.begin_ptr = ptr;
		this->imp.end_ptr = ptr + static_cast<size_type>(allocn - 1u);
		auto it{ptr};
		it = ::fast_io::freestanding::non_overlapped_copy(beginptr, currptr, it);
		it = ::fast_io::freestanding::non_overlapped_copy_n(otherptr, othern, it);
		*it = 0;
		this->imp.curr_ptr = it;
		if (!is_sso)
		{
			typed_allocator_type::deallocate_n(beginptr, static_cast<size_type>(static_cast<size_type>(endptr - beginptr) + 1u));
		}
	}
	constexpr void append_impl(char_type const *otherptr, size_type othern) noexcept
	{
		auto beginptr{this->imp.begin_ptr}, currptr{this->imp.curr_ptr}, endptr{this->imp.end_ptr};
		size_type thissize{static_cast<size_type>(currptr - beginptr)};
		size_type thiscap{static_cast<size_type>(endptr - beginptr)};
		size_type newsize{thissize + othern};
		bool const needreallocate{thiscap < newsize};
		if (needreallocate)
		{
			this->append_cold_impl(otherptr, othern);
			return;
		}
		*(this->imp.curr_ptr = ::fast_io::freestanding::my_copy_n(otherptr, othern, currptr)) = 0;
	}

public:
	constexpr void append(::fast_io::containers::basic_string_view<char_type> vw) noexcept
	{
		this->append_impl(vw.data(), vw.size());
	}
	constexpr void append(basic_string const &other) noexcept
	{
		this->append_impl(other.data(), other.size());
	}

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
		auto beginptr{this->imp.begin_ptr};
	{
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

#if defined(__cpp_lib_three_way_comparison)

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
	return basic_string<char_type, allocator_type>(::fast_io::containers::basic_string_view<char_type>(first, static_cast<::std::size_t>(last - first)));
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
