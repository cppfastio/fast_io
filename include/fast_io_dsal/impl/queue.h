#pragma once

namespace fast_io
{

namespace containers
{

namespace details
{

// pop_element prefers pop_front_unchecked when the container provides it
template <typename Container>
inline constexpr bool queue_pop_front_may_throw{[] {
	if constexpr (requires(Container &c) { c.pop_front_unchecked(); })
	{
		return !noexcept(::std::declval<Container &>().pop_front_unchecked());
	}
	else
	{
		return !noexcept(::std::declval<Container &>().pop_front());
	}
}()};

template <typename Container>
inline constexpr bool queue_pop_element_may_throw{
	!noexcept(typename Container::value_type(::std::declval<Container &>().front())) ||
	queue_pop_front_may_throw<Container>};

template <typename Container>
inline constexpr bool queue_pop_element_unchecked_may_throw{
	!noexcept(typename Container::value_type(::std::declval<Container &>().front_unchecked())) ||
	queue_pop_front_may_throw<Container>};

} // namespace details

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

	inline constexpr queue() noexcept = default;

	template <::std::ranges::range R>
	inline explicit constexpr queue(::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(container_type(::fast_io::freestanding::from_range, ::std::declval<R &&>()))
		: container(::fast_io::freestanding::from_range, ::std::forward<R>(rg))
	{}
	inline constexpr container_type const &get_container() const noexcept
	{
		return container;
	}

	inline constexpr container_type &get_container() noexcept
	{
		return container;
	}

	inline constexpr reference front()
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().front())
	{
		return container.front();
	}
	inline constexpr const_reference front() const
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type const &>().front())
	{
		return container.front();
	}

	inline constexpr reference front_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().front_unchecked())
	{
		return container.front_unchecked();
	}
	inline constexpr const_reference front_unchecked() const
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type const &>().front_unchecked())
	{
		return container.front_unchecked();
	}

	inline constexpr reference back()
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().back())
	{
		return container.back();
	}
	inline constexpr const_reference back() const
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type const &>().back())
	{
		return container.back();
	}

	inline constexpr reference back_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().back_unchecked())
	{
		return container.back_unchecked();
	}
	inline constexpr const_reference back_unchecked() const
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type const &>().back_unchecked())
	{
		return container.back_unchecked();
	}


	inline constexpr bool is_empty() const noexcept
	{
		return container.is_empty();
	}

	inline constexpr size_type size() const noexcept
		requires(requires() {
			container.size();
		})
	{
		return container.size();
	}
	inline constexpr void push(value_type const &value)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().push_back(::std::declval<value_type const &>()))
	{
		container.push_back(value);
	}

	inline constexpr void push(value_type &&value)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().push_back(::std::declval<value_type>()))
	{
		container.push_back(::std::move(value));
	}

	inline constexpr void reserve(size_type newcap)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().reserve(::std::declval<size_type>()))
	{
		container.reserve(newcap);
	}

	inline constexpr void push_unchecked(value_type const &value)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().push_back_unchecked(::std::declval<value_type const &>()))
	{
		container.push_back_unchecked(value);
	}

	inline constexpr void push_unchecked(value_type &&value)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().push_back_unchecked(::std::declval<value_type>()))
	{
		container.push_back_unchecked(::std::move(value));
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().emplace_back(::std::declval<Args &&>()...)))
	{
		return container.emplace_back(::std::forward<Args>(args)...);
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_unchecked(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().emplace_back_unchecked(::std::declval<Args &&>()...)))
	{
		return container.emplace_back_unchecked(::std::forward<Args>(args)...);
	}

	template <::std::ranges::range R>
	inline constexpr void push_range(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().append_range(::std::declval<R &&>()))
	{
		container.append_range(::std::forward<R>(rg));
	}

	inline constexpr void pop()
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().pop_front())
	{
		container.pop_front();
	}

	inline constexpr void pop_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().pop_front_unchecked())
	{
		container.pop_front_unchecked();
	}

	inline constexpr value_type pop_element()
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::queue_pop_element_may_throw<container_type>)
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

	inline constexpr value_type pop_element_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::queue_pop_element_unchecked_may_throw<container_type>)
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

	inline constexpr void clear() noexcept
	{
		container.clear();
	}
	inline constexpr void clear_destroy() noexcept
	{
		container.clear_destroy();
	}

	inline constexpr void swap(::fast_io::containers::queue<Container> &b)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().swap(::std::declval<container_type &>()))
	{
		container.swap(b.container);
	}
};

template <typename Container>
inline constexpr bool operator==(::fast_io::containers::queue<Container> const &a, ::fast_io::containers::queue<Container> const &b)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<Container const &>() == ::std::declval<Container const &>())
{
	return a.container == b.container;
}

template <typename Container>
inline constexpr auto operator<=>(::fast_io::containers::queue<Container> const &a, ::fast_io::containers::queue<Container> const &b)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<Container const &>() <=> ::std::declval<Container const &>())
{
	return a.container <=> b.container;
}

template <typename Container>
inline constexpr void swap(::fast_io::containers::queue<Container> &a, ::fast_io::containers::queue<Container> &b)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<::fast_io::containers::queue<Container> &>().swap(::std::declval<::fast_io::containers::queue<Container> &>()))
{
	a.swap(b);
}

} // namespace containers

} // namespace fast_io
