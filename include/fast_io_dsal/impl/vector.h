#pragma once
namespace fast_io
{

namespace containers
{

namespace details
{

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	vector_model
{
	char8_t *begin_ptr;
	char8_t *curr_ptr;
	char8_t *end_ptr;
};

template <typename allocator>
inline void grow_to_size_common_impl(vector_model *m, ::std::size_t newcap) noexcept
{
	auto begin_ptr{m->begin_ptr};

	::std::size_t const old_size{static_cast<::std::size_t>(m->curr_ptr - begin_ptr)};

	if constexpr (allocator::has_reallocate)
	{
		begin_ptr = reinterpret_cast<char8_t *>(allocator::reallocate(begin_ptr, newcap));
	}
	else
	{
		auto end_ptr{m->end_ptr};
		::std::size_t const old_cap{static_cast<::std::size_t>(end_ptr - begin_ptr)};
		begin_ptr = reinterpret_cast<char8_t *>(allocator::reallocate_n(begin_ptr, old_cap, newcap));
	}
	m->begin_ptr = begin_ptr;
	m->curr_ptr = begin_ptr + old_size;
	m->end_ptr = begin_ptr + newcap;
}

template <typename allocator>
inline void grow_to_size_common_aligned_impl(vector_model *m, ::std::size_t alignment, ::std::size_t newcap) noexcept
{
	auto begin_ptr{m->begin_ptr};
	::std::size_t const old_size{static_cast<::std::size_t>(m->curr_ptr - begin_ptr)};
	if constexpr (allocator::has_reallocate_aligned)
	{
		begin_ptr = reinterpret_cast<char8_t *>(allocator::reallocate_aligned(begin_ptr, alignment, newcap));
	}
	else
	{
		auto end_ptr{m->end_ptr};
		::std::size_t const oldcap{static_cast<::std::size_t>(end_ptr - begin_ptr)};
		begin_ptr = reinterpret_cast<char8_t *>(allocator::reallocate_aligned_n(begin_ptr, oldcap, alignment, newcap));
	}
	m->begin_ptr = begin_ptr;
	m->curr_ptr = begin_ptr + old_size;
	m->end_ptr = begin_ptr + newcap;
}

template <typename allocator, ::std::size_t size>
inline constexpr void grow_twice_common_impl(vector_model *m) noexcept
{
	auto begin_ptr{m->begin_ptr};
	auto end_ptr{m->end_ptr};
	grow_to_size_common_impl<allocator>(m,
										cal_grow_twice_size<size, true>(static_cast<::std::size_t>(end_ptr - begin_ptr)));
}

template <typename allocator, ::std::size_t size>
inline constexpr void grow_twice_common_aligned_impl(vector_model *m, ::std::size_t alignment) noexcept
{
	auto begin_ptr{m->begin_ptr};
	auto end_ptr{m->end_ptr};
	grow_to_size_common_aligned_impl<allocator>(m,
												alignment,
												cal_grow_twice_size<size, true>(static_cast<::std::size_t>(end_ptr - begin_ptr)));
}

template <typename T>
struct vector_internal
{
	T *begin_ptr{};
	T *curr_ptr{};
	T *end_ptr{};
};

} // namespace details

template <::std::movable T, typename allocator>
class vector
{
public:
	using allocator_type = allocator;
	using value_type = T;

private:
	using typed_allocator_type = typed_generic_allocator_adapter<allocator_type, value_type>;

public:
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
	::fast_io::containers::details::vector_internal<T> imp;

	explicit constexpr vector() noexcept = default;

private:
	void destroy() noexcept
	{
		clear();
		if constexpr (!typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(imp.begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(imp.begin_ptr,
											   static_cast<::std::size_t>(imp.end_ptr - imp.begin_ptr));
		}
	}
	struct run_destroy
	{
		vector *thisvec{};
		constexpr run_destroy() noexcept = default;
		explicit constexpr run_destroy(vector *p) noexcept
			: thisvec(p)
		{}
		run_destroy(run_destroy const &) = delete;
		run_destroy &operator=(run_destroy const &) = delete;
		constexpr ~run_destroy()
		{
			if (thisvec)
			{
				thisvec->destroy();
			}
		}
	};

	constexpr void default_construct_impl(size_type n)
	{
		auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
										typed_allocator_type::allocate(n)) +
								   n};
		run_destroy des(this);
		for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
		{
			::std::construct_at(this->imp.curr_ptr);
		}
		des.thisvec = nullptr;
	}

public:
	explicit constexpr vector(size_type n) noexcept(::fast_io::freestanding::is_zero_default_constructible_v<value_type> || noexcept(value_type()))
	{
		if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate_zero(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else
		{
			this->default_construct_impl(n);
		}
	}

	explicit constexpr vector(size_type n, ::fast_io::for_overwrite_t) noexcept(::std::is_trivially_default_constructible_v<value_type> || ::fast_io::freestanding::is_zero_default_constructible_v<value_type> || noexcept(value_type()))
	{
		if constexpr (::std::is_trivially_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate_zero(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else
		{
			this->default_construct_impl(n);
		}
	}

	explicit constexpr vector(size_type n, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
										typed_allocator_type::allocate(n)) +
								   n};
		run_destroy des(this);
		for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
		{
			::std::construct_at(this->imp.curr_ptr, val);
		}
		des.thisvec = nullptr;
	}

#ifdef __cpp_lib_containers_ranges
	template <::std::ranges::range R>
	explicit constexpr vector(::std::from_range_t, R &&rg)
	{
		using rvaluetype = ::std::ranges::range_value_t<R>;
		if constexpr (::std::ranges::sized_range<R> || ::std::ranges::forward_range<R>)
		{
			auto first{::std::ranges::begin(rg)};
			auto last{::std::ranges::end(rg)};
			size_type n;
			if constexpr (::std::ranges::sized_range<R>)
			{
				n = ::std::ranges::size(rg);
			}
			else
			{
				n = static_cast<size_type>(::std::ranges::distance(first, last));
			}
			auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
											typed_allocator_type::allocate(n)) +
									   n};
			if constexpr (
				::std::contiguous_range<R> &&
				::std::is_trivially_constructible_v<value_type, rvaluetype> &&
				::std::same_as<::std::remove_cref_t<rvaluetype>, ::std::remove_cref_t<value_type>>)
			{
				if (n) [[likely]]
				{
					::std::memcpy(this->imp.curr_ptr, ::std::ranges::data(rg), n * sizeof(value_type));
				}
				this->imp.curr_ptr = e;
			}
			else if constexpr (::std::is_nothrow_constructible_v<value_type, rvaluetype>)
			{
				auto curr{this->imp.begin_ptr};
				for (; curr != e; ++curr)
				{
					::std::construct_at(curr, *first);
					++first;
				}
				this->imp.curr_ptr = e;
			}
			else
			{
				run_destroy des(this);
				for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
				{
					::std::construct_at(this->imp.curr_ptr, *first);
					++first;
				}
				des.thisvec = nullptr;
			}
		}
		else
		{
			run_destroy des(this);
			for (auto const &e : rg)
			{
				this->emplace_back(e);
			}
			des.thisvec = nullptr;
		}
	}

	explicit constexpr vector(::std::initializer_list<value_type> ilist) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
		: vector(::std::from_range, ilist)
	{
	}

#endif

	constexpr vector(vector const &vec)
		requires(::std::copyable<value_type>)
	{
		std::size_t const vecsize{static_cast<std::size_t>(vec.imp.curr_ptr - vec.imp.begin_ptr)};
		if (vecsize == 0)
		{
			return;
		}
		imp.begin_ptr = typed_allocator_type::allocate(vecsize);
		if constexpr (::std::is_trivially_copyable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				std::size_t const n{vecsize * sizeof(value_type)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
				__builtin_memcpy(imp.begin_ptr, vec.imp.begin_ptr, n);
#else
				std::memcpy(imp.begin_ptr, vec.imp.begin_ptr, n);
#endif
#else
				std::memcpy(imp.begin_ptr, vec.imp.begin_ptr, n);
#endif
				imp.end_ptr = imp.curr_ptr = imp.begin_ptr + vecsize;
				return;
			}
		}
		run_destroy des(this);
		this->imp.curr_ptr = this->imp.begin_ptr;
		this->imp.end_ptr = this->imp.begin_ptr + vecsize;
		for (auto i{vec.imp.begin_ptr}; i != vec.imp.curr_ptr; ++i)
		{
			new (this->imp.curr_ptr) value_type(*i);
			++this->imp.curr_ptr;
		}
		des.thisvec = nullptr;
	}
	constexpr vector(vector const &vec) = delete;
	constexpr vector &operator=(vector const &vec)
		requires(::std::copyable<value_type>)
	{
		vector newvec(vec);
		this->operator=(::std::move(newvec));
		return *this;
	}
	constexpr vector &operator=(vector const &vec) = delete;
	constexpr vector(vector &&vec) noexcept
		: imp(vec.imp)
	{
		vec.imp = {};
	}
	constexpr vector &operator=(vector &&vec) noexcept
	{
		this->destroy();
		this->imp = vec.imp;
		vec.imp = nullptr;
		return *this;
	}
	constexpr ~vector()
	{
		destroy();
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	constexpr reference emplace_back_unchecked(Args &&...args) noexcept(noexcept(value_type(::std::forward<Args>(args)...)))
	{
		auto p{::new (imp.curr_ptr) value_type(::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

private:
	inline constexpr void grow_to_size_impl(size_type newcap) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				constexpr ::std::size_t mxv{max_size()};
				if constexpr (1 < sizeof(value_type))
				{
					if (mxv < newcap)
					{
						::fast_io::fast_terminate();
					}
				}
				newcap *= sizeof(value_type);
				if constexpr (alignof(value_type) <= allocator_type::default_alignment)
				{
					::fast_io::containers::details::grow_to_size_common_impl<allocator_type>(
						reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)),
						newcap);
				}
				else
				{
					::fast_io::containers::details::grow_to_size_common_aligned_impl<allocator_type>(
						reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)),
						alignof(value_type), newcap);
				}
				return;
			}
		}
		std::size_t cap;
		if constexpr (!typed_allocator_type::has_deallocate)
		{
			cap = static_cast<size_type>(imp.end_ptr - imp.begin_ptr);
		}
		auto new_begin_ptr = typed_allocator_type::allocate(newcap);
		auto new_i{new_begin_ptr};
		for (auto old_i{imp.begin_ptr}, old_e{imp.curr_ptr}; old_i != old_e; ++old_i)
		{
			new (new_i) value_type(::std::move(*old_i));
			old_i->~value_type();
			++new_i;
		}
		if constexpr (typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(imp.begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(imp.begin_ptr, cap);
		}
		imp.begin_ptr = new_begin_ptr;
		imp.curr_ptr = new_i;
		imp.end_ptr = new_begin_ptr + newcap;
	}
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr void grow_twice_impl() noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				if constexpr (alignof(value_type) <= allocator_type::default_alignment)
				{
					::fast_io::containers::details::grow_twice_common_impl<allocator_type, sizeof(value_type)>(
						reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)));
				}
				else
				{
					::fast_io::containers::details::grow_twice_common_aligned_impl<allocator_type, sizeof(value_type)>(
						reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)),
						alignof(value_type));
				}
				return;
			}
		}
		std::size_t const cap{static_cast<size_type>(imp.end_ptr - imp.begin_ptr)};
		grow_to_size_impl(::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false>(cap));
	}

public:
	constexpr void reserve(size_type n) noexcept
	{
		if (n <= static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
		{
			return;
		}
		grow_to_size_impl(n);
	}

	constexpr void shrink_to_fit() noexcept
	{
		if (imp.curr_ptr == imp.end_ptr)
		{
			return;
		}
		grow_to_size_impl(static_cast<std::size_t>(imp.curr_ptr - imp.begin_ptr));
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	constexpr void pop_back() noexcept
	{
		if (imp.curr_ptr == imp.begin_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		(--imp.curr_ptr)->~value_type();
	}

	constexpr void pop_back_unchecked() noexcept
	{
		(--imp.curr_ptr)->~value_type();
	}

	constexpr void push_back(T const &value) noexcept(noexcept(this->emplace_back(value)))
	{
		this->emplace_back(value);
	}
	constexpr void push_back(T &&value) noexcept(noexcept(this->emplace_back(::std::move(value))))
	{
		this->emplace_back(::std::move(value));
	}
	constexpr void push_back_unchecked(T const &value) noexcept(noexcept(this->emplace_back_unchecked(value)))
	{
		this->emplace_back_unchecked(value);
	}
	constexpr void push_back_unchecked(T &&value) noexcept(noexcept(this->emplace_back_unchecked(::std::move(value))))
	{
		this->emplace_back_unchecked(::std::move(value));
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
	constexpr void clear() noexcept
	{
		if constexpr (!::fast_io::freestanding::is_trivially_relocatable_v<value_type>)
		{
			for (auto old_i{imp.begin_ptr}, old_e{imp.curr_ptr}; old_i != old_e; ++old_i)
			{
				old_i->~value_type();
			}
		}
		imp.curr_ptr = imp.begin_ptr;
	}
	constexpr size_type size() const noexcept
	{
		return static_cast<size_type>(imp.curr_ptr - imp.begin_ptr);
	}
	constexpr size_type capacity() const noexcept
	{
		return static_cast<size_type>(imp.end_ptr - imp.begin_ptr);
	}

	[[nodiscard]] static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(value_type)};
		return mx;
	}
	[[nodiscard]] constexpr const_reference index_unchecked(size_type pos) const noexcept
	{
		return imp.begin_ptr[pos];
	}
	[[nodiscard]] constexpr reference index_unchecked(size_type pos) noexcept
	{
		return imp.begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] constexpr const_reference
	operator[](size_type pos) const noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (static_cast<::std::size_t>(curr_ptr - begin_ptr) <= pos) [[unlikely]]
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
	[[nodiscard]] constexpr reference
	operator[](size_type pos) noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (static_cast<::std::size_t>(curr_ptr - begin_ptr) <= pos) [[unlikely]]
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
	[[nodiscard]] constexpr const_reference
	front() const noexcept
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
	[[nodiscard]] constexpr reference
	front() noexcept
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
	[[nodiscard]] constexpr const_reference
	back() const noexcept
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
	[[nodiscard]] constexpr reference
	back() noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return curr_ptr[-1];
	}

	[[nodiscard]] constexpr const_reference front_unchecked() const noexcept
	{
		return *imp.begin_ptr;
	}
	[[nodiscard]] constexpr reference front_unchecked() noexcept
	{
		return *imp.begin_ptr;
	}
	[[nodiscard]] constexpr const_reference back_unchecked() const noexcept
	{
		return imp.curr_ptr[-1];
	}
	[[nodiscard]] constexpr reference back_unchecked() noexcept
	{
		return imp.curr_ptr[-1];
	}

	[[nodiscard]] constexpr iterator begin() noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] constexpr iterator end() noexcept
	{
		return imp.curr_ptr;
	}
	[[nodiscard]] constexpr const_iterator begin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] constexpr const_iterator end() const noexcept
	{
		return imp.curr_ptr;
	}
	[[nodiscard]] constexpr const_iterator cbegin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] constexpr const_iterator cend() const noexcept
	{
		return imp.curr_ptr;
	}

	[[nodiscard]] constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{imp.curr_ptr};
	}
	[[nodiscard]] constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{imp.begin_ptr};
	}
	[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{imp.curr_ptr};
	}
	[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr};
	}
	[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{imp.curr_ptr};
	}
	[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr};
	}

	constexpr void clear_destroy() noexcept
	{
		this->destroy();
		imp = {};
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	constexpr reference emplace_back(Args &&...args) noexcept(noexcept(value_type(::std::forward<Args>(args)...)))
	{
		if (imp.curr_ptr == imp.end_ptr)
#if __has_cpp_attribute(unlikely)
			[[unlikely]]
#endif
		{
			grow_twice_impl();
		}
		auto p{::std::construct_at(imp.curr_ptr, ::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

private:
	constexpr pointer movebackward_common_impl(pointer iter) noexcept
	{
		if (imp.curr_ptr == imp.end_ptr) [[unlikely]]
		{
			auto idx{iter - imp.begin_ptr};
			this->grow_twice_impl();
			iter = imp.begin_ptr + idx;
		}
		++imp.curr_ptr;
		if (iter != imp.end_ptr) [[likely]]
		{
			::fast_io::containers::details::move_backward_construct(iter, imp.curr_ptr - 1, imp.curr_ptr);
			iter->~value_type();
		}
		return iter;
	}

public:
	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	constexpr iterator emplace(const_iterator iter, Args &&...args)
	{
#ifdef __cpp_if_consteval
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			auto beginptr{imp.begin_ptr};
			return ::std::construct_at(this->movebackward_common_impl(iter - beginptr + beginptr), ::std::forward<Args>(args)...);
		}
		else
		{
			return ::std::construct_at(this->movebackward_common_impl(const_cast<pointer>(iter)), ::std::forward<Args>(args)...);
		}
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	constexpr reference emplace_index(size_type idx, Args &&...args)
	{
		auto beginptr{imp.begin_ptr};
		size_type sz{static_cast<size_type>(imp.curr_ptr - beginptr)};
		if (sz < idx)
		{
			::fast_io::fast_terminate();
		}
		return *::std::construct_at(this->movebackward_common_impl(beginptr + idx), ::std::forward<Args>(args)...);
	}

	constexpr iterator insert(const_iterator iter, const_reference val)
	{
		return this->emplace(iter, val);
	}

	constexpr iterator insert(const_iterator iter, value_type &&val)
	{
		return this->emplace(iter, ::std::move(val));
	}

	constexpr reference insert_index(size_type idx, const_reference val)
	{
		return this->emplace_index(idx, val);
	}

	constexpr reference insert_index(size_type idx, value_type &&val)
	{
		return this->emplace_index(idx, val);
	}
};

template <typename T, typename allocator1, typename allocator2>
	requires ::std::equality_comparable<T>
constexpr bool operator==(vector<T, allocator1> const &lhs, vector<T, allocator2> const &rhs) noexcept
{
	return ::std::equal(lhs.imp.begin_ptr, lhs.imp.curr_ptr, rhs.imp.begin_ptr, rhs.imp.curr_ptr);
}

template <typename T, typename allocator1, typename allocator2>
	requires ::std::three_way_comparable<T>
constexpr auto operator<=>(vector<T, allocator1> const &lhs, vector<T, allocator2> const &rhs) noexcept
{
	return ::std::lexicographical_compare_three_way(lhs.imp.begin_ptr, lhs.imp.curr_ptr, rhs.imp.begin_ptr, rhs.imp.curr_ptr, ::std::compare_three_way{});
}

template <typename T, typename allocator>
constexpr void swap(vector<T, allocator> &lhs, vector<T, allocator> &rhs) noexcept
{
	lhs.swap(rhs);
}

} // namespace containers

namespace freestanding
{

template <typename T, typename Alloc>
struct is_trivially_relocatable<::fast_io::containers::vector<T, Alloc>>
{
	inline static constexpr bool value = true;
};

template <typename T, typename Alloc>
struct is_zero_default_constructible<::fast_io::containers::vector<T, Alloc>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io