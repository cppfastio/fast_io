#pragma once

namespace fast_io
{

namespace containers
{

template <typename Container>
class queue
{
public:
	using container_type = Container;
	using value_type = typename container_type::value_type;
	using size_type = typename container_type::size_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
	container_type container;

	constexpr queue() noexcept = default;

#ifdef __cpp_lib_containers_ranges
	template <::std::ranges::range R>
	explicit constexpr queue(::std::from_range_t, R &&rg)
		: container(::std::from_range, ::std::forward<R>(rg))
	{}
#endif
	constexpr container_type const &get_container() const noexcept
	{
		return container;
	}

	constexpr container_type &get_container() noexcept
	{
		return container;
	}

	constexpr reference front() noexcept
	{
		return container.front();
	}
	constexpr const_reference front() const noexcept
	{
		return container.front();
	}

	constexpr reference front_unchecked() noexcept
	{
		return container.front_unchecked();
	}
	constexpr const_reference front_unchecked() const noexcept
	{
		return container.front_unchecked();
	}

	constexpr reference back() noexcept
	{
		return container.back();
	}
	constexpr const_reference back() const noexcept
	{
		return container.back();
	}

	constexpr reference back_unchecked() noexcept
	{
		return container.back_unchecked();
	}
	constexpr const_reference back_unchecked() const noexcept
	{
		return container.back_unchecked();
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
		requires(requires() {
			container.size();
		})
	{
		return container.size();
	}
	constexpr void push(value_type const &value)
	{
		container.push_back(value);
	}

	constexpr void push(value_type &&value)
	{
		container.push_back(::std::move(value));
	}

	constexpr void reserve(size_type newcap) noexcept
	{
		container.reserve(newcap);
	}

	constexpr void push_unchecked(value_type const &value)
	{
		container.push_back_unchecked(value);
	}

	constexpr void push_unchecked(value_type &&value)
	{
		container.push_back_unchecked(::std::move(value));
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr reference emplace(Args &&...args) noexcept
	{
		return container.emplace_back(::std::forward<Args>(args)...);
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr reference emplace_unchecked(Args &&...args) noexcept
	{
		return container.emplace_back_unchecked(::std::forward<Args>(args)...);
	}

	template <::std::ranges::range R>
	constexpr void push_range(R &&rg)
	{
		container.append_range(::std::forward<R>(rg));
	}

	constexpr void pop() noexcept
	{
		container.pop_front();
	}

	constexpr void pop_unchecked() noexcept
	{
		container.pop_front_unchecked();
	}

	constexpr value_type pop_element() noexcept
		requires(::std::move_constructible<value_type>)
	{
		value_type front(::std::move(container.front()));
		if constexpr (requires() {
						  { container.pop_front_unchecked() };
					  })
		{
			container.pop_front_unchecked();
		}
		else
		{
			container.pop_front();
		}
		return front;
	}

	constexpr value_type pop_element_unchecked() noexcept
		requires(::std::move_constructible<value_type>)
	{
		value_type front(::std::move(container.front_unchecked()));
		if constexpr (requires() {
						  { container.pop_front_unchecked() };
					  })
		{
			container.pop_front_unchecked();
		}
		else
		{
			container.pop_front();
		}
		return front;
	}

	constexpr void clear() noexcept
	{
		container.clear();
	}
	constexpr void clear_destroy() noexcept
	{
		container.clear_destroy();
	}

	constexpr void swap(::fast_io::containers::queue<Container> &b) noexcept
	{
		container.swap(b.container);
	}
};

template <typename Container>
constexpr bool operator==(::fast_io::containers::queue<Container> const &a, ::fast_io::containers::queue<Container> const &b) noexcept
{
	return a.container == b.container;
}

template <typename Container>
constexpr auto operator<=>(::fast_io::containers::queue<Container> const &a, ::fast_io::containers::queue<Container> const &b) noexcept
{
	return a.container <=> b.container;
}

template <typename Container>
constexpr void swap(::fast_io::containers::queue<Container> &a, ::fast_io::containers::queue<Container> &b) noexcept
{
	a.swap(b);
}

} // namespace containers

} // namespace fast_io
