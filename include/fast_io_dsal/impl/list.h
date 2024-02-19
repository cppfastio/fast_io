#pragma once

namespace fast_io
{

namespace containers
{

namespace details
{

struct list_node_common
{
	void *prev;
	void *next;
};

template <typename T>
struct list_node
{
	list_node_common ptrs;
	T element;
};

template <typename T, bool isconst>
class list_iterator
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

	constexpr list_iterator &operator++() noexcept
	{
		iter = static_cast<list_node_common *>(iter)->next;
		return *this;
	}

	constexpr list_iterator &operator--() noexcept
	{
		iter = static_cast<list_node_common *>(iter)->prev;
		return *this;
	}

	constexpr list_iterator operator++(int) noexcept
	{
		auto temp(*this);
		++*this;
		return temp;
	}

	constexpr list_iterator operator--(int) noexcept
	{
		auto temp(*this);
		--*this;
		return temp;
	}

	constexpr reference operator*() const noexcept
	{
		return static_cast<list_node<T> *>(iter)->element;
	}

	constexpr operator list_iterator<T, true>()
		requires(!isconst)
	{
		return {this->iter};
	}
};

template <typename T, bool isconst1, bool isconst2>
inline constexpr bool operator==(list_iterator<T, isconst1> a, list_iterator<T, isconst2> b) noexcept
{
	return a.iter == b.iter;
}

template <typename T, bool isconst1, bool isconst2>
inline constexpr bool operator!=(list_iterator<T, isconst1> a, list_iterator<T, isconst2> b) noexcept
{
	return a.iter != b.iter;
}

template <typename allocator>
inline void list_trivially_destroy(void *first, void *last, ::std::size_t sz)
{
	for (void *it{first}; it != last;)
	{
		auto next{static_cast<::fast_io::containers::details::list_node_common *>(it)->next};
		generic_allocator_adapter<allocator>::deallocate_n(it, sz);
		it = next;
	}
}

template <typename allocator>
inline void list_trivially_destroy_aligned(void *first, void *last,
										   ::std::size_t align, ::std::size_t sz)
{
	for (void *it{first}; it != last;)
	{
		auto next{static_cast<::fast_io::containers::details::list_node_common *>(it)->next};
		generic_allocator_adapter<allocator>::deallocate_aligned_n(it, align, sz);
		it = next;
	}
}

template <typename allocator, ::std::size_t align, ::std::size_t sz>
inline void list_trivially_destroy_sa(void *first, void *last)
{
	if constexpr (align <= allocator::default_alignment)
	{
		::fast_io::containers::details::list_trivially_destroy<allocator>(first, last, sz);
	}
	else
	{
		::fast_io::containers::details::list_trivially_destroy_aligned<allocator>(first, last, align, sz);
	}
}

inline constexpr void list_main_insert_ptr_common(void *newnodevp, void *iter) noexcept
{
	auto newnode(static_cast<::fast_io::containers::details::list_node_common *>(newnodevp));
	auto node = static_cast<::fast_io::containers::details::list_node_common *>(iter);
	auto prev = static_cast<::fast_io::containers::details::list_node_common *>(node->prev);
	newnode->next = node;
	newnode->prev = prev;
	node->prev = newnode;
	prev->next = newnode;
}

inline constexpr void list_main_push_front_ptr_common(void *newnodevp, void *imp) noexcept
{
	auto newnode(static_cast<::fast_io::containers::details::list_node_common *>(newnodevp));
	auto prev = static_cast<::fast_io::containers::details::list_node_common *>(imp);
	auto node = static_cast<::fast_io::containers::details::list_node_common *>(prev->next);
	newnode->next = node;
	newnode->prev = prev;
	node->prev = newnode;
	prev->next = newnode;
}

inline constexpr void list_main_erase_ptr_common(void *iter) noexcept
{
	auto node = static_cast<::fast_io::containers::details::list_node_common *>(iter);
	auto prev = static_cast<::fast_io::containers::details::list_node_common *>(node->prev);
	auto next = static_cast<::fast_io::containers::details::list_node_common *>(node->next);
	prev->next = next;
	next->prev = prev;
}

template <typename allocator>
inline constexpr void *list_trivially_allocate_insert(void *iter, ::std::size_t sz) noexcept
{
	void *newnode = generic_allocator_adapter<allocator>::allocate(sz);
	list_main_insert_ptr_common(newnode, iter);
	return newnode;
}

template <typename allocator>
inline constexpr void *list_trivially_allocate_insert_aligned(void *iter, ::std::size_t align, ::std::size_t sz) noexcept
{
	void *newnode = generic_allocator_adapter<allocator>::allocate_aligned(align, sz);
	list_main_insert_ptr_common(newnode, iter);
	return newnode;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz>
inline constexpr void *list_trivially_allocate_insert_sa(void *iter) noexcept
{
	if constexpr (align <= allocator::default_alignment)
	{
		return ::fast_io::containers::details::list_trivially_allocate_insert<allocator>(iter, sz);
	}
	else
	{
		return ::fast_io::containers::details::list_trivially_allocate_insert_aligned<allocator>(iter, align, sz);
	}
}

template <typename allocator>
inline constexpr void *list_trivially_allocate_push_front(void *imp, ::std::size_t sz) noexcept
{
	void *newnode = generic_allocator_adapter<allocator>::allocate(sz);
	list_main_push_front_ptr_common(newnode, imp);
	return newnode;
}

template <typename allocator>
inline constexpr void *list_trivially_allocate_push_front_aligned(void *imp, ::std::size_t align, ::std::size_t sz) noexcept
{
	void *newnode = generic_allocator_adapter<allocator>::allocate_aligned(align, sz);
	list_main_push_front_ptr_common(newnode, imp);
	return newnode;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz>
inline constexpr void *list_trivially_allocate_push_front_sa(void *imp) noexcept
{
	if constexpr (align <= allocator::default_alignment)
	{
		return ::fast_io::containers::details::list_trivially_allocate_push_front<allocator>(imp, sz);
	}
	else
	{
		return ::fast_io::containers::details::list_trivially_allocate_push_front_aligned<allocator>(imp, align, sz);
	}
}

inline constexpr void list_reverse_common(void *firstptr, void *lastptr) noexcept
{
	for (; firstptr != lastptr;)
	{
		auto first{static_cast<::fast_io::containers::details::list_node_common *>(firstptr)};
		auto nextptr(first->next);
		::std::ranges::swap(first->next, first->prev);
		firstptr = nextptr;
	}
}

inline constexpr void list_splice_range_common_unchecked(void *nodeptr, void *firstptr, void *lastptr) noexcept
{
	auto node(static_cast<::fast_io::containers::details::list_node_common *>(nodeptr));
	auto first(static_cast<::fast_io::containers::details::list_node_common *>(firstptr));
	auto last(static_cast<::fast_io::containers::details::list_node_common *>(lastptr));
	auto nodeprev{static_cast<::fast_io::containers::details::list_node_common *>(node->prev)};

	auto firstprev(static_cast<::fast_io::containers::details::list_node_common *>(first->prev));
	auto lastprev(static_cast<::fast_io::containers::details::list_node_common *>(last->prev));

	nodeprev->next = first;
	first->prev = nodeprev;
	node->prev = lastprev;
	lastprev->next = node;

	last->prev = firstprev;
	firstprev->next = last;
}

inline constexpr void list_splice_range_common(void *nodeptr, void *firstptr, void *lastptr) noexcept
{
	if (firstptr == lastptr)
	{
		return;
	}
	list_splice_range_common_unchecked(nodeptr, firstptr, lastptr);
}

inline constexpr void list_splice_single_common(void *nodeptr, void *iterptr) noexcept
{
	auto node(static_cast<::fast_io::containers::details::list_node_common *>(nodeptr));
	auto iter(static_cast<::fast_io::containers::details::list_node_common *>(iterptr));
	auto iternext(static_cast<::fast_io::containers::details::list_node_common *>(iter->next));
	auto iterprev(static_cast<::fast_io::containers::details::list_node_common *>(iter->prev));
	auto nodeprev(static_cast<::fast_io::containers::details::list_node_common *>(node->prev));

	iter->prev = nodeprev;
	iter->next = node;
	iterprev->next = iternext;
	iternext->prev = iterprev;
}

inline constexpr ::std::size_t list_ptr_distance(void *firstptr, void *lastptr) noexcept
{
	::std::size_t n{};
	for (; firstptr != lastptr; firstptr = static_cast<::fast_io::containers::details::list_node_common *>(firstptr)->next)
	{
		++n;
	}
	return n;
}

inline constexpr void *list_ptr_advance(void *firstptr, ::std::size_t n) noexcept
{
	for (; n; --n)
	{
		firstptr = static_cast<::fast_io::containers::details::list_node_common *>(firstptr)->next;
	}
	return firstptr;
}

template <typename T, typename Cmp>
inline constexpr void list_merge_common(void *leftfirstptr, void *leftlastptr, void *rightfirstptr, void *rightlastptr, Cmp cmp)
{
	for (; leftfirstptr != leftlastptr;)
	{
		auto rightfirst{static_cast<::fast_io::containers::details::list_node_common *>(rightfirstptr)};
		auto rightcurrptr{rightfirstptr};
		auto &leftfirstele{static_cast<list_node<T> *>(leftfirstptr)->element};

		for (; rightcurrptr != rightlastptr && cmp(static_cast<list_node<T> *>(rightcurrptr)->element, leftfirstele);
			 rightcurrptr = static_cast<::fast_io::containers::details::list_node_common *>(rightcurrptr)->next)
		{
		}

		auto leftfirst{static_cast<::fast_io::containers::details::list_node_common *>(leftfirstptr)};
		auto leftfirstnext{static_cast<::fast_io::containers::details::list_node_common *>(leftfirst->next)};
		::fast_io::containers::details::list_splice_range_common(leftfirst, rightfirst, rightcurrptr);
		rightfirstptr = rightcurrptr;
		leftfirstptr = leftfirstnext;
	}
	::fast_io::containers::details::list_splice_range_common(leftlastptr, rightfirstptr, rightlastptr);
}

template <typename T, typename Cmp>
inline constexpr void list_sort_n_common(void *firstptr, void *lastptr, ::std::size_t n, Cmp cmp)
{
	switch (n)
	{
	case 2:
	{
		auto first{static_cast<::fast_io::containers::details::list_node_common *>(firstptr)};
		auto firstnextptr{first->next};
		if (cmp(static_cast<list_node<T> *>(firstnextptr)->element, static_cast<list_node<T> *>(firstptr)->element))
		{
			auto firstprev{static_cast<::fast_io::containers::details::list_node_common *>(first->prev)};
			auto firstnext{static_cast<::fast_io::containers::details::list_node_common *>(firstnextptr)};
			firstprev->next = firstnext;
			firstnext->prev = firstprev;
			firstnext->next = first;
			first->prev = firstnext;
			auto last{static_cast<::fast_io::containers::details::list_node_common *>(lastptr)};
			first->next = last;
			last->prev = first;
		}

		[[fallthrough]];
	}
	case 0:
	case 1:
	{
		return;
	}
	default:
	{
		::std::size_t halfdis{n >> 1};
		void *middleptr{list_ptr_advance(firstptr, halfdis)};

		auto first{static_cast<::fast_io::containers::details::list_node_common *>(firstptr)};
		auto firstprev{static_cast<::fast_io::containers::details::list_node_common *>(first->prev)};
		auto middle{static_cast<::fast_io::containers::details::list_node_common *>(middleptr)};
		auto middleprev{static_cast<::fast_io::containers::details::list_node_common *>(middle->prev)};

		::fast_io::containers::details::list_node_common leftdetacher{middleprev, first};
		first->prev = middleprev->next = __builtin_addressof(leftdetacher);

		auto last{static_cast<::fast_io::containers::details::list_node_common *>(lastptr)};
		auto lastprev{static_cast<::fast_io::containers::details::list_node_common *>(last->prev)};
		::fast_io::containers::details::list_node_common rightdetacher{lastprev, middle};
		middle->prev = lastprev->next = __builtin_addressof(rightdetacher);

		list_sort_n_common<T, Cmp>(first, __builtin_addressof(leftdetacher), halfdis, cmp);
		list_sort_n_common<T, Cmp>(middle, __builtin_addressof(rightdetacher), static_cast<::std::size_t>(n - halfdis), cmp);

		list_merge_common<T, Cmp>(leftdetacher.next, __builtin_addressof(leftdetacher),
								  rightdetacher.next, __builtin_addressof(rightdetacher), cmp);

		auto leftdetacherprev{static_cast<::fast_io::containers::details::list_node_common *>(leftdetacher.prev)};
		leftdetacherprev->next = last;
		last->prev = leftdetacherprev;
		auto leftdetachernext{static_cast<::fast_io::containers::details::list_node_common *>(leftdetacher.next)};
		leftdetachernext->prev = firstprev;
		firstprev->next = leftdetachernext;
	}
	}
}

template <typename T, typename Cmp>
inline constexpr void list_sort_common(void *firstptr, void *lastptr, Cmp cmp)
{
	if (firstptr == lastptr)
	{
		return;
	}
	::std::size_t n{list_ptr_distance(firstptr, lastptr)};
	list_sort_n_common<T, Cmp>(firstptr, lastptr, n, cmp);
}

} // namespace details

template <typename T, typename allocator>
class list
{
public:
	using allocator_type = allocator;
	using value_type = T;

private:
	using node_type = ::fast_io::containers::details::list_node<value_type>;
	using typed_allocator_type = typed_generic_allocator_adapter<allocator_type,
																 node_type, false>;
	static inline constexpr bool alloc_with_status{allocator::has_status};
	using handle_type = ::std::conditional_t<alloc_with_status, typename allocator::handle_type, allocator_type>;
	using handle_holder_type = ::fast_io::containers::details::handle_holder<handle_type>;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	handle_holder_type allochdl;

	constexpr auto allocate_new_raw_node() noexcept
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
		::fast_io::containers::details::list_node<value_type> *ptr;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
		[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
		[[no_unique_address]]
#endif
#endif
		::std::conditional_t<alloc_with_status, list<T, allocator> *, allocator> v;
		explicit constexpr new_handle_guard(::fast_io::containers::details::list_node<value_type> *p,
											list<T, allocator> *va)
		{
			ptr = p;
			if constexpr (alloc_with_status)
			{
				v = va;
			}
		}
		new_handle_guard(new_handle_guard const &) = delete;
		new_handle_guard &operator=(new_handle_guard const &) = delete;
		constexpr ~new_handle_guard()
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
			}
		}
	};

	struct list_destroyer
	{
		list<T, allocator> *plst;
		explicit constexpr list_destroyer(list<T, allocator> *pl) noexcept
			: plst(pl)
		{}
		constexpr void release()
		{
			plst = nullptr;
		}
		constexpr ~list_destroyer()
		{
			if (plst == nullptr)
			{
				return;
			}
			plst->destroy();
		}
	};

public:
	using pointer = value_type *;
	using const_pointer = value_type const *;

	using reference = value_type &;
	using const_reference = value_type const &;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	using iterator = ::fast_io::containers::details::list_iterator<T, false>;
	using const_iterator = ::fast_io::containers::details::list_iterator<T, true>;

	::fast_io::containers::details::list_node_common imp;

	constexpr list() noexcept
		: imp{__builtin_addressof(imp), __builtin_addressof(imp)}
	{
	}

	template <::std::forward_iterator Iter, typename Sentinel>
	explicit constexpr list(Iter first, Sentinel last)
		: imp{__builtin_addressof(imp), __builtin_addressof(imp)}
	{
		list_destroyer destroyer(this);
		for (; first != last; ++first)
		{
			this->push_back(*first);
		}
		destroyer.release();
	}

	explicit constexpr list(::std::initializer_list<value_type> ilist)
		: list(ilist.begin(), ilist.end())
	{
	}

	explicit constexpr list(::std::size_t n)
		: imp{__builtin_addressof(imp), __builtin_addressof(imp)}
	{
		list_destroyer destroyer(this);
		for (::std::size_t i{}; i != n; ++i)
		{
			this->emplace_back();
		}
		destroyer.release();
	}

	inline constexpr iterator begin() noexcept
	{
		return {imp.next};
	}
	inline constexpr iterator end() noexcept
	{
		return {__builtin_addressof(imp)};
	}

	inline constexpr const_iterator begin() const noexcept
	{
		return {imp.next};
	}
	inline constexpr const_iterator end() const noexcept
	{
		return {__builtin_addressof(imp)};
	}

	inline constexpr const_iterator cbegin() const noexcept
	{
		return {imp.next};
	}
	inline constexpr const_iterator cend() const noexcept
	{
		return {__builtin_addressof(imp)};
	}

	inline constexpr bool empty() const noexcept
	{
		return imp.next == __builtin_addressof(imp);
	}

	inline constexpr bool is_empty() const noexcept
	{
		return imp.next == __builtin_addressof(imp);
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mxvl{SIZE_MAX / sizeof(node_type)};
		return mxvl;
	}

private:
	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr auto allocate_construct_new_node(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
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
	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr reference emplace_back(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			auto newnode = static_cast<node_type *>(list_trivially_allocate_insert_sa<allocator_type, alignof(node_type), sizeof(node_type)>(__builtin_addressof(imp)));
			::std::construct_at(
				__builtin_addressof(newnode->element),
				::std::forward<Args>(args)...);
			return newnode->element;
		}
		else
		{
			auto newnodecons = allocate_construct_new_node(::std::forward<Args>(args)...);
			::fast_io::containers::details::list_main_insert_ptr_common(newnodecons, __builtin_addressof(imp));
			return newnodecons->element;
		}
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr reference emplace_front(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			auto newnode = static_cast<node_type *>(list_trivially_allocate_push_front_sa<allocator_type, alignof(node_type), sizeof(node_type)>(__builtin_addressof(imp)));
			::std::construct_at(
				__builtin_addressof(newnode->element),
				::std::forward<Args>(args)...);
			return newnode->element;
		}
		else
		{
			auto newnodecons = allocate_construct_new_node(::std::forward<Args>(args)...);
			::fast_io::containers::details::list_main_push_front_ptr_common(newnodecons, __builtin_addressof(imp));
			return newnodecons->element;
		}
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr reference emplace(const_iterator iter, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			auto newnode = static_cast<node_type *>(list_trivially_allocate_insert_sa<allocator_type, alignof(node_type), sizeof(node_type)>(iter.iter));
			::std::construct_at(
				__builtin_addressof(newnode->element),
				::std::forward<Args>(args)...);
			return newnode->element;
		}
		else
		{
			auto newnodecons = allocate_construct_new_node(::std::forward<Args>(args)...);
			::fast_io::containers::details::list_main_insert_ptr_common(newnodecons, iter.iter);
			return newnodecons->element;
		}
	}

private:
	constexpr void destroy_node(void *it) noexcept
	{
		auto node = static_cast<node_type *>(it);
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			__builtin_addressof(node->element)->~value_type();
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

public:
	constexpr void push_back(const_reference value) noexcept(noexcept(this->emplace_back(value)))
	{
		this->emplace_back(value);
	}

	constexpr void push_front(const_reference value) noexcept(noexcept(this->emplace_back(value)))
	{
		this->emplace_front(value);
	}

	constexpr void push_back(T &&value) noexcept(noexcept(this->emplace_back(::std::move(value))))
	{
		this->emplace_back(std::move(value));
	}

	constexpr void push_front(T &&value) noexcept(noexcept(this->emplace_front(::std::move(value))))
	{
		this->emplace_front(std::move(value));
	}

	constexpr void insert(const_iterator iter, T &&value) noexcept(noexcept(this->emplace_front(::std::move(value))))
	{
		this->emplace(iter, std::move(value));
	}

	constexpr void insert(const_iterator iter, const_reference value) noexcept(noexcept(this->emplace(iter, value)))
	{
		this->emplace(iter, value);
	}

	constexpr void pop_back() noexcept
	{
		auto node = static_cast<::fast_io::containers::details::list_node_common *>(imp.next);
		auto prev = static_cast<::fast_io::containers::details::list_node_common *>(node->prev);
		imp.prev = prev;
		prev->next = __builtin_addressof(imp);
		this->destroy_node(node);
	}

	constexpr void pop_front() noexcept
	{
		auto node = static_cast<::fast_io::containers::details::list_node_common *>(imp.prev);
		auto next = static_cast<::fast_io::containers::details::list_node_common *>(node->next);
		imp.next = next;
		next->prev = __builtin_addressof(imp);
		this->destroy_node(node);
	}

	constexpr void erase(const_iterator iter) noexcept
	{
		auto node = static_cast<::fast_io::containers::details::list_node_common *>(iter.iter);
		::fast_io::containers::details::list_main_erase_ptr_common(node);
		destroy_node(node);
	}

	constexpr list(list const &other)
		requires(::std::copyable<value_type>)
	{
		for (auto const &ele : other)
		{
			this->push_back(ele);
		}
	}

	constexpr list &operator=(list const &other)
		requires(::std::copyable<value_type>)
	{
		list temp(other);
		this->operator=(::std::move(temp));
		return *this;
	}

	constexpr list(list &&other) noexcept
		: imp(other.imp)
	{
		auto prev = static_cast<::fast_io::containers::details::list_node_common *>(imp.prev);
		auto next = static_cast<::fast_io::containers::details::list_node_common *>(imp.next);
		next->prev = prev->next = __builtin_addressof(imp);
		other.imp = {__builtin_addressof(other.imp), __builtin_addressof(other.imp)};
	}

	constexpr list &operator=(list &&other) noexcept
	{
		if (__builtin_addressof(other) != this) [[likely]]
		{
			this->destroy();
			imp = other.imp;
			auto prev = static_cast<::fast_io::containers::details::list_node_common *>(imp.prev);
			auto next = static_cast<::fast_io::containers::details::list_node_common *>(imp.next);
			next->prev = prev->next = __builtin_addressof(imp);
			other.imp = {__builtin_addressof(other.imp), __builtin_addressof(other.imp)};
		}
		return *this;
	}

private:
	constexpr void destroy() noexcept
	{
#if __cpp_if_consteval >= 202106L
		if !consteval
#else
		if (!__builtin_is_constant_evaluated())
#endif
		{
			if constexpr (::std::is_trivially_destructible_v<value_type> && !alloc_with_status)
			{
				::fast_io::containers::details::list_trivially_destroy_sa<allocator_type,
																		  alignof(node_type), sizeof(node_type)>(imp.next, __builtin_addressof(imp));
				return;
			}
		}
		for (void *it{imp.next}, *ed{__builtin_addressof(imp)}; it != ed;)
		{
			auto next{static_cast<::fast_io::containers::details::list_node_common *>(it)->next};
			this->destroy_node(it);
			it = next;
		}
	}

public:
	constexpr void clear() noexcept
	{
		this->destroy();
		imp = {__builtin_addressof(imp), __builtin_addressof(imp)};
	}

	constexpr void splice(const_iterator pos, const_iterator first, const_iterator last) noexcept
	{
		::fast_io::containers::details::list_splice_range_common(pos.iter, first.iter, last.iter);
	}

	constexpr void splice(const_iterator pos, const_iterator it) noexcept
	{
		::fast_io::containers::details::list_splice_single_common(pos.iter, it.iter);
	}

	constexpr void splice(const_iterator pos, list &&other) noexcept
	{
		::fast_io::containers::details::list_splice_range_common(pos.iter, other.imp.next, __builtin_addressof(other.imp));
#if 0
		other.imp = {__builtin_addressof(other.imp), __builtin_addressof(other.imp)};
#endif
	}

	constexpr void reverse() noexcept
	{
		::fast_io::containers::details::list_reverse_common(imp.next, __builtin_addressof(imp));
		::std::ranges::swap(imp.prev, imp.next);
	}

	template <typename Cmp>
	constexpr void merge(list &&other, Cmp cmp) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return;
		}
		::fast_io::containers::details::list_merge_common<value_type, Cmp>(imp.next, __builtin_addressof(imp),
																		   other.imp.next, __builtin_addressof(other.imp), cmp);
	}

	constexpr void merge(list &&other) noexcept
	{
		this->merge(::std::move(other), ::std::ranges::less{});
	}

	template <typename Cmp>
	constexpr void sort(Cmp cmp) noexcept
	{
		::fast_io::containers::details::list_sort_common<value_type, Cmp>(imp.next, __builtin_addressof(imp), cmp);
	}

	constexpr void sort() noexcept
	{
		this->sort(::std::ranges::less{});
	}

	constexpr ~list()
	{
		this->destroy();
	}
};
} // namespace containers

} // namespace fast_io