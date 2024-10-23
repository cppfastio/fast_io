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
	using string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;

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
			auto newres{::fast_io::containers::details::string_allocate_init<allocator_type>(otherptr, othern)};
			auto ptrn{newres.ptr + othern};
			this->imp = {newres.ptr, ptrn, newres.ptr + newres.count};
		}
	}

public:
	explicit constexpr basic_string(char_type const *f, char_type const *e) noexcept
	{
		this->construct_impl(f, static_cast<size_type>(e - f));
	}

	explicit constexpr basic_string(string_view_type othervw) noexcept
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
		*(this->imp.curr_ptr = ::fast_io::freestanding::overlapped_copy_n(otherptr, othern, thisbegin)) = 0;
	}

public:
	constexpr void assign(string_view_type myview) noexcept
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
	constexpr pointer insert_cold_impl(pointer insertpos, char_type const *otherptr, size_type othern) noexcept
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
		it = ::fast_io::freestanding::non_overlapped_copy(beginptr, insertpos, it);
		auto retit{it};
		it = ::fast_io::freestanding::non_overlapped_copy_n(otherptr, othern, it);
		it = ::fast_io::freestanding::non_overlapped_copy(insertpos, currptr, it);
		*it = 0;
		this->imp.curr_ptr = it;
		if (!is_sso)
		{
			typed_allocator_type::deallocate_n(beginptr, static_cast<size_type>(static_cast<size_type>(endptr - beginptr) + 1u));
		}
		return retit;
	}
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	constexpr void append_cold_impl(char_type const *otherptr, size_type othern) noexcept
	{
#if 0
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
#else
		this->insert_cold_impl(this->imp.curr_ptr, otherptr, othern);
#endif
	}
	constexpr void append_impl(char_type const *otherptr, size_type othern) noexcept
	{
		auto beginptr{this->imp.begin_ptr}, currptr{this->imp.curr_ptr}, endptr{this->imp.end_ptr};
		size_type thissize{static_cast<size_type>(currptr - beginptr)};
		size_type thiscap{static_cast<size_type>(endptr - beginptr)};
		size_type newsize{thissize + othern};
		bool const needreallocate{thiscap < newsize};
		if (needreallocate) [[unlikely]]
		{
			this->append_cold_impl(otherptr, othern);
			return;
		}
		*(this->imp.curr_ptr = ::fast_io::freestanding::overlapped_copy_n(otherptr, othern, currptr)) = 0;
	}

public:
	constexpr void append(string_view_type vw) noexcept
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

	constexpr void shrink_to_fit() noexcept
	{
		if (this->imp.curr_ptr == this->imp.end_ptr)
		{
			return;
		}
		::fast_io::containers::details::string_heap_dilate_uncheck<allocator_type>(this->imp, static_cast<size_type>(this->imp.curr_ptr - this->imp.begin_ptr), __builtin_addressof(this->nullterminator));
	}

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

private:
	constexpr pointer insert_impl(pointer ptr, char_type const *otherptr, size_type othern) noexcept
	{
		auto beginptr{this->imp.begin_ptr}, currptr{this->imp.curr_ptr}, endptr{this->imp.end_ptr};
		size_type thissize{static_cast<size_type>(currptr - beginptr)};
		size_type thiscap{static_cast<size_type>(endptr - beginptr)};
		size_type newsize{thissize + othern};
		bool const needreallocate{thiscap < newsize};
		if (needreallocate) [[unlikely]]
		{
			return this->insert_cold_impl(ptr, otherptr, othern);
		}
		auto newcurrptr{currptr + othern};
		*newcurrptr = 0;
		auto lastptr{::fast_io::freestanding::copy_backward(ptr, currptr, newcurrptr)};
		auto retptr{::fast_io::freestanding::copy_backward(otherptr, otherptr + othern, lastptr)};
		this->imp.curr_ptr = newcurrptr;
		return retptr;
	}
	constexpr void insert_index_impl(size_type idx, char_type const *otherptr, size_type othern) noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		size_type sz{static_cast<size_type>(this->imp.curr_ptr - beginptr)};
		if (sz < idx)
		{
			::fast_io::fast_terminate();
		}
		this->insert_impl(beginptr + idx, otherptr, othern);
	}

public:
	constexpr void insert_index(size_type idx, string_view_type vw) noexcept
	{
		return this->insert_index_impl(idx, vw.data(), vw.size());
	}
	constexpr iterator insert(const_iterator ptr, string_view_type vw) noexcept
	{
#ifdef __cpp_if_consteval
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return this->insert_impl(this->imp.begin_ptr + (ptr - this->imp.begin_ptr), vw.data(), vw.size());
		}
		else
		{
			return this->insert_impl(const_cast<pointer>(ptr), vw.data(), vw.size());
		}
	}
	constexpr void insert_index(size_type idx, basic_string const &other) noexcept
	{
		return this->insert_index_impl(idx, other.data(), other.size());
	}

private:
	constexpr pointer erase_impl(pointer first, pointer last) noexcept
	{
		*(this->imp.curr_ptr = ::fast_io::freestanding::my_copy(last, this->imp.curr_ptr, first)) = 0;
		return first;
	}

	constexpr pointer erase_impl(pointer first) noexcept
	{
		return this->erase_impl(first, first + 1);
	}

public:
	constexpr iterator erase(const_iterator first, const_iterator last) noexcept
	{
#ifdef __cpp_if_consteval
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			auto beginptr{this->imp.begin_ptr};
			return this->erase_impl(beginptr + (first - beginptr), beginptr + (last - beginptr));
		}
		else
		{
			return this->erase_impl(const_cast<pointer>(first), const_cast<pointer>(last));
		}
	}
	constexpr void erase_index(size_type firstidx, size_type lastidx) noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto currptr{this->imp.curr_ptr};
		size_type const sz{static_cast<size_type>(currptr - beginptr)};
		if (lastidx < firstidx || sz < lastidx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		this->erase_impl(beginptr + firstidx, beginptr + lastidx);
	}
	constexpr iterator erase(const_iterator it) noexcept
	{
#ifdef __cpp_if_consteval
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			auto beginptr{this->imp.begin_ptr};
			return this->erase_impl(beginptr + (it - beginptr));
		}
		else
		{
			return this->erase_impl(const_cast<pointer>(it));
		}
	}
	constexpr void erase_index(size_type idx) noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto currptr{this->imp.curr_ptr};
		size_type const sz{static_cast<size_type>(currptr - beginptr)};
		if (sz <= idx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		this->erase_impl(beginptr + idx);
	}
	constexpr void swap(basic_string &other) noexcept
	{
		::std::swap(other.imp, this->imp);
		if (other.imp.begin_ptr == __builtin_addressof(this->nullterminator))
		{
			other.imp = {__builtin_addressof(other.nullterminator),
						 __builtin_addressof(other.nullterminator),
						 __builtin_addressof(other.nullterminator)};
			other.nullterminator = 0;
		}
		if (this->imp.begin_ptr == __builtin_addressof(other.nullterminator))
		{
			this->imp = {__builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator),
						 __builtin_addressof(this->nullterminator)};
			this->nullterminator = 0;
		}
	}
	constexpr iterator insert(const_iterator ptr, basic_string const &other) noexcept
	{
#ifdef __cpp_if_consteval
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return this->insert_impl(this->imp.begin_ptr + (ptr - this->imp.begin_ptr), other.data(), other.size());
		}
		else
		{
			return this->insert_impl(const_cast<pointer>(ptr), other.data(), other.size());
		}
	}

private:
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	constexpr pointer replace_cold_impl(pointer first, pointer last, const_pointer otherptr, size_type othern) noexcept
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
		size_type const toremoven{static_cast<size_type>(last - first)};
		size_type newsize{thissize + othern - toremoven};

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
		it = ::fast_io::freestanding::non_overlapped_copy(beginptr, first, it);
		auto retit{it};
		it = ::fast_io::freestanding::non_overlapped_copy_n(otherptr, othern, it);
		it = ::fast_io::freestanding::non_overlapped_copy(last, currptr, it);
		*it = 0;
		this->imp.curr_ptr = it;
		if (!is_sso)
		{
			typed_allocator_type::deallocate_n(beginptr, static_cast<size_type>(static_cast<size_type>(endptr - beginptr) + 1u));
		}
		return retit;
	}
	constexpr pointer replace_impl(pointer first, pointer last, const_pointer otherdata, size_type othersize) noexcept
	{
		size_type const toremoven{static_cast<size_type>(last - first)};
		auto currptr{this->imp.curr_ptr};
		if (othersize < toremoven) [[likely]]
		{
			auto itr{::fast_io::freestanding::overlapped_copy_n(otherdata, othersize, first)};
			if (itr != last)
			{
				*(this->imp.curr_ptr = ::fast_io::freestanding::copy_backward(itr, last, currptr)) = 0;
			}
			return first;
		}
		return this->replace_cold_impl(first, last, otherdata, othersize);
	}

	constexpr void replace_index_impl(size_type firstidx, size_type lastidx, const_pointer otherdata, size_type othersize) noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto currptr{this->imp.curr_ptr};
		size_type const sz{static_cast<size_type>(currptr - beginptr)};
		if (lastidx < firstidx || sz < lastidx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		this->replace_impl(beginptr + firstidx, beginptr + lastidx, otherdata, othersize);
	}

public:
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr iterator replace(const_iterator first, const_iterator last, string_view_type view) noexcept
	{
#ifdef __cpp_if_consteval
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			auto beginptr{this->imp.begin_ptr};
			return this->replace_impl(beginptr + (first - beginptr), beginptr + (last - beginptr), view.data(), view.size());
		}
		else
		{
			return this->replace_impl(const_cast<pointer>(first), const_cast<pointer>(last), view.data(), view.size());
		}
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr iterator replace(const_iterator first, const_iterator last, basic_string const &view) noexcept
	{
#ifdef __cpp_if_consteval
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			auto beginptr{this->imp.begin_ptr};
			return this->replace_impl(beginptr + (first - beginptr), beginptr + (last - beginptr), view.data(), view.size());
		}
		else
		{
			return this->replace_impl(const_cast<pointer>(first), const_cast<pointer>(last), view.data(), view.size());
		}
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr void replace_index(size_type firstidx, size_type lastidx, string_view_type view) noexcept
	{
		return this->replace_index_impl(firstidx, lastidx, view.data(), view.size());
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr void replace_index(size_type firstidx, size_type lastidx, basic_string const &view) noexcept
	{
		return this->replace_index_impl(firstidx, lastidx, view.data(), view.size());
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr cstring_view_type subview_back(size_type count) const noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto currptr{this->imp.curr_ptr};
		size_type const thisn{static_cast<size_type>(currptr - beginptr)};
		if (thisn < count) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return cstring_view_type(::fast_io::containers::null_terminated, beginptr + (thisn - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr cstring_view_type subview_back_unchecked(size_type count) const noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto currptr{this->imp.curr_ptr};
		size_type const thisn{static_cast<size_type>(currptr - beginptr)};
		return cstring_view_type(::fast_io::containers::null_terminated, beginptr + (thisn - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr string_view_type subview_front(size_type count) const noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto currptr{this->imp.curr_ptr};
		size_type const thisn{static_cast<size_type>(currptr - beginptr)};
		if (thisn < count) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return string_view_type(beginptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr string_view_type subview_front_unchecked(size_type count) const noexcept
	{
		return string_view_type(this->imp.begin_ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr string_view_type subview(size_type pos, size_type count = ::fast_io::containers::npos) const noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto currptr{this->imp.curr_ptr};
		size_type const thisn{static_cast<size_type>(currptr - beginptr)};
		if (thisn < pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		size_type const val{thisn - pos};
		if (val < count)
		{
			if (count != ::fast_io::containers::npos) [[unlikely]]
			{
				::fast_io::fast_terminate();
			}
			count = val;
		}
		return string_view_type(beginptr + pos, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr string_view_type subview_unchecked(size_type pos, size_type count = ::fast_io::containers::npos) const noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		if (count == ::fast_io::containers::npos)
		{
			auto currptr{this->imp.curr_ptr};
			size_type const thisn{static_cast<size_type>(currptr - beginptr)};
			size_type const val{thisn - pos};
			count = val;
		}
		return string_view_type(beginptr + pos, count);
	}

	inline constexpr bool starts_with(string_view_type sv) const noexcept
	{
		return string_view_type(this->data(), this->size()).starts_with(sv);
	}
	inline constexpr bool starts_with(basic_string const &other) const noexcept
	{
		return this->starts_with(string_view_type(other));
	}

	inline constexpr bool starts_with_character(value_type ch) const noexcept
	{
		return string_view_type(this->data(), this->size()).starts_with_character(ch);
	}
	inline constexpr bool ends_with(string_view_type sv) const noexcept
	{
		return string_view_type(this->data(), this->size()).ends_with(sv);
	}
	inline constexpr bool ends_with(basic_string const &other) const noexcept
	{
		return this->ends_with(string_view_type(other));
	}

	inline constexpr bool ends_with_character(value_type ch) const noexcept
	{
		return string_view_type(this->data(), this->size()).ends_with_character(ch);
	}

	inline constexpr void remove_suffix(size_type svn) noexcept
	{
		auto beginptr{this->imp.begin_ptr};
		auto currptr{this->imp.curr_ptr};
		size_type const thisn{static_cast<size_type>(currptr - beginptr)};
		if (thisn < svn) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		currptr -= svn;
		*(this->imp.curr_ptr = currptr) = 0;
	}

	inline constexpr void remove_suffix_unchecked(size_type svn) noexcept
	{
		*(this->imp.curr_ptr -= svn) = 0;
	}

	inline constexpr bool contains(string_view_type vw) const noexcept
	{
		auto ed{this->imp.curr_ptr};
		return ::std::search(this->imp.begin_ptr, ed, vw.ptr, vw.ptr + vw.n) != ed;
	}
	inline constexpr bool contains(basic_string const &other) const noexcept
	{
		auto ed{this->imp.curr_ptr};
		return ::std::search(this->imp.begin_ptr, ed, other.cbegin(), other.cend()) != ed;
	}
	inline constexpr bool contains_character(char_type ch) const noexcept
	{
		auto ed{this->imp.curr_ptr};
		return ::std::find(this->imp.begin_ptr, ed, ch) != ed;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr size_type copy(char_type *dest, size_type count, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).copy(dest, count, pos);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr size_type copy_unchecked(char_type *dest, size_type count, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).copy_unchecked(dest, count, pos);
	}

	inline constexpr basic_string substr_back(size_type count) const noexcept
	{
		return basic_string(this->subview_back(count));
	}

	inline constexpr basic_string substr_front(size_type count) const noexcept
	{
		return basic_string(this->subview_front(count));
	}

	inline constexpr basic_string substr(size_type idx, size_type count = ::fast_io::containers::npos) const noexcept
	{
		return basic_string(this->subview(idx, count));
	}

	inline constexpr size_type find_character(char_type ch, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_character(ch, pos);
	}
	inline constexpr size_type find_not_character(char_type ch, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_not_character(ch, pos);
	}
	inline constexpr size_type rfind_character(char_type ch, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).rfind_character(ch, pos);
	}
	inline constexpr size_type rfind_not_character(char_type ch, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).rfind_not_character(ch, pos);
	}
	inline constexpr size_type find(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return string_view_type(this->data(), this->size()).find(s, pos, count);
	}
	inline constexpr size_type find(string_view_type v, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).find(v, pos);
	}
	inline constexpr size_type rfind(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return string_view_type(this->data(), this->size()).rfind(s, pos, count);
	}
	inline constexpr size_type rfind(string_view_type v, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).rfind(v.data(), v.size(), pos);
	}
	inline constexpr size_type rfind(basic_string const &v, size_type pos = 0) const noexcept
	{
		return string_view_type(this->data(), this->size()).rfind(v.data(), v.size(), pos);
	}

	inline constexpr size_type find_first_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_first_of(s, pos, count);
	}
	inline constexpr size_type find_first_of(string_view_type s, size_type pos) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_first_of(s, pos);
	}
	inline constexpr size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_first_of(s, pos, count);
	}
	inline constexpr size_type find_first_not_of(string_view_type s, size_type pos) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_first_not_of(s, pos);
	}
	inline constexpr size_type find_first_not_of(basic_string const &s, size_type pos) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_first_not_of(s.data(), s.size(), pos);
	}
	inline constexpr size_type find_last_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_last_of(s, pos, count);
	}
	inline constexpr size_type find_last_of(string_view_type s, size_type pos) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_last_of(s, pos);
	}
	inline constexpr size_type find_last_of(basic_string const &s, size_type pos) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_last_of(s.data(), pos, s.size());
	}
	inline constexpr size_type find_last_not_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_last_not_of(s, pos, count);
	}
	inline constexpr size_type find_last_not_of(string_view_type s, size_type pos) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_last_not_of(s.ptr, pos, s.n);
	}
	inline constexpr size_type find_last_not_of(basic_string const &s, size_type pos) const noexcept
	{
		return string_view_type(this->data(), this->size()).find_last_not_of(s.data(), pos, s.size());
	}
	inline constexpr auto compare_three_way(size_type pos1, size_type count1, const_pointer s, size_type count2) const noexcept
	{
		return this->subview(pos1, count1) <=> string_view_type(s, count2);
	}
	inline constexpr auto compare_three_way_unchecked(size_type pos1, size_type count1, const_pointer s, size_type count2) const noexcept
	{
		return this->subview_unchecked(pos1, count1) <=> string_view_type(s, count2);
	}
	inline constexpr auto compare_three_way(size_type pos1, size_type count1, string_view_type other, size_type pos2, size_type count2) const noexcept
	{
		return this->subview(pos1, count1) <=> other.subview(pos2, count2);
	}
	inline constexpr auto compare_three_way_unchecked(size_type pos1, size_type count1, string_view_type other, size_type pos2, size_type count2) const noexcept
	{
		return this->subview_unchecked(pos1, count1) <=> other.subview_unchecked(pos2, count2);
	}
	inline constexpr auto compare_three_way(size_type pos1, size_type count1, basic_string const &other, size_type pos2, size_type count2) const noexcept
	{
		return this->subview(pos1, count1) <=> other.subview(pos2, count2);
	}
	inline constexpr auto compare_three_way_unchecked(size_type pos1, size_type count1, basic_string const &other, size_type pos2, size_type count2) const noexcept
	{
		return this->subview_unchecked(pos1, count1) <=> other.subview_unchecked(pos2, count2);
	}

	template <typename Operation>
	inline constexpr void resize_and_overwrite(size_type count, Operation op) noexcept
	{
		auto beginptr{this->imp.begin_ptr}, currptr{this->imp.curr_ptr};
		size_type thissize{static_cast<size_type>(currptr - beginptr)};
		if (thissize < count)
		{
			auto endptr{this->imp.end_ptr};
			size_type thiscap{static_cast<size_type>(endptr - beginptr)};
			if (thiscap < count)
			{
				::fast_io::containers::details::string_heap_dilate_uncheck<allocator_type>(this->imp, count, __builtin_addressof(this->nullterminator));
				beginptr = this->imp.begin_ptr;
			}
		}
		*(this->imp.curr_ptr = (beginptr + op(beginptr, count))) = 0;
	}

	inline constexpr void resize(size_type count, char_type ch) noexcept
	{
		auto beginptr{this->imp.begin_ptr}, currptr{this->imp.curr_ptr};
		size_type thissize{static_cast<size_type>(currptr - beginptr)};
		if (count <= thissize)
		{
			if (count != thissize)
			{
				*(this->imp.curr_ptr -= static_cast<size_type>(thissize - count)) = 0;
			}
			return;
		}
		auto endptr{this->imp.end_ptr};
		size_type thiscap{static_cast<size_type>(endptr - beginptr)};
		if (thiscap < count)
		{
			::fast_io::containers::details::string_heap_dilate_uncheck<allocator_type>(this->imp, count, __builtin_addressof(this->nullterminator));
			currptr = this->imp.curr_ptr;
		}
		*(this->imp.curr_ptr = ::fast_io::freestanding::uninitialized_fill_n(currptr, static_cast<size_type>(count - thissize), ch)) = 0;
	}
	inline constexpr void resize(size_type count) noexcept
	{
		this->resize(count, 0);
	}
};

template <::std::integral chtype, typename allocator1, typename U>
inline constexpr void erase(::fast_io::containers::basic_string<chtype, allocator1> const &c, U const &value)
{
	auto it = ::std::remove(c.begin(), c.end(), value);
	auto r = c.end() - it;
	c.erase(it, c.end());
	return r;
}

template <::std::integral chtype, typename allocator1, typename Pred>
inline constexpr void erase_if(::fast_io::containers::basic_string<chtype, allocator1> const &c, Pred pred)
{
	auto it = ::std::remove_if(c.begin(), c.end(), pred);
	auto r = c.end() - it;
	c.erase(it, c.end());
	return r;
}

template <::std::integral chtype, typename allocator1, typename allocator2>
constexpr bool operator==(::fast_io::containers::basic_string<chtype, allocator1> const &lhs, ::fast_io::containers::basic_string<chtype, allocator2> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <::std::integral chtype, typename allocator1>
constexpr bool operator==(::fast_io::containers::basic_string<chtype, allocator1> const &lhs, ::fast_io::containers::basic_string_view<chtype> rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <::std::integral chtype, typename allocator1>
constexpr bool operator==(::fast_io::containers::basic_string_view<chtype> lhs, ::fast_io::containers::basic_string<chtype, allocator1> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <::std::integral char_type, typename allocator1, ::std::size_t n>
constexpr bool operator==(::fast_io::containers::basic_string<char_type, allocator1> a, char_type const (&buffer)[n]) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::equal(a.cbegin(), a.cend(), buffer, buffer + nm1);
}

template <::std::integral char_type, typename allocator1, ::std::size_t n>
constexpr bool operator==(char_type const (&buffer)[n], ::fast_io::containers::basic_string<char_type, allocator1> a) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::equal(buffer, buffer + nm1, a.cbegin(), a.cend());
}

#if defined(__cpp_lib_three_way_comparison)

template <::std::integral chtype, typename allocator1, typename allocator2>
constexpr auto operator<=>(::fast_io::containers::basic_string<chtype, allocator1> const &lhs, ::fast_io::containers::basic_string<chtype, allocator2> const &rhs) noexcept
{
	return ::std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), ::std::compare_three_way{});
}

template <::std::integral chtype, typename allocator1>
constexpr auto operator<=>(::fast_io::containers::basic_string<chtype, allocator1> const &lhs, ::fast_io::containers::basic_string_view<chtype> rhs) noexcept
{
	return ::std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), ::std::compare_three_way{});
}

template <::std::integral chtype, typename allocator1>
constexpr auto operator<=>(::fast_io::containers::basic_string_view<chtype> lhs, ::fast_io::containers::basic_string<chtype, allocator1> const &rhs) noexcept
{
	return ::std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), ::std::compare_three_way{});
}

template <::std::integral char_type, typename allocator1, ::std::size_t n>
constexpr auto operator<=>(::fast_io::containers::basic_string<char_type, allocator1> a, char_type const (&buffer)[n]) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::lexicographical_compare_three_way(a.cbegin(), a.cend(), buffer, buffer + nm1, ::std::compare_three_way{});
}

template <::std::integral char_type, typename allocator1, ::std::size_t n>
constexpr auto operator<=>(char_type const (&buffer)[n], ::fast_io::containers::basic_string<char_type, allocator1> a) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::lexicographical_compare_three_way(buffer, buffer + nm1, a.cbegin(), a.cend(), ::std::compare_three_way{});
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

template <::std::integral chtype, typename alloctype>
inline constexpr void swap(::fast_io::containers::basic_string<chtype, alloctype> &a, ::fast_io::containers::basic_string<chtype, alloctype> &b) noexcept
{
	a.swap(b);
}

} // namespace fast_io::containers
