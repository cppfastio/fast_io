#pragma once

namespace fast_io::containers
{

namespace details
{

// Binary-heap operations over random-access iterators. std::ranges::*
// _heap algorithms cannot propagate a 'throws' comparator, so priority_queue
// uses these directly.

template <typename T, typename Cmp>
inline constexpr bool pq_heap_op_may_throw{
	!noexcept(::std::declval<Cmp &>()(::std::declval<T &>(), ::std::declval<T &>())) ||
	!::std::is_nothrow_move_constructible_v<T> ||
	!::std::is_nothrow_move_assignable_v<T>};

// [first, last-1) is a heap; move *(last-1) into its heap position.
template <::std::random_access_iterator Iter, typename Cmp>
inline constexpr void pq_push_heap(Iter first, Iter last, Cmp cmp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::pq_heap_op_may_throw<::std::iter_value_t<Iter>, Cmp>)
{
	using T = ::std::iter_value_t<Iter>;
	using diff_t = ::std::iter_difference_t<Iter>;
	auto const n{static_cast<diff_t>(last - first)};
	if (n < 2)
	{
		return;
	}
	diff_t idx{n - 1};
	T value(::std::move(first[idx]));
	while (idx != 0)
	{
		diff_t const parent{static_cast<diff_t>((idx - 1) >> 1)};
		if (!cmp(first[parent], value))
		{
			break;
		}
		first[idx] = ::std::move(first[parent]);
		idx = parent;
	}
	first[idx] = ::std::move(value);
}

// move the max element at *first to *(last-1), re-heapify [first, last-1).
template <::std::random_access_iterator Iter, typename Cmp>
inline constexpr void pq_pop_heap(Iter first, Iter last, Cmp cmp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::pq_heap_op_may_throw<::std::iter_value_t<Iter>, Cmp>)
{
	using T = ::std::iter_value_t<Iter>;
	using diff_t = ::std::iter_difference_t<Iter>;
	auto const n{static_cast<diff_t>(last - first)};
	if (n < 2)
	{
		return;
	}
	auto *const base{::std::to_address(first)};
	::fast_io::freestanding::iter_swap(base, base + (n - 1));
	diff_t const m{n - 1};
	diff_t idx{0};
	T value(::std::move(first[idx]));
	for (;;)
	{
		diff_t child{static_cast<diff_t>((idx << 1) + 1)};
		if (child >= m)
		{
			break;
		}
		if (child + 1 < m && cmp(first[child], first[child + 1]))
		{
			++child;
		}
		if (!cmp(value, first[child]))
		{
			break;
		}
		first[idx] = ::std::move(first[child]);
		idx = child;
	}
	first[idx] = ::std::move(value);
}

template <::std::random_access_iterator Iter, typename Cmp>
inline constexpr void pq_make_heap(Iter first, Iter last, Cmp cmp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::pq_heap_op_may_throw<::std::iter_value_t<Iter>, Cmp>)
{
	using T = ::std::iter_value_t<Iter>;
	using diff_t = ::std::iter_difference_t<Iter>;
	auto const n{static_cast<diff_t>(last - first)};
	if (n < 2)
	{
		return;
	}
	for (diff_t i{static_cast<diff_t>((n - 2) >> 1)};;)
	{
		T value(::std::move(first[i]));
		diff_t idx{i};
		for (;;)
		{
			diff_t child{static_cast<diff_t>((idx << 1) + 1)};
			if (child >= n)
			{
				break;
			}
			if (child + 1 < n && cmp(first[child], first[child + 1]))
			{
				++child;
			}
			if (!cmp(value, first[child]))
			{
				break;
			}
			first[idx] = ::std::move(first[child]);
			idx = child;
		}
		first[idx] = ::std::move(value);
		if (i == 0)
		{
			break;
		}
		--i;
	}
}

} // namespace details

template <typename Cmp, typename Container>
	requires ::std::ranges::random_access_range<Container> &&
			 ::std::strict_weak_order<Cmp, typename Container::value_type, typename Container::value_type> &&
			 (::std::is_empty_v<Cmp> && ::std::is_nothrow_default_constructible_v<Cmp>)
class priority_queue
{
public:
	using container_type = Container;
	using value_compare = Cmp;
	using value_type = typename container_type::value_type;
	using size_type = typename container_type::size_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
	container_type container;
	inline constexpr priority_queue() FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_default_constructible_v<container_type>) = default;
	inline constexpr container_type &get_container() noexcept
	{
		return container;
	}
	inline constexpr container_type const &get_container() const noexcept
	{
		return container;
	}
	template <::std::ranges::range R>
	inline explicit constexpr priority_queue(::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(container_type(::fast_io::freestanding::from_range, ::std::declval<R &&>())) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
		: container(::fast_io::freestanding::from_range, ::std::forward<R>(rg))
	{
		::fast_io::containers::details::pq_make_heap(this->container.begin(), this->container.end(), value_compare{});
	}
	inline constexpr void swap(priority_queue &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().swap(::std::declval<container_type &>()))
	{
		container.swap(other.container);
	}

	inline constexpr bool is_empty() const noexcept
	{
		return container.is_empty();
	}

	inline constexpr size_type size() const noexcept
	{
		return container.size();
	}

	inline constexpr void clear() noexcept
	{
		container.clear();
	}

	inline constexpr void clear_destroy() noexcept
	{
		container.clear_destroy();
	}

	inline constexpr void reserve(size_type newcap)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().reserve(::std::declval<size_type>()))
	{
		container.reserve(newcap);
	}

	inline constexpr void push(value_type const &value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().push_back(::std::declval<value_type const &>())) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		container.push_back(value);
		::fast_io::containers::details::pq_push_heap(container.begin(), container.end(), value_compare{});
	}

	inline constexpr void push(value_type &&value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().push_back(::std::declval<value_type>())) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		container.push_back(::std::move(value));
		::fast_io::containers::details::pq_push_heap(container.begin(), container.end(), value_compare{});
	}

	inline constexpr void push_unchecked(value_type const &value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().push_back(::std::declval<value_type const &>())) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		container.push_back(value);
		::fast_io::containers::details::pq_push_heap(container.begin(), container.end(), value_compare{});
	}

	inline constexpr void push_unchecked(value_type &&value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().push_back(::std::declval<value_type>())) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		container.push_back(::std::move(value));
		::fast_io::containers::details::pq_push_heap(container.begin(), container.end(), value_compare{});
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr void emplace(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().emplace_back(::std::declval<Args &&>()...)) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		container.emplace_back(::std::forward<Args>(args)...);
		::fast_io::containers::details::pq_push_heap(container.begin(), container.end(), value_compare{});
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr void emplace_unchecked(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().emplace_back_unchecked(::std::declval<Args &&>()...)) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		container.emplace_back_unchecked(::std::forward<Args>(args)...);
		::fast_io::containers::details::pq_push_heap(container.begin(), container.end(), value_compare{});
	}

	template <::std::ranges::range R>
	inline constexpr void push_range(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().append_range(::std::declval<R &&>())) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		container.append_range(::std::forward<R>(rg));
		::fast_io::containers::details::pq_push_heap(container.begin(), container.end(), value_compare{});
	}

	inline constexpr const_reference top() const
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type const &>().front())
	{
		return container.front();
	}

	inline constexpr const_reference top_unchecked() const
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type const &>().front_unchecked())
	{
		return container.front_unchecked();
	}

	inline constexpr void pop()
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().pop_back()) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		::fast_io::containers::details::pq_pop_heap(container.begin(), container.end(), value_compare{});
		container.pop_back();
	}

	inline constexpr void pop_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().pop_back_unchecked()) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
	{
		::fast_io::containers::details::pq_pop_heap(container.begin(), container.end(), value_compare{});
		container.pop_back_unchecked();
	}

	inline constexpr value_type pop_element()
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(value_type(::std::declval<container_type &>().back())) ||
									   !noexcept(::std::declval<container_type &>().pop_back()) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
		requires(::std::move_constructible<value_type>)
	{
		::fast_io::containers::details::pq_pop_heap(container.begin(), container.end(), value_compare{});
		value_type back(::std::move(container.back()));
		container.pop_back();
		return back;
	}

	inline constexpr value_type pop_element_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(value_type(::std::declval<container_type &>().back_unchecked())) ||
									   !noexcept(::std::declval<container_type &>().pop_back()) ||
									   ::fast_io::containers::details::pq_heap_op_may_throw<value_type, value_compare>)
		requires(::std::move_constructible<value_type>)
	{
		::fast_io::containers::details::pq_pop_heap(container.begin(), container.end(), value_compare{});
		value_type back(::std::move(container.back_unchecked()));
		container.pop_back();
		return back;
	}
};

template <typename Cmp, typename Container>
inline constexpr void swap(::fast_io::containers::priority_queue<Cmp, Container> &a, ::fast_io::containers::priority_queue<Cmp, Container> &b)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<::fast_io::containers::priority_queue<Cmp, Container> &>().swap(::std::declval<::fast_io::containers::priority_queue<Cmp, Container> &>()))
{
	a.swap(b);
}

} // namespace fast_io::containers
