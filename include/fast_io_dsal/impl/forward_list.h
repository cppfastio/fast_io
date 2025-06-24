#pragma once

namespace fast_io
{

namespace containers
{

namespace details
{

struct forward_list_node_common
{
	void *next;
};

template <typename T>
struct forward_list_node
{
	void *next;
	T element;
};

template <typename T, bool isconst>
class forward_list_iterator
{
public:
	using value_type = T;
	using pointer = ::std::conditional_t<isconst, value_type const *, value_type *>;
	using const_pointer = value_type const *;

	using reference = ::std::conditional_t<isconst, value_type const &, value_type &>;
	using const_reference = value_type const &;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	void *iter{};

	inline constexpr forward_list_iterator &operator++() noexcept
	{
		iter = *static_cast<void **>(iter);
		return *this;
	}

	inline constexpr forward_list_iterator operator++(int) noexcept
	{
		auto temp(*this);
		++*this;
		return temp;
	}

	inline constexpr reference operator*() const noexcept
	{
		return static_cast<forward_list_node<T> *>(iter)->element;
	}

	inline constexpr pointer operator->() const noexcept
	{
		return __builtin_addressof(static_cast<forward_list_node<T> *>(iter)->element);
	}

	inline constexpr operator forward_list_iterator<T, true>() const noexcept
		requires(!isconst)
	{
		return {this->iter};
	}
};

template <typename T, bool isconst1, bool isconst2>
inline constexpr bool operator==(forward_list_iterator<T, isconst1> a, forward_list_iterator<T, isconst2> b) noexcept
{
	return a.iter == b.iter;
}

template <typename T, bool isconst1, bool isconst2>
inline constexpr bool operator!=(forward_list_iterator<T, isconst1> a, forward_list_iterator<T, isconst2> b) noexcept
{
	return a.iter != b.iter;
}

template <typename allocator>
inline void forward_list_trivially_destroy(void *first, void *last, ::std::size_t sz)
{
	for (void *it{first}; it != last;)
	{
		auto next{*static_cast<void **>(it)};
		generic_allocator_adapter<allocator>::deallocate_n(it, sz);
		it = next;
	}
}

template <typename allocator>
inline void forward_list_trivially_destroy_aligned(void *first, void *last,
												   ::std::size_t align, ::std::size_t sz)
{
	for (void *it{first}; it != last;)
	{
		auto next{*static_cast<void **>(it)};
		generic_allocator_adapter<allocator>::deallocate_aligned_n(it, align, sz);
		it = next;
	}
}

template <typename allocator, ::std::size_t align, ::std::size_t sz>
inline void forward_list_trivially_destroy_sa(void *first, void *last)
{
	if constexpr (align <= allocator::default_alignment)
	{
		::fast_io::containers::details::forward_list_trivially_destroy<allocator>(first, last, sz);
	}
	else
	{
		::fast_io::containers::details::forward_list_trivially_destroy_aligned<allocator>(first, last, align, sz);
	}
}

inline constexpr void forward_list_main_insert_after_ptr_common(void *newnodevp, void *iter) noexcept
{
	auto newnode(static_cast<::fast_io::containers::details::forward_list_node_common *>(newnodevp));
	auto node = static_cast<::fast_io::containers::details::forward_list_node_common *>(iter);
	newnode->next = node->next;
	node->next = newnode;
}

inline constexpr void forward_list_main_push_front_ptr_common(void *newnodevp, void *imp) noexcept
{
	auto newnode(static_cast<::fast_io::containers::details::forward_list_node_common *>(newnodevp));
	newnode->next = imp;
}

inline constexpr void forward_list_main_erase_after_ptr_common(void *iter) noexcept
{
	auto node = static_cast<::fast_io::containers::details::forward_list_node_common *>(iter);
	auto next = static_cast<::fast_io::containers::details::forward_list_node_common *>(node);
	node->next = next->next;
}

template <typename allocator>
inline constexpr void *forward_list_trivially_allocate_insert_after(void *iter, ::std::size_t sz) noexcept
{
	void *newnode = generic_allocator_adapter<allocator>::allocate(sz);
	forward_list_main_insert_after_ptr_common(newnode, iter);
	return newnode;
}

template <typename allocator>
inline constexpr void *forward_list_trivially_allocate_insert_after_aligned(void *iter, ::std::size_t align, ::std::size_t sz) noexcept
{
	void *newnode = generic_allocator_adapter<allocator>::allocate_aligned(align, sz);
	forward_list_main_insert_after_ptr_common(newnode, iter);
	return newnode;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz>
inline constexpr void *forward_list_trivially_allocate_insert_after_sa(void *iter) noexcept
{
	if constexpr (align <= allocator::default_alignment)
	{
		return ::fast_io::containers::details::forward_list_trivially_allocate_insert_after<allocator>(iter, sz);
	}
	else
	{
		return ::fast_io::containers::details::forward_list_trivially_allocate_insert_after_aligned<allocator>(iter, align, sz);
	}
}

inline constexpr void forward_list_splice_before_after_iter(void *posptr, void *beforefirstptr) noexcept
{
	auto pos = static_cast<::fast_io::containers::details::forward_list_node_common *>(posptr);
	auto beforefirst = static_cast<::fast_io::containers::details::forward_list_node_common *>(beforefirstptr);
	auto posnext = static_cast<::fast_io::containers::details::forward_list_node_common *>(pos->next);
	auto first = static_cast<::fast_io::containers::details::forward_list_node_common *>(beforefirst->next);
	auto firstnext = static_cast<::fast_io::containers::details::forward_list_node_common *>(first->next);
	pos->next = firstnext;
	beforefirst->next = firstnext;
	first->next = posnext;
}

inline constexpr void forward_list_splice_before_after_range_common(void *posptr, void *beforefirstptr, void *beforelastptr) noexcept
{
	if (beforefirstptr == beforelastptr) [[unlikely]]
	{
		return;
	}
	auto pos = static_cast<::fast_io::containers::details::forward_list_node_common *>(posptr);
	auto beforefirst = static_cast<::fast_io::containers::details::forward_list_node_common *>(beforefirstptr);
	auto beforelast = static_cast<::fast_io::containers::details::forward_list_node_common *>(beforelastptr);
	auto posnext = static_cast<::fast_io::containers::details::forward_list_node_common *>(pos->next);
	auto first = static_cast<::fast_io::containers::details::forward_list_node_common *>(beforefirst->next);
	auto last = static_cast<::fast_io::containers::details::forward_list_node_common *>(beforelast->next);
	beforefirst->next = last;
	beforelast->next = posnext;
	pos->next = first;
}

inline constexpr ::std::size_t forward_list_ptr_distance(void *firstptr) noexcept
{
	::std::size_t n{};
	for (; firstptr; firstptr = static_cast<::fast_io::containers::details::forward_list_node_common *>(firstptr)->next)
	{
		++n;
	}
	return n;
}

inline constexpr void *forward_list_ptr_advance(void *firstptr, ::std::size_t n) noexcept
{
	for (; n; --n)
	{
		firstptr = static_cast<::fast_io::containers::details::forward_list_node_common *>(firstptr)->next;
	}
	return firstptr;
}

template <typename T, typename Cmp>
inline constexpr void *forward_list_merge_common(void *b1, void *b2, Cmp cmp) noexcept
{
	auto bb1{static_cast<fast_io::containers::details::forward_list_node<T> *>(b1)};
	auto bb2{static_cast<fast_io::containers::details::forward_list_node<T> *>(b2)};
	void *dummy{};
	void *tail{__builtin_addressof(dummy)};
	while (bb1 && bb2)
	{
		if (cmp(bb1->element, bb2->element))
		{
			static_cast<::fast_io::containers::details::forward_list_node_common *>(tail)->next = bb1;
			bb1 = static_cast<fast_io::containers::details::forward_list_node<T> *>(bb1->next);
		}
		else
		{
			static_cast<::fast_io::containers::details::forward_list_node_common *>(tail)->next = bb2;
			bb2 = static_cast<fast_io::containers::details::forward_list_node<T> *>(bb2->next);
		}
		tail = static_cast<::fast_io::containers::details::forward_list_node_common *>(tail)->next;
	}
	static_cast<::fast_io::containers::details::forward_list_node_common *>(tail)->next = bb1 ? bb1 : bb2;
	return dummy;
}

inline constexpr void *forward_list_ptr_advance_remove(void *firstptr, ::std::size_t n) noexcept
{
	for (; n; --n)
	{
		firstptr = static_cast<::fast_io::containers::details::forward_list_node_common *>(firstptr)->next;
	}
	return firstptr;
}

template <typename T, typename Cmp>
inline void *forward_list_sort_common_n(void *firstptr, ::std::size_t n, Cmp cmp) noexcept
{
	switch (n)
	{
	case 2:
	{
		auto it1{static_cast<::fast_io::containers::details::forward_list_node<T> *>(firstptr)};
		auto it2{static_cast<::fast_io::containers::details::forward_list_node<T> *>(it1->next)};
		if (cmp(it2->element, it1->element))
		{
			it1->next = it2->next;
			it2->next = it1;
			it1 = it2;
		}
		return it1;
	}
		[[fallthrough]];
	case 1:
		[[fallthrough]];
	case 0:
		return firstptr;
	default:
	{
		::std::size_t const mid{static_cast<::std::size_t>(n >> 1u)};
		::std::size_t const midm1{static_cast<::std::size_t>(mid - 1u)};
		auto beforemidptr{::fast_io::containers::details::forward_list_ptr_advance(firstptr, midm1)};
		auto midptr{static_cast<::fast_io::containers::details::forward_list_node_common *>(beforemidptr)->next};
		static_cast<::fast_io::containers::details::forward_list_node_common *>(beforemidptr)->next = nullptr;

		firstptr = ::fast_io::containers::details::forward_list_sort_common_n<T, Cmp>(firstptr, mid, cmp);
		midptr = ::fast_io::containers::details::forward_list_sort_common_n<T, Cmp>(midptr, static_cast<::std::size_t>(n - mid), cmp);
		return ::fast_io::containers::details::forward_list_merge_common<T, Cmp>(firstptr, midptr, cmp);
	}
	}
}

template <typename T, typename Cmp>
inline constexpr void *forward_list_sort_common(void *firstptr, Cmp cmp) noexcept
{
	return ::fast_io::containers::details::forward_list_sort_common_n<T, Cmp>(firstptr,
																			  ::fast_io::containers::details::forward_list_ptr_distance(firstptr), cmp);
}

template <typename T, typename Cmp>
inline constexpr void forward_list_sort_common_before(void *beforefirstptr, Cmp cmp) noexcept
{
	auto bf{static_cast<::fast_io::containers::details::forward_list_node_common *>(beforefirstptr)};
	bf->next = ::fast_io::containers::details::forward_list_sort_common<T, Cmp>(bf->next, cmp);
}

inline constexpr void *forward_list_reverse_common(void *firstptr) noexcept
{
	void *prev{};
	for (auto it{firstptr}; it;)
	{
		auto next{static_cast<::fast_io::containers::details::forward_list_node_common *>(it)->next};
		static_cast<::fast_io::containers::details::forward_list_node_common *>(it)->next = prev;
		prev = it;
		it = next;
	}
	return prev;
}

inline constexpr void forward_list_reverse_common_before(void *beforefirstptr) noexcept
{
	auto bf{static_cast<::fast_io::containers::details::forward_list_node_common *>(beforefirstptr)};
	bf->next = forward_list_reverse_common(bf->next);
}

} // namespace details

template <typename T, typename allocator>
class forward_list
{
public:
	using allocator_type = allocator;
	using value_type = T;

private:
	using node_type = ::fast_io::containers::details::forward_list_node<value_type>;
	using untyped_allocator_type = generic_allocator_adapter<allocator_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<untyped_allocator_type, node_type>;
	static inline constexpr bool alloc_with_status{untyped_allocator_type::has_status};
	using handle_type = ::std::conditional_t<alloc_with_status, typename untyped_allocator_type::handle_type, allocator_type>;
	using handle_holder_type = ::fast_io::containers::details::handle_holder<handle_type>;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	handle_holder_type allochdl;

	inline constexpr auto allocate_new_raw_node() noexcept
	{
		if constexpr (alloc_with_status)
		{
			return typed_allocator_type::handle_allocate(allochdl.get(), 1);
		}
		else
		{
			return typed_allocator_type::allocate(1);
		}
	}

	struct new_handle_guard
	{
		::fast_io::containers::details::forward_list_node<value_type> *ptr;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
		[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
		[[no_unique_address]]
#endif
#endif
		::std::conditional_t<alloc_with_status, forward_list<T, allocator> *, allocator> v;
		inline explicit constexpr new_handle_guard(::fast_io::containers::details::forward_list_node<value_type> *p,
												   forward_list<T, allocator> *va)
		{
			ptr = p;
			if constexpr (alloc_with_status)
			{
				v = va;
			}
		}
		inline new_handle_guard(new_handle_guard const &) = delete;
		inline new_handle_guard &operator=(new_handle_guard const &) = delete;
		inline constexpr ~new_handle_guard()
		{
			if (ptr == nullptr)
			{
				return;
			}
			if constexpr (alloc_with_status)
			{
				typed_allocator_type::handle_deallocate(v->allochdl.get(), v, 1);
			}
			else
			{
				typed_allocator_type::deallocate(ptr, 1);
				ptr = nullptr;
			}
		}
	};
	struct forward_list_destroyer
	{
		forward_list<T, allocator> *plst;
		inline explicit constexpr forward_list_destroyer(forward_list<T, allocator> *pl) noexcept
			: plst(pl)
		{}
		inline constexpr void release() noexcept
		{
			plst = nullptr;
		}
		inline forward_list_destroyer(forward_list_destroyer const &) = delete;
		inline forward_list_destroyer &operator=(forward_list_destroyer const &) = delete;
		inline constexpr ~forward_list_destroyer()
		{
			if (plst == nullptr)
			{
				return;
			}
			plst->destroy();
		}
	};

	inline constexpr void destroy_node(void *it) noexcept
	{
		auto node = static_cast<node_type *>(it);
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			(__builtin_addressof(node->element))->~value_type();
		}
		if constexpr (alloc_with_status)
		{
			typed_allocator_type::handle_deallocate_n(allochdl.get(), node, 1);
		}
		else
		{
			typed_allocator_type::deallocate_n(node, 1);
		}
	}

	inline constexpr void destroy_impl(void *first, void *last) noexcept
	{
#if __cpp_if_consteval >= 202106L
		if !consteval
#else
		if (!__builtin_is_constant_evaluated())
#endif
		{
			if constexpr (::std::is_trivially_destructible_v<value_type> && !alloc_with_status)
			{
				::fast_io::containers::details::forward_list_trivially_destroy_sa<allocator_type,
																				  alignof(node_type), sizeof(node_type)>(first, last);
				return;
			}
		}
		for (void *it{first}; it != last;)
		{
			auto next{*static_cast<void **>(it)};
			this->destroy_node(it);
			it = next;
		}
	}

	inline constexpr void destroy() noexcept
	{
		this->destroy_impl(this->imp, nullptr);
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr auto allocate_construct_new_node(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		auto newnode = allocate_new_raw_node();
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			::std::construct_at(
				__builtin_addressof(newnode->element),
				::std::forward<Args>(args)...);
		}
		else
		{
			new_handle_guard guard(newnode, this);
			::std::construct_at(
				__builtin_addressof(newnode->element),
				::std::forward<Args>(args)...);
			guard.ptr = nullptr;
		}
		return newnode;
	}

public:
	using pointer = value_type *;
	using const_pointer = value_type const *;

	using reference = value_type &;
	using const_reference = value_type const &;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	using iterator = ::fast_io::containers::details::forward_list_iterator<T, false>;
	using const_iterator = ::fast_io::containers::details::forward_list_iterator<T, true>;

	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

	node_type *imp{};
	inline constexpr forward_list() noexcept = default;

private:
	template <typename Iter, typename Sentinel>
	inline constexpr void forward_list_range_init_common(Iter first, Sentinel last)
	{
		forward_list_destroyer destroyer(this);
		void *beforeit{__builtin_addressof(this->imp)};
		for (; first != last; ++first)
		{
			beforeit = this->emplace_after_impl(beforeit, *first);
		}
		destroyer.release();
	}

public:
	template <std::ranges::input_range Rg>
	inline explicit constexpr forward_list(::fast_io::freestanding::from_range_t, Rg &&rg)
	{
		if constexpr (::std::ranges::contiguous_range<Rg>)
		{
			this->forward_list_range_init_common(::std::to_address(::std::ranges::cbegin(rg)), ::std::to_address(::std::ranges::cend(rg)));
		}
		else
		{
			this->forward_list_range_init_common(::std::ranges::cbegin(rg), ::std::ranges::cend(rg));
		}
	}

	inline explicit constexpr forward_list(::std::initializer_list<value_type> ilist)
		: forward_list(::fast_io::freestanding::from_range, ilist)
	{
	}

	inline explicit constexpr forward_list(::std::size_t n, const_reference r) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		forward_list_destroyer destroyer(this);
		void *beforeit{__builtin_addressof(this->imp)};
		for (::std::size_t i{}; i != n; ++i)
		{
			beforeit = this->emplace_after_impl(beforeit, r);
		}
		destroyer.release();
	}

	inline explicit constexpr forward_list(::std::size_t n) noexcept(::std::is_nothrow_default_constructible_v<value_type> && ::std::is_nothrow_copy_constructible_v<value_type>)
		: forward_list(n, value_type())
	{
	}

	inline constexpr ~forward_list()
	{
		this->destroy();
	}

	[[nodiscard]] inline constexpr iterator before_begin() noexcept
	{
		return {__builtin_addressof(imp)};
	}

	[[nodiscard]] inline constexpr const_iterator before_begin() const noexcept
	{
		return {const_cast<void *>(static_cast<void const *>(__builtin_addressof(this->imp)))};
	}

	[[nodiscard]] inline constexpr const_iterator cbefore_begin() const noexcept
	{
		return {const_cast<void *>(static_cast<void const *>(__builtin_addressof(this->imp)))};
	}

	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return {imp};
	}

	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return {imp};
	}

	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return {imp};
	}

	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return {nullptr};
	}

	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return {nullptr};
	}

	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return {nullptr};
	}

	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return this->imp == nullptr;
	}
	[[nodiscard]] inline constexpr bool is_empty() const noexcept
	{
		return this->imp == nullptr;
	}

	[[nodiscard]] inline constexpr reference front() noexcept
	{
		if (this->imp == nullptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return this->imp->element;
	}
	[[nodiscard]] inline constexpr const_reference front() const noexcept
	{
		if (this->imp == nullptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return this->imp->element;
	}

	[[nodiscard]] inline constexpr reference front_unchecked() noexcept
	{
		return this->imp->element;
	}
	[[nodiscard]] inline constexpr const_reference front_unchecked() const noexcept
	{
		return this->imp->element;
	}

private:
	inline constexpr void erase_after_impl(void *first, void *last) noexcept
	{
		auto node = static_cast<::fast_io::containers::details::forward_list_node_common *>(first);
		auto next = static_cast<::fast_io::containers::details::forward_list_node_common *>(node)->next;
		node->next = last;
		this->destroy_impl(next, last);
	}
	inline constexpr void erase_after_impl(void *ptr) noexcept
	{
		auto node = static_cast<::fast_io::containers::details::forward_list_node_common *>(ptr);
		auto next = static_cast<::fast_io::containers::details::forward_list_node_common *>(node)->next;
		auto nextnext = static_cast<::fast_io::containers::details::forward_list_node_common *>(next)->next;
		node->next = nextnext;
		this->destroy_node(next);
	}

public:
	inline constexpr void erase_after(const_iterator iter) noexcept
	{
		this->erase_after_impl(iter.iter);
	}

	inline constexpr void erase_after(const_iterator first, const_iterator last) noexcept
	{
		this->erase_after_impl(first.iter, last.iter);
	}

	inline constexpr void pop_front_unchecked() noexcept
	{
		this->erase_after_impl(__builtin_addressof(this->imp));
	}

	inline constexpr void pop_front() noexcept
	{
		if (this->imp == nullptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		this->erase_after_impl(__builtin_addressof(this->imp));
	}

private:
	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr node_type *emplace_after_impl(void *ptr, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			auto newnode = static_cast<node_type *>(::fast_io::containers::details::forward_list_trivially_allocate_insert_after_sa<allocator_type, alignof(node_type), sizeof(node_type)>(ptr));
			::std::construct_at(
				__builtin_addressof(newnode->element),
				::std::forward<Args>(args)...);
			return newnode;
		}
		else
		{
			auto newnodecons = this->allocate_construct_new_node(::std::forward<Args>(args)...);
			::fast_io::containers::details::forward_list_main_insert_after_ptr_common(newnodecons, ptr);
			return newnodecons;
		}
	}

public:
	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr iterator emplace_after(const_iterator iter, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		return {this->emplace_after_impl(iter.iter, ::std::forward<Args>(args)...)};
	}

	inline constexpr iterator insert_after(const_iterator iter, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		return {this->emplace_after_impl(iter.iter, val)};
	}

	inline constexpr iterator insert_after(const_iterator iter, value_type &&val) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		return {this->emplace_after_impl(iter.iter, ::std::move(val))};
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_front(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		return this->emplace_after_impl(__builtin_addressof(this->imp), ::std::forward<Args>(args)...)->element;
	}
	inline constexpr void push_front(const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->emplace_front(val);
	}
	inline constexpr void push_front(value_type &&val) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		this->emplace_front(::std::move(val));
	}

	inline constexpr forward_list(forward_list const &other)
		requires(::std::copyable<value_type>)
	{
		forward_list_destroyer destroyer(this);
		void *itt{__builtin_addressof(this->imp)};
		node_type *it{static_cast<node_type *>(itt)};
		for (auto const &ele : other)
		{
			it = this->emplace_after_impl(it, ele);
		}
		destroyer.release();
	}
	inline constexpr forward_list(forward_list const &) = delete;

	inline constexpr forward_list &operator=(forward_list const &other)
		requires(::std::copyable<value_type>)
	{
		if (__builtin_addressof(other) != this) [[likely]]
		{
			forward_list temp(other);
			this->imp = temp.imp;
			temp.imp = nullptr;
		}
		return *this;
	}
	inline constexpr forward_list &operator=(forward_list const &) = delete;

	inline constexpr forward_list(forward_list &&other) noexcept
		: imp(other.imp)
	{
		other.imp = nullptr;
	}

	inline constexpr forward_list &operator=(forward_list &&other) noexcept
	{
		if (__builtin_addressof(other) != this) [[likely]]
		{
			this->destroy();
			this->imp = other.imp;
			other.imp = nullptr;
		}
		return *this;
	}

	inline constexpr void splice_before_after(const_iterator pos, const_iterator beforeit) noexcept
	{
		::fast_io::containers::details::forward_list_splice_before_after_iter(pos.iter, beforeit.iter);
	}
	inline constexpr void splice_before_after(const_iterator pos, const_iterator beforefirst, const_iterator beforelast) noexcept
	{
		::fast_io::containers::details::forward_list_splice_before_after_range_common(pos.iter, beforefirst.iter, beforelast.iter);
	}
#if 0
	inline constexpr void reverse_after(const_iterator beforefirst, const_iterator last) noexcept
	{
		::fast_io::containers::details::forward_list_reverse_after_common(beforefirst.iter, last.iter);
	}
#endif
	inline constexpr void reverse() noexcept
	{
		::fast_io::containers::details::forward_list_reverse_common_before(__builtin_addressof(this->imp));
	}
	inline constexpr void reverse_after(const_iterator beforeit) noexcept
	{
		::fast_io::containers::details::forward_list_reverse_common_before(beforeit->iter);
	}
	template <typename Cmp>
	inline constexpr void sort(Cmp cmp) noexcept
	{
		::fast_io::containers::details::forward_list_sort_common_before<value_type, Cmp>(
			__builtin_addressof(this->imp), cmp);
	}

	inline constexpr void sort() noexcept
	{
		this->sort(::std::ranges::less{});
	}

	template <typename Cmp>
	inline constexpr void sort_after(const_iterator beforeit, Cmp cmp) noexcept
	{
		::fast_io::containers::details::forward_list_sort_common_before<value_type, Cmp>(
			beforeit->next, cmp);
	}

	inline constexpr void sort_after(const_iterator beforeit) noexcept
	{
		this->sort_after(beforeit, ::std::ranges::less{});
	}

	template <typename Cmp>
	inline constexpr void merge(forward_list &&other, Cmp cmp) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return;
		}
		this->imp = static_cast<node_type *>(::fast_io::containers::details::forward_list_merge_common<value_type, Cmp>(
			this->imp, other.imp, cmp));
		other.imp = nullptr;
	}

	inline constexpr void merge(forward_list &&other) noexcept
	{
		this->merge(::std::move(other), ::std::ranges::less{});
	}

	template <typename Cmp>
	inline constexpr void merge_after(const_iterator beforeit, forward_list &&other, Cmp cmp) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return;
		}
		auto otherimp{other.imp};
		other.imp = nullptr;
		auto newimp{static_cast<node_type *>(::fast_io::containers::details::forward_list_merge_common<value_type, Cmp>(
			beforeit->next, otherimp, cmp))};
		beforeit->next = newimp;
	}

	inline constexpr void merge_after(const_iterator beforeit, forward_list &&other) noexcept
	{
		this->merge_after(beforeit, ::std::move(other), ::std::ranges::less{});
	}

	inline constexpr void clear_destroy() noexcept
	{
		this->destroy();
		this->imp = nullptr;
	}

	inline constexpr void clear() noexcept
	{
		this->clear_destroy();
	}
};

template <typename T, typename allocator1, typename allocator2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(::fast_io::containers::forward_list<T, allocator1> const &lhs, ::fast_io::containers::forward_list<T, allocator2> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

#if defined(__cpp_lib_three_way_comparison)

template <typename T, typename allocator1, typename allocator2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(::fast_io::containers::forward_list<T, allocator1> const &lhs, ::fast_io::containers::forward_list<T, allocator2> const &rhs) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), ::std::compare_three_way{});
}

#endif

} // namespace containers

} // namespace fast_io
