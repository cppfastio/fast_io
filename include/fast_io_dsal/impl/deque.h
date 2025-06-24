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
	deque_control_block_common
{
	void **controller_ptr;
	void *begin_ptr, *curr_ptr, *end_ptr;
};

template <typename T>
struct deque_control_block
{
	T **controller_ptr;
	T *begin_ptr, *curr_ptr, *end_ptr;
};

inline constexpr ::std::size_t deque_block_size_shift{12};

inline constexpr ::std::size_t deque_block_size_common{static_cast<::std::size_t>(1) << deque_block_size_shift};

template <::std::size_t sz>
inline constexpr ::std::size_t deque_block_size{sz <= (deque_block_size_common / 16u) ? (deque_block_size_common / sz) : 16u};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	deque_controller_block_common
{
	using replacetype = char unsigned;
	void **controller_start_ptr;
	void **controller_start_reserved_ptr;
	void **controller_after_ptr;
	void **controller_after_reserved_ptr;
};

template <typename T>
struct deque_controller_block
{
	using replacetype = T;
	T **controller_start_ptr;
	T **controller_start_reserved_ptr;
	T **controller_after_ptr;
	T **controller_after_reserved_ptr;
};

template <typename T>
struct deque_controller
{
	using replacetype = T;
	using controlreplacetype = T *;
	::fast_io::containers::details::deque_control_block<T> front_block;
	::fast_io::containers::details::deque_control_block<T> back_block;
	::fast_io::containers::details::deque_controller_block<T> controller_block;
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	deque_controller_common
{
	using replacetype = char unsigned;
	using controlreplacetype = void *;
	::fast_io::containers::details::deque_control_block_common front_block;
	::fast_io::containers::details::deque_control_block_common back_block;
	::fast_io::containers::details::deque_controller_block_common controller_block;
};

template <typename T, bool isconst>
struct deque_iterator
{
	using value_type = T;
	using pointer = ::std::conditional_t<isconst, value_type const *, value_type *>;
	using const_pointer = value_type const *;

	using reference = ::std::conditional_t<isconst, value_type const &, value_type &>;
	using const_reference = value_type const &;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	deque_control_block<T> itercontent;

	inline constexpr deque_iterator &operator++() noexcept
	{
		if (++itercontent.curr_ptr == itercontent.end_ptr) [[unlikely]]
		{
			constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(value_type)>};
			itercontent.end_ptr = ((itercontent.curr_ptr = itercontent.begin_ptr = (*++itercontent.controller_ptr)) + blocksize);
		}
		return *this;
	}

	inline constexpr deque_iterator &operator--() noexcept
	{
		if (itercontent.curr_ptr == itercontent.begin_ptr) [[unlikely]]
		{
			constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(value_type)>};
			itercontent.end_ptr = itercontent.curr_ptr = ((itercontent.begin_ptr = (*--itercontent.controller_ptr)) + blocksize);
		}
		--itercontent.curr_ptr;
		return *this;
	}

	inline constexpr deque_iterator operator++(int) noexcept
	{
		auto temp(*this);
		++*this;
		return temp;
	}

	inline constexpr deque_iterator operator--(int) noexcept
	{
		auto temp(*this);
		--*this;
		return temp;
	}

	inline constexpr reference operator*() const noexcept
	{
		return *this->itercontent.curr_ptr;
	}

	inline constexpr pointer operator->() const noexcept
	{
		return this->itercontent.curr_ptr;
	}

	inline constexpr deque_iterator &operator+=(difference_type pos) noexcept
	{
		constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(value_type)>};
		constexpr size_type blocksizem1{blocksize - 1u};
		size_type unsignedpos{static_cast<size_type>(pos)};
		auto curr_ptr{this->itercontent.curr_ptr};
		auto controllerptr{this->itercontent.controller_ptr};
		decltype(curr_ptr) beginptr;
		if (pos < 0)
		{
			size_type diff{static_cast<size_type>(this->itercontent.end_ptr - curr_ptr)};
			constexpr size_type zero{};
			size_type abspos{static_cast<size_type>(zero - unsignedpos)};
			diff += abspos;
			this->itercontent.curr_ptr = (beginptr = *(controllerptr -= diff / blocksize)) + (blocksizem1 - diff % blocksize);
		}
		else
		{
			size_type diff{static_cast<size_type>(curr_ptr - this->itercontent.begin_ptr)};
			diff += unsignedpos;
			this->itercontent.curr_ptr = (beginptr = *(controllerptr += diff / blocksize)) + diff % blocksize;
		}
		this->itercontent.controller_ptr = controllerptr;
		this->itercontent.begin_ptr = beginptr;
		this->itercontent.end_ptr = beginptr + blocksize;
		return *this;
	}

	inline constexpr deque_iterator &operator-=(difference_type pos) noexcept
	{
		constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(value_type)>};
		constexpr size_type blocksizem1{blocksize - 1u};
		size_type unsignedpos{static_cast<size_type>(pos)};
		auto curr_ptr{this->itercontent.curr_ptr};
		auto controllerptr{this->itercontent.controller_ptr};
		decltype(curr_ptr) beginptr;
		if (pos < 0)
		{
			size_type diff{static_cast<size_type>(curr_ptr - this->itercontent.begin_ptr)};
			constexpr size_type zero{};
			size_type abspos{static_cast<size_type>(zero - unsignedpos)};
			diff += abspos;
			this->itercontent.curr_ptr = (beginptr = *(controllerptr += diff / blocksize)) + diff % blocksize;
		}
		else
		{
			size_type diff{static_cast<size_type>(this->itercontent.end_ptr - curr_ptr)};
			diff += unsignedpos;
			this->itercontent.curr_ptr = (beginptr = *(controllerptr -= diff / blocksize)) + (blocksizem1 - diff % blocksize);
		}
		this->itercontent.controller_ptr = controllerptr;
		this->itercontent.begin_ptr = beginptr;
		this->itercontent.end_ptr = beginptr + blocksize;
		return *this;
	}

	inline constexpr reference operator[](difference_type pos) const noexcept
	{
		constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(value_type)>};
		constexpr size_type blocksizem1{blocksize - 1u};
		size_type unsignedpos{static_cast<size_type>(pos)};
		auto curr_ptr{this->itercontent.curr_ptr};
		auto controllerptr{this->itercontent.controller_ptr};
		if (pos < 0)
		{
			size_type diff{static_cast<size_type>(this->itercontent.end_ptr - curr_ptr) - 1u};
			constexpr size_type zero{};
			size_type abspos{static_cast<size_type>(zero - unsignedpos)};
			diff += abspos;
			return (*(controllerptr - diff / blocksize))[blocksizem1 - diff % blocksize];
		}
		else
		{
			size_type diff{static_cast<size_type>(curr_ptr - this->itercontent.begin_ptr)};
			diff += unsignedpos;
			return controllerptr[diff / blocksize][diff % blocksize];
		}
	}

	inline constexpr operator deque_iterator<T, true>() const noexcept
		requires(!isconst)
	{
		return {this->itercontent};
	}
};

template <typename T, bool isconst>
inline constexpr ::fast_io::containers::details::deque_iterator<T, isconst> operator+(::fast_io::containers::details::deque_iterator<T, isconst> a, ::std::ptrdiff_t pos) noexcept
{
	return (a += pos);
}

template <typename T, bool isconst>
inline constexpr ::fast_io::containers::details::deque_iterator<T, isconst> operator+(::std::ptrdiff_t pos, ::fast_io::containers::details::deque_iterator<T, isconst> a) noexcept
{
	return (a += pos);
}

template <typename T, bool isconst>
inline constexpr ::fast_io::containers::details::deque_iterator<T, isconst> operator-(::fast_io::containers::details::deque_iterator<T, isconst> a, ::std::ptrdiff_t pos) noexcept
{
	return (a -= pos);
}

template <typename T>
inline constexpr ::std::ptrdiff_t deque_iter_difference_common(::fast_io::containers::details::deque_control_block<T> const &a, ::fast_io::containers::details::deque_control_block<T> const &b) noexcept
{
	::std::ptrdiff_t controllerdiff{a.controller_ptr - b.controller_ptr};
	constexpr ::std::ptrdiff_t blocksizedf{static_cast<::std::ptrdiff_t>(::fast_io::containers::details::deque_block_size<sizeof(T)>)};
	return controllerdiff * blocksizedf + (a.curr_ptr - b.begin_ptr) + (b.begin_ptr - b.curr_ptr);
}

template <typename T, bool isconst1, bool isconst2>
inline constexpr ::std::ptrdiff_t operator-(::fast_io::containers::details::deque_iterator<T, isconst1> const &a, ::fast_io::containers::details::deque_iterator<T, isconst2> const &b) noexcept
{
	return ::fast_io::containers::details::deque_iter_difference_common(a.itercontent, b.itercontent);
}

template <typename T, bool isconst1, bool isconst2>
inline constexpr bool operator==(::fast_io::containers::details::deque_iterator<T, isconst1> const &a, ::fast_io::containers::details::deque_iterator<T, isconst2> const &b) noexcept
{
	return a.itercontent.curr_ptr == b.itercontent.curr_ptr;
}

template <typename T, bool isconst1, bool isconst2>
inline constexpr auto operator<=>(::fast_io::containers::details::deque_iterator<T, isconst1> const &a, ::fast_io::containers::details::deque_iterator<T, isconst2> const &b) noexcept
{
	auto block3way{a.itercontent.controller_ptr <=> b.itercontent.controller_ptr};
	if (block3way == 0)
	{
		return a.itercontent.curr_ptr <=> b.itercontent.curr_ptr;
	}
	return block3way;
}

template <typename allocator, typename controllerblocktype>
inline constexpr void deque_destroy_controller(controllerblocktype *controllerptr) noexcept
{
	auto &controller{*controllerptr};
	if constexpr (allocator::has_deallocate)
	{
		allocator::deallocate(controller.controller_start_ptr);
	}
	else
	{
		::std::size_t n{static_cast<::std::size_t>(controller.controller_after_ptr - controller.controller_start_ptr) * sizeof(void *)};
		allocator::deallocate_n(controller.controller_start_ptr, n);
	}
	
	controller.controller_start_ptr = nullptr;
}

template <typename allocator, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common_align(controllerblocktype *controllerptr, ::std::size_t aligns, ::std::size_t totalsz) noexcept
{
	auto &controller_block{*controllerptr};
	for (auto i{controller_block.controller_start_reserved_ptr}, e{controller_block.controller_after_reserved_ptr}; i != e; ++i)
	{
		allocator::deallocate_aligned_n(*i, aligns, totalsz);
	}
	deque_destroy_controller<allocator>(controllerptr);
}

template <typename allocator, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common_no_align(controllerblocktype *controllerptr, ::std::size_t totalsz) noexcept
{
	auto &controller_block{*controllerptr};
	for (auto i{controller_block.controller_start_reserved_ptr}, e{controller_block.controller_after_reserved_ptr}; i != e; ++i)
	{
		allocator::deallocate_n(*i, totalsz);
	}
	deque_destroy_controller<allocator>(controllerptr);
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common_impl(controllerblocktype *controllerptr) noexcept
{
	constexpr ::std::size_t totalsz{sz * ::fast_io::containers::details::deque_block_size<sz>};
	if constexpr (align <= allocator::default_alignment)
	{
		::fast_io::containers::details::deque_destroy_trivial_common_no_align<allocator, controllerblocktype>(controllerptr, totalsz);
	}
	else
	{
		::fast_io::containers::details::deque_destroy_trivial_common_align<allocator, controllerblocktype>(controllerptr, align, totalsz);
	}
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common(controllerblocktype &controller) noexcept
{
	if (__builtin_is_constant_evaluated())
	{
		::fast_io::containers::details::deque_destroy_trivial_common_impl<allocator, align, sz, controllerblocktype>(__builtin_addressof(controller));
	}
	else
	{
		::fast_io::containers::details::deque_destroy_trivial_common_impl<allocator, align, sz, ::fast_io::containers::details::deque_controller_block_common>(
			reinterpret_cast<::fast_io::containers::details::deque_controller_block_common *>(__builtin_addressof(controller)));
	}
}

template <typename allocator, typename dequecontroltype, typename replacetype>
inline constexpr void deque_init_grow_common_controllerallocate_impl(dequecontroltype &controller, ::std::size_t total_block_size, ::std::size_t mid, replacetype *blockptr) noexcept
{
	using controlreplacetype = typename dequecontroltype::controlreplacetype;
	constexpr ::std::size_t allocatesize{sizeof(controlreplacetype) * 4};
	auto controllerstartptr{static_cast<controlreplacetype *>(allocator::allocate(allocatesize))};
	controller.controller_block.controller_start_ptr = controllerstartptr;
	controller.controller_block.controller_after_reserved_ptr = (controller.controller_block.controller_start_reserved_ptr = controller.back_block.controller_ptr = controller.front_block.controller_ptr = controllerstartptr + 1) + 1;
	controller.controller_block.controller_after_ptr = controllerstartptr + 3u;
	*controller.back_block.controller_ptr = blockptr;
	controller.front_block.begin_ptr = controller.back_block.begin_ptr = blockptr;
	controller.back_block.curr_ptr = controller.front_block.curr_ptr = blockptr + mid;
	controller.front_block.end_ptr = controller.back_block.end_ptr = blockptr + total_block_size;
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_init_grow_common_noalign_impl(dequecontroltype &controller, ::std::size_t total_block_size, ::std::size_t mid) noexcept
{
	::fast_io::containers::details::deque_init_grow_common_controllerallocate_impl<allocator>(controller, total_block_size, mid, static_cast<typename dequecontroltype::replacetype *>(allocator::allocate_zero(total_block_size)));
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_init_grow_common_align_impl(dequecontroltype &controller, ::std::size_t align, ::std::size_t total_block_size, ::std::size_t mid) noexcept
{
	::fast_io::containers::details::deque_init_grow_common_controllerallocate_impl<allocator>(controller, total_block_size, mid, static_cast<typename dequecontroltype::replacetype *>(allocator::allocate_zero_aligned(align, total_block_size)));
}

template <typename allocator, ::std::size_t align, ::std::size_t block_size, ::std::size_t mid, typename dequecontroltype>
inline constexpr void deque_init_grow_common(dequecontroltype &controller) noexcept
{
	if constexpr (align <= allocator::default_alignment)
	{
		::fast_io::containers::details::deque_init_grow_common_noalign_impl<allocator>(controller, block_size, mid);
	}
	else
	{
		::fast_io::containers::details::deque_init_grow_common_align_impl<allocator>(controller, align, block_size, mid);
	}
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_reallocate_controller_block_common_impl(dequecontroltype &controller) noexcept
{
	constexpr ::std::size_t max_size{SIZE_MAX / sizeof(void *) / 2u - 1u};

	::std::size_t old_size{static_cast<::std::size_t>(controller.controller_block.controller_after_ptr - controller.controller_block.controller_start_ptr)};

	if (old_size > max_size)
	{
		::fast_io::fast_terminate();
	}

	::std::size_t new_size{2 * old_size};
	::std::size_t start_reserved_index{static_cast<::std::size_t>(controller.controller_block.controller_start_reserved_ptr - controller.controller_block.controller_start_ptr)};
	::std::size_t after_reserved_index{static_cast<::std::size_t>(controller.controller_block.controller_after_reserved_ptr - controller.controller_block.controller_start_ptr)};
	::std::size_t front_block_index{static_cast<::std::size_t>(controller.front_block.controller_ptr - controller.controller_block.controller_start_ptr)};
	::std::size_t back_block_index{static_cast<::std::size_t>(controller.back_block.controller_ptr - controller.controller_block.controller_start_ptr)};

	auto block_temp{allocator::reallocate_n(controller.controller_block.controller_start_ptr, old_size * sizeof(void *), (new_size + 1u) * sizeof(void *))};

	controller.controller_block.controller_start_ptr = static_cast<decltype(controller.controller_block.controller_start_ptr)>(block_temp);

	controller.controller_block.controller_start_reserved_ptr = controller.controller_block.controller_start_ptr + start_reserved_index;
	controller.controller_block.controller_after_reserved_ptr = controller.controller_block.controller_start_ptr + after_reserved_index;
	controller.controller_block.controller_after_ptr = controller.controller_block.controller_start_ptr + new_size;
	controller.front_block.controller_ptr = controller.controller_block.controller_start_ptr + front_block_index;
	controller.back_block.controller_ptr = controller.controller_block.controller_start_ptr + back_block_index;
}

template <typename T>
inline constexpr T **make_blocks_balance(T **begin, T **end, T **b, T **e) noexcept
{
	::std::size_t external_diff{static_cast<::std::size_t>(end - begin)};
	::std::size_t internal_diff{static_cast<::std::size_t>(e - b)};

	if (external_diff - internal_diff == 1u)
	{
		if (begin == b)
		{
			T *t = end[-1];
			::std::copy(b, e, begin + 1);
			*begin = t;
			return begin + 1u;
		}
		else
		{
			T *t = *begin;
			::std::copy(b, e, begin);
			end[-1] = t;
			return begin;
		}
	}
	else
	{
		T **blocktemp{begin + (external_diff - internal_diff) / 2u};

		if (blocktemp < b)
		{
			for (::std::size_t i{}; i != internal_diff; ++i)
			{
				::std::swap(b[i], blocktemp[i]);
			}
		}
		else
		{
			for (::std::size_t i{internal_diff}; i--;)
			{
				::std::swap(b[i], blocktemp[i]);
			}
		}

		return blocktemp;
	}
}

template <typename dequecontroltype>
inline constexpr void deque_make_reserved_blocks_balance_common_impl(dequecontroltype &controller) noexcept
{
	::std::size_t diff{static_cast<::std::size_t>(controller.back_block.controller_ptr - controller.front_block.controller_ptr)};

	controller.front_block.controller_ptr =
		::fast_io::containers::details::make_blocks_balance(
			controller.controller_block.controller_start_reserved_ptr,
			controller.controller_block.controller_after_reserved_ptr,
			controller.front_block.controller_ptr,
			controller.back_block.controller_ptr + 1);

	controller.back_block.controller_ptr = controller.front_block.controller_ptr + diff;
}

template <typename dequecontroltype>
inline constexpr void deque_make_unreserved_blocks_balance_common_impl(dequecontroltype &controller) noexcept
{
	::std::size_t reserved_size{static_cast<::std::size_t>(controller.controller_block.controller_after_reserved_ptr - controller.controller_block.controller_start_reserved_ptr)};
	::std::size_t front_block_index{static_cast<::std::size_t>(controller.front_block.controller_ptr - controller.controller_block.controller_start_reserved_ptr)};
	::std::size_t back_block_index{static_cast<::std::size_t>(controller.back_block.controller_ptr - controller.controller_block.controller_start_reserved_ptr)};

	controller.controller_block.controller_start_reserved_ptr =
		::fast_io::containers::details::make_blocks_balance(
			controller.controller_block.controller_start_ptr,
			controller.controller_block.controller_after_ptr,
			controller.controller_block.controller_start_reserved_ptr,
			controller.controller_block.controller_after_reserved_ptr);

	controller.controller_block.controller_after_reserved_ptr = controller.controller_block.controller_start_reserved_ptr + reserved_size;
	controller.front_block.controller_ptr = controller.controller_block.controller_start_reserved_ptr + front_block_index;
	controller.back_block.controller_ptr = controller.controller_block.controller_start_reserved_ptr + back_block_index;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_grow_front_common_impl(dequecontroltype &controller) noexcept
{
	if (controller.controller_block.controller_start_ptr == nullptr) [[unlikely]]
	{
		constexpr ::std::size_t mid = block_size / sz / 2;
		::fast_io::containers::details::deque_init_grow_common<allocator, align, block_size, mid>(controller);

		if (controller.front_block.curr_ptr != controller.front_block.begin_ptr)
		{
			return;
		}
	}

	// has reserved block == fasle
	if (controller.front_block.controller_ptr == controller.controller_block.controller_start_reserved_ptr)
	{
		// could balance reserved blocks == false
		if (controller.controller_block.controller_after_reserved_ptr - controller.back_block.controller_ptr < 2u)
		{
			// has unreserved block == fasle
			if (controller.controller_block.controller_start_ptr == controller.controller_block.controller_start_reserved_ptr)
			{
				// could balance unreserved blocks == false
				if (controller.controller_block.controller_after_ptr == controller.controller_block.controller_after_reserved_ptr)
				{
					::fast_io::containers::details::deque_reallocate_controller_block_common_impl<allocator>(controller);
				}

				if (controller.controller_block.controller_start_ptr == controller.controller_block.controller_start_reserved_ptr)
				{
					::fast_io::containers::details::deque_make_unreserved_blocks_balance_common_impl(controller);
				}
			}

			void *block_temp{};

			if constexpr (align <= allocator::default_alignment)
			{
				block_temp = allocator::allocate(block_size);
			}
			else
			{
				block_temp = allocator::allocate_zero_aligned(align, block_size);
			}

			*--controller.controller_block.controller_start_reserved_ptr = reinterpret_cast<decltype(*controller.controller_block.controller_start_reserved_ptr)>(block_temp);
		}
		else
		{
			::fast_io::containers::details::deque_make_reserved_blocks_balance_common_impl(controller);
		}
	}

	controller.front_block.curr_ptr = controller.front_block.end_ptr = static_cast<dequecontroltype::replacetype *>(controller.front_block.begin_ptr = *--controller.front_block.controller_ptr) + block_size;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_grow_back_common_impl(dequecontroltype &controller) noexcept
{
	if (controller.controller_block.controller_start_ptr == nullptr) [[unlikely]]
	{
		constexpr ::std::size_t mid = block_size / sz / 2;
		::fast_io::containers::details::deque_init_grow_common<allocator, align, block_size, mid>(controller);
		return;
	}

	// has reserved block == fasle
	if (controller.controller_block.controller_after_reserved_ptr - controller.back_block.controller_ptr < 2u)
	{
		// could balance reserved blocks == false
		if (controller.controller_block.controller_start_reserved_ptr == controller.front_block.controller_ptr)
		{
			// has unreserved block == fasle
			if (controller.controller_block.controller_after_ptr == controller.controller_block.controller_after_reserved_ptr)
			{
				// could balance unreserved blocks == false
				if (controller.controller_block.controller_start_ptr == controller.controller_block.controller_start_reserved_ptr)
				{
					::fast_io::containers::details::deque_reallocate_controller_block_common_impl<allocator>(controller);
				}
				// has unreserved block == fasle (is same as pre step)
				if (controller.controller_block.controller_after_ptr == controller.controller_block.controller_after_reserved_ptr)
				{
					::fast_io::containers::details::deque_make_unreserved_blocks_balance_common_impl(controller);
				}
			}

			void *block_temp{};

			if constexpr (align <= allocator::default_alignment)
			{
				block_temp = allocator::allocate(block_size);
			}
			else
			{
				block_temp = allocator::allocate_zero_aligned(align, block_size);
			}

			*controller.controller_block.controller_after_reserved_ptr = reinterpret_cast<decltype(*controller.controller_block.controller_after_reserved_ptr)>(block_temp);
			++controller.controller_block.controller_after_reserved_ptr;
		}
		else
		{
			::fast_io::containers::details::deque_make_reserved_blocks_balance_common_impl(controller);
		}
	}

	controller.back_block.end_ptr = static_cast<dequecontroltype::replacetype *>(controller.back_block.curr_ptr = controller.back_block.begin_ptr = *++controller.back_block.controller_ptr) + block_size;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_grow_front_common(dequecontroltype &controller) noexcept
{
	::fast_io::containers::details::deque_grow_front_common_impl<allocator, align, sz, block_size>(controller);
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_grow_back_common(dequecontroltype &controller) noexcept
{
	::fast_io::containers::details::deque_grow_back_common_impl<allocator, align, sz, block_size>(controller);
}

} // namespace details

template <typename T, typename allocator>
class deque FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
public:
	using value_type = T;
	using pointer = value_type *;
	using reference = value_type &;
	using const_reference = value_type const &;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using iterator = ::fast_io::containers::details::deque_iterator<T, false>;
	using const_iterator = ::fast_io::containers::details::deque_iterator<T, true>;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

	::fast_io::containers::details::deque_controller<T> controller;
	static inline constexpr size_type block_size{::fast_io::containers::details::deque_block_size<sizeof(value_type)>};
	inline constexpr deque() noexcept
		: controller{{}, {}, {}}
	{}

	inline constexpr deque(deque const &) = delete;
	inline constexpr deque &operator=(deque const &) = delete;

	inline constexpr deque(deque &&) noexcept = default;
	inline constexpr deque &operator=(deque &&) noexcept = default;

private:
	static inline constexpr void destroy_block_element(pointer first, pointer last) noexcept
	{
		for (; first != last; ++first)
		{
			first->~value_type();
		}
	}

	inline constexpr void destroy_all_elements() noexcept
	{
		destroy_block_element(controller.front_block.curr_ptr, controller.front_block.end_ptr);
		auto front_controller_ptr{controller.front_block.controller_ptr};
		auto back_controller_ptr{controller.back_block.controller_ptr};
		if (front_controller_ptr != back_controller_ptr)
		{
			for (T **it{front_controller_ptr + 1}, **ed{back_controller_ptr}; it != ed; ++it)
			{
				T *blockptr{*it};
				destroy_block_element(blockptr, blockptr + block_size);
			}
		}
		destroy_block_element(controller.back_block.begin_ptr, controller.back_block.curr_ptr);
	}

	inline constexpr void destroy() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			this->destroy_all_elements();
		}
		::fast_io::containers::details::deque_destroy_trivial_common<allocator, alignof(value_type), sizeof(value_type)>(controller.controller_block);
	}

	[[deprecated]] inline constexpr void init_grow() noexcept
	{
		constexpr size_type mid{block_size >> 1u};
		if (__builtin_is_constant_evaluated())
		{
			::fast_io::containers::details::deque_init_grow_common<allocator, alignof(value_type), block_size, mid>(controller);
		}
		else
		{
			::fast_io::containers::details::deque_init_grow_common<allocator, alignof(value_type), sizeof(value_type) * block_size, sizeof(value_type) * mid>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(controller)));
		}
	}

#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr void grow_front() noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			::fast_io::containers::details::deque_grow_front_common<allocator, alignof(value_type), sizeof(value_type), block_size>(controller);
		}
		else
		{
			::fast_io::containers::details::deque_grow_front_common<allocator, alignof(value_type), sizeof(value_type), sizeof(value_type) * block_size>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(controller)));
		}
	}

#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr void grow_back() noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			::fast_io::containers::details::deque_grow_back_common<allocator, alignof(value_type), sizeof(value_type), block_size>(controller);
		}
		else
		{
			::fast_io::containers::details::deque_grow_back_common<allocator, alignof(value_type), sizeof(value_type), sizeof(value_type) * block_size>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(controller)));
		}
	}

	inline constexpr void front_backspace() noexcept
	{
		controller.front_block.end_ptr = (controller.front_block.curr_ptr = controller.front_block.begin_ptr = *++controller.front_block.controller_ptr) + block_size;
	}

	inline constexpr void back_backspace() noexcept
	{
		controller.back_block.curr_ptr = controller.back_block.end_ptr = (controller.back_block.begin_ptr = *--controller.back_block.controller_ptr) + block_size;
	}

public:
#if 0
	inline constexpr void clear() noexcept
	{
		if (controller.controller_block.start_ptr == controller.controller_block.start_ptr)
		{
			return;
		}
		this->destroy();
		size_type n{(controller.back_block.controller_ptr - controller.front_block.controller_ptr)};
		n >>= 1u;
		auto mid{controller.front_block.controller_ptr + n};
		*mid;
	}
#endif
	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_back(Args &&...args)
	{
		if (controller.back_block.curr_ptr == controller.back_block.end_ptr) [[unlikely]]
		{
			grow_back();
		}
		auto currptr{controller.back_block.curr_ptr};
		if constexpr (!::std::is_trivially_constructible_v<value_type>)
		{
			::std::construct_at(currptr, ::std::forward<Args>(args)...);
		}
		++controller.back_block.curr_ptr;
		return *currptr;
	}

	inline constexpr void push_back(value_type const &value)
	{
		this->emplace_back(value);
	}

	inline constexpr void push_back(value_type &&value)
	{
		this->emplace_back(::std::move(value));
	}

	inline constexpr void pop_back() noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		pop_back_unchecked();
	}

	inline constexpr void pop_back_unchecked() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy_at(controller.back_block.curr_ptr - 1);
		}
		if (--controller.back_block.curr_ptr == controller.back_block.begin_ptr) [[unlikely]]
		{
			this->back_backspace();
		}
	}

	inline constexpr reference back_unchecked() noexcept
	{
		return controller.back_block.curr_ptr[-1];
	}

	inline constexpr const_reference back_unchecked() const noexcept
	{
		return controller.back_block.curr_ptr[-1];
	}

	inline constexpr reference back() noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		return controller.back_block.curr_ptr[-1];
	}

	inline constexpr const_reference back() const noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		return controller.back_block.curr_ptr[-1];
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_front(Args &&...args)
	{
		if (controller.front_block.curr_ptr == controller.front_block.begin_ptr) [[unlikely]]
		{
			grow_front();
		}
		if constexpr (!::std::is_trivially_constructible_v<value_type>)
		{
			::std::construct_at(--controller.front_block.curr_ptr, ::std::forward<Args>(args)...);
		}
		return *controller.front_block.curr_ptr;
	}

	inline constexpr void push_front(value_type const &value)
	{
		this->emplace_front(value);
	}

	inline constexpr void push_front(value_type &&value)
	{
		this->emplace_front(::std::move(value));
	}

	inline constexpr void pop_front() noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		pop_front_unchecked();
	}

	inline constexpr void pop_front_unchecked() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy_at(controller.front_block.curr_ptr);
		}

		if (++controller.front_block.curr_ptr == controller.front_block.end_ptr) [[unlikely]]
		{
			this->front_backspace();
		}
	}

	inline constexpr reference front_unchecked() noexcept
	{
		return *controller.front_block.curr_ptr;
	}

	inline constexpr const_reference front_unchecked() const noexcept
	{
		return *controller.front_block.curr_ptr;
	}

	inline constexpr reference front() noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		return *controller.front_block.curr_ptr;
	}

	inline constexpr const_reference front() const noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		return *controller.front_block.curr_ptr;
	}

	inline constexpr reference operator[](size_type index) noexcept
	{
		if (size() <= index) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		size_type real_index{static_cast<size_type>(controller.front_block.curr_ptr - controller.front_block.begin_ptr) + index};

		return controller.front_block.controller_ptr[real_index / block_size][real_index % block_size];
	}

	inline constexpr const_reference operator[](size_type index) const noexcept
	{
		if (size() <= index) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		size_type real_index{static_cast<size_type>(controller.front_block.curr_ptr - controller.front_block.begin_ptr) + index};

		return controller.front_block.controller_ptr[real_index / block_size][real_index % block_size];
	}

	inline constexpr reference index_unchecked(size_type index) noexcept
	{
		size_type real_index{static_cast<size_type>(controller.front_block.curr_ptr - controller.front_block.begin_ptr) + index};

		return controller.front_block.controller_ptr[real_index / block_size][real_index % block_size];
	}

	inline constexpr const_reference index_unchecked(size_type index) const noexcept
	{
		size_type real_index{static_cast<size_type>(controller.front_block.curr_ptr - controller.front_block.begin_ptr) + index};

		return controller.front_block.controller_ptr[real_index / block_size][real_index % block_size];
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mxval{SIZE_MAX / sizeof(value_type)};
		return mxval;
	}

	static inline constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mxval{SIZE_MAX / sizeof(value_type) * sizeof(value_type)};
		return mxval;
	}

	inline constexpr size_type size() const noexcept
	{
		return block_size * static_cast<size_type>(controller.back_block.controller_ptr - controller.front_block.controller_ptr) + static_cast<size_type>((controller.back_block.curr_ptr - controller.back_block.begin_ptr) + (controller.front_block.begin_ptr - controller.front_block.curr_ptr));
	}

	inline constexpr size_type size_bytes() const noexcept
	{
		return size() * sizeof(value_type);
	}

	inline constexpr iterator begin() noexcept
	{
		return {this->controller.front_block};
	}

	inline constexpr const_iterator begin() const noexcept
	{
		return {this->controller.front_block};
	}

	inline constexpr const_iterator cbegin() const noexcept
	{
		return {this->controller.front_block};
	}

	inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator({this->controller.front_block});
	}

	inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator({this->controller.front_block});
	}

	inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator({this->controller.front_block});
	}

private:
	inline constexpr ::fast_io::containers::details::deque_control_block<value_type> end_common() noexcept
	{
		::fast_io::containers::details::deque_control_block<value_type> backblock{this->controller.back_block};
		if (backblock.curr_ptr == backblock.end_ptr) [[unlikely]]
		{
			if (backblock.controller_ptr) [[likely]]
			{
				backblock.end_ptr = ((backblock.curr_ptr = backblock.begin_ptr = (*++backblock.controller_ptr)) + block_size);
			}
		}
		return {backblock};
	}

	inline constexpr ::fast_io::containers::details::deque_control_block<value_type> end_common() const noexcept
	{
		::fast_io::containers::details::deque_control_block<value_type> backblock{this->controller.back_block};
		if (backblock.curr_ptr == backblock.end_ptr) [[unlikely]]
		{
			if (backblock.controller_ptr) [[likely]]
			{
				backblock.end_ptr = ((backblock.curr_ptr = backblock.begin_ptr = (*++backblock.controller_ptr)) + block_size);
			}
		}
		return {backblock};
	}

public:
	inline constexpr iterator end() noexcept
	{
		return {this->end_common()};
	}

	inline constexpr const_iterator end() const noexcept
	{
		return {this->end_common()};
	}

	inline constexpr const_iterator cend() const noexcept
	{
		return {this->end_common()};
	}

	inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator({this->end_common()});
	}

	inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator({this->end_common()});
	}

	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator({this->end_common()});
	}

	inline constexpr bool empty() const noexcept
	{
		return controller.front_block.curr_ptr == controller.back_block.curr_ptr;
	}

	inline constexpr bool is_empty() const noexcept
	{
		return controller.front_block.curr_ptr == controller.back_block.curr_ptr;
	}

	inline constexpr void clear_destroy() noexcept
	{
		this->destroy();
		this->controller = {{}, {}, {}};
	}

	inline constexpr ~deque()
	{
		this->destroy();
	}
};

template <typename T, typename allocator1, typename allocator2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(deque<T, allocator1> const &lhs, deque<T, allocator2> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

#if defined(__cpp_lib_three_way_comparison)

template <typename T, typename allocator1, typename allocator2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(deque<T, allocator1> const &lhs, deque<T, allocator2> const &rhs) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), ::std::compare_three_way{});
}

#endif

} // namespace containers

namespace freestanding
{

template <typename T, typename allocator>
struct is_trivially_copyable_or_relocatable<::fast_io::containers::deque<T, allocator>>
{
	inline static constexpr bool value = true;
};

template <typename T, typename allocator>
struct is_zero_default_constructible<::fast_io::containers::deque<T, allocator>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io
