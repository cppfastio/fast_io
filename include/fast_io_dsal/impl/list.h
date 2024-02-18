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
	list_node_common
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

	constexpr ~list()
	{
		this->destroy();
	}
};
} // namespace containers

} // namespace fast_io