#pragma once
// unfinished
namespace fast_io
{

namespace containers
{

template <typename T>
class dynarray
{
public:
	using value_type = T;
	using pointer = value_type *;
	using const_pointer = value_type const *;

	using reference = value_type &;
	using const_reference = value_type const &;

	using iterator = value_type *;
	using const_iterator = value_type const *;

	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	pointer buffer_begin, buffer_end;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr dynarray(size_type n, ::fast_io::for_overwrite_t)
	{
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			buffer_end = (buffer_begin = new T[n]) + n;
		}
		else
		{
			void *bufferptr;
			if constexpr (sizeof(T) == 1)
			{
#if defined(__GNUC__) || defined(__clang__)
				bufferptr = __builtin_alloca(n);
#else
				bufferptr = _malloca(n);
#endif
			}
			else
			{
#if defined(__GNUC__) || defined(__clang__)
				::std::size_t toallocate{n};
				if (__builtin_mul_overflow(n, sizeof(T), __builtin_addressof(toallocate)))
				{
					__builtin_trap();
				}
				constexpr ::std::size_t aligns{alignof(T) < alignof(::std::max_align_t) ? alignof(::std::max_align_t) : alignof(T)};
				bufferptr = __builtin_alloca_with_align(toallocate, aligns);
#else
				constexpr ::std::size_t mx{SIZE_MAX / sizeof(T)};
				if (mx < n)
				{
					::std::abort();
				}
				bufferptr = _malloca(n * sizeof(T));
#endif
			}
			pointer first = reinterpret_cast<pointer>(bufferptr);
			pointer last = first + n;
			for (pointer it{first}; it != last; ++it)
			{
				new (it) value_type;
			}
			buffer_begin = first;
			buffer_end = last;
		}
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr dynarray(size_type n)
	{
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			buffer_end = (buffer_begin = new T[n]{}) + n;
		}
		else
		{
			void *bufferptr;
			::std::size_t toallocate;
			if constexpr (sizeof(T) == 1)
			{
				toallocate = n;
#if defined(__GNUC__) || defined(__clang__)
				bufferptr = __builtin_alloca(n);
#else
				bufferptr = _malloca(n);
#endif
			}
			else
			{
#if defined(__GNUC__) || defined(__clang__)
				if (__builtin_mul_overflow(n, sizeof(T), __builtin_addressof(toallocate)))
				{
					__builtin_trap();
				}
				constexpr ::std::size_t aligns{alignof(T) < alignof(::std::max_align_t) ? alignof(::std::max_align_t) : alignof(T)};
				bufferptr = __builtin_alloca_with_align(toallocate, aligns);
#else
				constexpr ::std::size_t mx{SIZE_MAX / sizeof(T)};
				if (mx < n)
				{
					::std::abort();
				}
				toallocate = n * sizeof(T);
				bufferptr = _malloca(toallocate);
#endif
			}
			if constexpr(::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
			{
				__builtin_memset(bufferptr,0,toallocate);
				buffer_end = ((buffer_begin = reinterpret_cast<pointer>(bufferptr)) + n);
			}
			else
			{
				pointer first = reinterpret_cast<pointer>(bufferptr);
				pointer last = first + n;
				for (pointer it{first}; it != last; ++it)
				{
					new (static_cast<void*>(it)) value_type();
				}
				buffer_begin = first;
				buffer_end = last;
			}
		}
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr dynarray(::std::initializer_list<value_type> ilist)
	{
		::std::size_t n{ilist.size()};

#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			buffer_end = (buffer_begin = new T[n]) + n;
			for (::std::size_t i{}; i != n; ++i)
			{
				buffer_begin[i] = ::std::move(ilist[i]);
			}
		}
		else
		{
			void *bufferptr;
			if constexpr (sizeof(T) == 1)
			{
#if defined(__GNUC__) || defined(__clang__)
				bufferptr = __builtin_alloca(n);
#else
				bufferptr = _malloca(n);
#endif
			}
			else
			{
#if defined(__GNUC__) || defined(__clang__)
				::std::size_t toallocate{n};
				if (__builtin_mul_overflow(n, sizeof(T), __builtin_addressof(toallocate)))
				{
					__builtin_trap();
				}
				constexpr ::std::size_t aligns{alignof(T) < alignof(::std::max_align_t) ? alignof(::std::max_align_t) : alignof(T)};
				bufferptr = __builtin_alloca_with_align(toallocate, aligns);
#else
				constexpr ::std::size_t mx{SIZE_MAX / sizeof(T)};
				if (mx < n)
				{
					::std::abort();
				}
				bufferptr = _malloca(n * sizeof(T));
#endif
			}
			pointer first = reinterpret_cast<pointer>(bufferptr);
			pointer last = first + n;
			auto ilistfirst{ilist.begin()};
			for (pointer it{first}; it != last; ++it)
			{
				new (it) value_type(*ilistfirst);
				++ilistfirst;
			}
			buffer_begin = first;
			buffer_end = last;
		}
	}

	[[nodiscard]] constexpr ::std::size_t size() const noexcept
	{
		return static_cast<::std::size_t>(buffer_end - buffer_begin);
	}

	[[nodiscard]] static inline constexpr ::std::size_t max_size() noexcept
	{
		constexpr ::std::size_t mxv{SIZE_MAX / sizeof(T)};
		return mxv;
	}

	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return buffer_begin == buffer_end;
	}

	[[nodiscard]] inline constexpr bool is_empty() const noexcept
	{
		return buffer_begin == buffer_end;
	}

	[[nodiscard]] constexpr pointer data() noexcept
	{
		return buffer_begin;
	}

	[[nodiscard]] constexpr const_pointer data() const noexcept
	{
		return buffer_begin;
	}

	[[nodiscard]] constexpr const_reference front() const noexcept
	{
		return *buffer_begin;
	}
	[[nodiscard]] constexpr reference front() noexcept
	{
		return *buffer_begin;
	}
	[[nodiscard]] constexpr const_reference back() const noexcept
	{
		return buffer_end[-1];
	}
	[[nodiscard]] constexpr reference back() noexcept
	{
		return buffer_end[-1];
	}

	[[nodiscard]] constexpr iterator begin() noexcept
	{
		return buffer_begin;
	}
	[[nodiscard]] constexpr iterator end() noexcept
	{
		return buffer_end;
	}
	[[nodiscard]] constexpr const_iterator begin() const noexcept
	{
		return buffer_begin;
	}
	[[nodiscard]] constexpr const_iterator end() const noexcept
	{
		return buffer_end;
	}
	[[nodiscard]] constexpr const_iterator cbegin() const noexcept
	{
		return buffer_begin;
	}
	[[nodiscard]] constexpr const_iterator cend() const noexcept
	{
		return buffer_end;
	}
	[[nodiscard]] constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{buffer_end};
	}
	[[nodiscard]] constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{buffer_begin};
	}
	[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{buffer_end};
	}
	[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{buffer_begin};
	}
	[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{buffer_end};
	}
	[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{buffer_begin};
	}

	[[nodiscard]] constexpr reference operator[](size_type n) noexcept
	{
		return buffer_begin[n];
	}
	[[nodiscard]] constexpr const_reference operator[](size_type n) const noexcept
	{
		return buffer_begin[n];
	}

	dynarray(dynarray const &) = delete;
	dynarray &operator=(dynarray const &) = delete;

	constexpr ~dynarray()
	{
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			delete[] buffer_begin;
		}
		else
		{
			if constexpr (!::std::is_trivially_destructible_v<T>)
			{
				for (auto it{buffer_begin}; it != buffer_end; ++it)
				{
					it->~value_type();
				}
			}
#if defined(_MSC_VER) && !defined(__clang__)
			_freea(buffer_begin);
#endif
		}
	}
};

} // namespace containers

} // namespace fast_io
