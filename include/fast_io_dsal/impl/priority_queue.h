#pragma once

namespace fast_io::containers
{

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
	constexpr priority_queue() noexcept(::std::is_nothrow_default_constructible_v<container_type>) = default;
	constexpr container_type &get_container() noexcept
	{
		return container;
	}
	constexpr container_type const &get_container() const noexcept
	{
		return container;
	}
#ifdef __cpp_lib_containers_ranges
	template <::std::ranges::range R>
	explicit constexpr priority_queue(::std::from_range_t, R &&rg)
		: container(::std::from_range, ::std::forward<R>(rg))
	{
		::std::ranges::make_heap(this->container, value_compare{});
	}
#endif

	constexpr void swap(priority_queue &other) noexcept
	{
		container.swap(other.container);
	}
	constexpr bool empty() const noexcept
	{
		if constexpr (requires() {
						  { container.is_empty() } -> ::std::convertible_to<bool>;
					  })
		{
			return container.is_empty();
		}
		else
		{
			return container.empty();
		}
	}

	constexpr bool is_empty() const noexcept
	{
		return container.is_empty();
	}

	constexpr size_type size() const noexcept
	{
		return container.size();
	}

	constexpr void clear() noexcept
	{
		container.clear();
	}

	constexpr void clear_destroy() noexcept
	{
		container.clear_destroy();
	}

	constexpr void reserve(size_type newcap) noexcept
	{
		container.reserve(newcap);
	}

	constexpr void push(value_type const &value)
	{
		container.push_back(value);
		::std::ranges::push_heap(container, value_compare{});
	}

	constexpr void push(value_type &&value)
	{
		container.push_back(::std::move(value));
		::std::ranges::push_heap(container, value_compare{});
	}

	constexpr void push_unchecked(value_type const &value)
	{
		container.push_back(value);
		::std::ranges::push_heap(container, value_compare{});
	}

	constexpr void push_unchecked(value_type &&value)
	{
		container.push_back(::std::move(value));
		::std::ranges::push_heap(container, value_compare{});
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr void emplace(Args &&...args) noexcept
	{
		container.emplace_back(::std::forward<Args>(args)...);
		::std::ranges::push_heap(container, value_compare{});
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr void emplace_unchecked(Args &&...args) noexcept
	{
		container.emplace_back_unchecked(::std::forward<Args>(args)...);
		::std::ranges::push_heap(container, value_compare{});
	}

	template <::std::ranges::range R>
	constexpr void push_range(R &&rg)
	{
		container.append_range(::std::forward<R>(rg));
		::std::ranges::push_heap(container, value_compare{});
	}

	constexpr const_reference top() const noexcept
	{
		return container.front();
	}

	constexpr const_reference top_unchecked() const noexcept
	{
		return container.front_unchecked();
	}

	constexpr void pop() noexcept
	{
		::std::ranges::pop_heap(container, value_compare{});
		container.pop_back();
	}

	constexpr void pop_unchecked() noexcept
	{
		::std::ranges::pop_heap(container, value_compare{});
		container.pop_back_unchecked();
	}

	constexpr value_type pop_element() noexcept
		requires(::std::move_constructible<value_type>)
	{
		::std::ranges::pop_heap(container, value_compare{});
		value_type back(::std::move(container.back()));
		container.pop_back();
		return back;
	}

	constexpr value_type pop_element_unchecked() noexcept
		requires(::std::move_constructible<value_type>)
	{
		::std::ranges::pop_heap(container, value_compare{});
		value_type back(::std::move(container.back()));
		container.pop_back();
		return back;
	}
};

template <typename Cmp, typename Container>
constexpr void swap(::fast_io::containers::priority_queue<Cmp, Container> &a, ::fast_io::containers::priority_queue<Cmp, Container> &b) noexcept
{
	a.swap(b);
}

} // namespace fast_io::containers
