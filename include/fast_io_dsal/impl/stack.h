#pragma once

namespace fast_io
{

namespace containers
{

namespace details
{

// stack forwards to push_back()/back() containers when available, and to
// push_front()/front() (e.g. deque without these members' fallback shape)
// otherwise; these predicates evaluate noexcept-ness of whichever branch a
// given container takes so adaptors inherit the container's throws spec.

template <typename Container>
concept stack_container_uses_back = requires(Container &c, typename Container::value_type const &v) {
	{ c.push_back(v) };
	{ c.back() } -> ::std::same_as<typename Container::reference>;
};

template <typename Container>
concept stack_container_has_pop_back = requires(Container &c) {
	{ c.pop_back() };
};

template <typename Container>
concept stack_container_has_pop_back_unchecked = requires(Container &c) {
	{ c.pop_back_unchecked() };
};

template <typename Container>
concept stack_container_has_pop_front_unchecked = requires(Container &c) {
	{ c.pop_front_unchecked() };
};

template <typename Container>
concept stack_container_uses_back_const = requires(Container &c) {
	{ c.back() } -> ::std::same_as<typename Container::const_reference>;
};

template <typename Container>
concept stack_container_uses_back_unchecked_const = requires(Container &c) {
	{ c.back_unchecked() } -> ::std::same_as<typename Container::const_reference>;
};

template <typename Container, bool isconst>
inline constexpr bool stack_top_may_throw{[] {
	using cref = ::std::conditional_t<isconst, Container const &, Container &>;
	if constexpr (isconst ? stack_container_uses_back_const<Container> : stack_container_uses_back<Container>)
	{
		return !noexcept(::std::declval<cref>().back());
	}
	else
	{
		return !noexcept(::std::declval<cref>().front());
	}
}()};

template <typename Container, bool isconst>
inline constexpr bool stack_top_unchecked_may_throw{[] {
	using cref = ::std::conditional_t<isconst, Container const &, Container &>;
	if constexpr (isconst ? stack_container_uses_back_unchecked_const<Container> : stack_container_uses_back<Container>)
	{
		return !noexcept(::std::declval<cref>().back_unchecked());
	}
	else
	{
		return !noexcept(::std::declval<cref>().front_unchecked());
	}
}()};

template <typename Container, typename Arg>
inline constexpr bool stack_push_may_throw{[] {
	if constexpr (stack_container_uses_back<Container>)
	{
		return !noexcept(::std::declval<Container &>().push_back(::std::declval<Arg>()));
	}
	else
	{
		return !noexcept(::std::declval<Container &>().push_front(::std::declval<Arg>()));
	}
}()};

template <typename Container, typename Arg>
inline constexpr bool stack_push_unchecked_may_throw{[] {
	if constexpr (stack_container_uses_back<Container>)
	{
		return !noexcept(::std::declval<Container &>().push_back_unchecked(::std::declval<Arg>()));
	}
	else
	{
		return !noexcept(::std::declval<Container &>().push_front_unchecked(::std::declval<Arg>()));
	}
}()};

template <typename Container, typename... Args>
inline constexpr bool stack_emplace_may_throw{[] {
	if constexpr (stack_container_uses_back<Container>)
	{
		return !noexcept(::std::declval<Container &>().emplace_back(::std::declval<Args>()...));
	}
	else
	{
		return !noexcept(::std::declval<Container &>().emplace_front(::std::declval<Args>()...));
	}
}()};

template <typename Container, typename... Args>
inline constexpr bool stack_emplace_unchecked_may_throw{[] {
	if constexpr (stack_container_uses_back<Container>)
	{
		return !noexcept(::std::declval<Container &>().emplace_back_unchecked(::std::declval<Args>()...));
	}
	else
	{
		return !noexcept(::std::declval<Container &>().emplace_front_unchecked(::std::declval<Args>()...));
	}
}()};

template <typename Container>
inline constexpr bool stack_pop_may_throw{[] {
	if constexpr (stack_container_has_pop_back<Container>)
	{
		return !noexcept(::std::declval<Container &>().pop_back());
	}
	else
	{
		return !noexcept(::std::declval<Container &>().pop_front());
	}
}()};

template <typename Container>
inline constexpr bool stack_pop_unchecked_may_throw{[] {
	if constexpr (stack_container_has_pop_back<Container>)
	{
		return !noexcept(::std::declval<Container &>().pop_back_unchecked());
	}
	else
	{
		return !noexcept(::std::declval<Container &>().pop_front_unchecked());
	}
}()};

// pop_element first moves the element out of the container, then pops
template <typename Container>
inline constexpr bool stack_pop_element_may_throw{[] {
	if constexpr (stack_container_has_pop_back<Container>)
	{
		if constexpr (stack_container_has_pop_back_unchecked<Container>)
		{
			return !noexcept(typename Container::value_type(::std::declval<Container &>().back())) ||
				   !noexcept(::std::declval<Container &>().pop_back_unchecked());
		}
		else
		{
			return !noexcept(typename Container::value_type(::std::declval<Container &>().back())) ||
				   !noexcept(::std::declval<Container &>().pop_back());
		}
	}
	else
	{
		if constexpr (stack_container_has_pop_front_unchecked<Container>)
		{
			return !noexcept(typename Container::value_type(::std::declval<Container &>().front())) ||
				   !noexcept(::std::declval<Container &>().pop_front_unchecked());
		}
		else
		{
			return !noexcept(typename Container::value_type(::std::declval<Container &>().front())) ||
				   !noexcept(::std::declval<Container &>().pop_front());
		}
	}
}()};

template <typename Container>
inline constexpr bool stack_pop_element_unchecked_may_throw{[] {
	if constexpr (stack_container_uses_back<Container>)
	{
		return !noexcept(::std::declval<typename Container::value_type>(
				   ::std::declval<Container &>().back_unchecked())) ||
			   stack_pop_unchecked_may_throw<Container>;
	}
	else
	{
		return !noexcept(::std::declval<typename Container::value_type>(
				   ::std::declval<Container &>().front_unchecked())) ||
			   stack_pop_unchecked_may_throw<Container>;
	}
}()};

} // namespace details

template <typename Container>
class stack
{
public:
	using container_type = Container;
	using value_type = typename container_type::value_type;
	using size_type = typename container_type::size_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
	container_type container;

	inline constexpr stack() noexcept = default;

	template <::std::ranges::range R>
	inline explicit constexpr stack(::fast_io::freestanding::from_range_t, R &&rg)
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

	inline constexpr reference top()
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_top_may_throw<container_type, false>)
	{
		if constexpr (requires() {
						  { container.back() } -> ::std::same_as<reference>;
					  })
		{
			return container.back();
		}
		else
		{
			return container.front();
		}
	}
	inline constexpr const_reference top() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_top_may_throw<container_type, true>)
	{
		if constexpr (requires() {
						  { container.back() } -> ::std::same_as<const_reference>;
					  })
		{
			return container.back();
		}
		else
		{
			return container.front();
		}
	}

	inline constexpr reference top_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_top_unchecked_may_throw<container_type, false>)
	{
		if constexpr (requires() {
						  { container.back_unchecked() } -> ::std::same_as<reference>;
					  })
		{
			return container.back_unchecked();
		}
		else
		{
			return container.front_unchecked();
		}
	}
	inline constexpr const_reference top_unchecked() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_top_unchecked_may_throw<container_type, true>)
	{
		if constexpr (requires() {
						  { container.back_unchecked() } -> ::std::same_as<const_reference>;
					  })
		{
			return container.back_unchecked();
		}
		else
		{
			return container.front_unchecked();
		}
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
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_push_may_throw<container_type, value_type const &>)
	{
		if constexpr (requires() {
						  { container.push_back(value) };
						  { container.back() } -> ::std::same_as<reference>;
					  })
		{
			container.push_back(value);
		}
		else
		{
			container.push_front(value);
		}
	}

	inline constexpr void push(value_type &&value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_push_may_throw<container_type, value_type>)
	{
		if constexpr (requires() {
						  { container.push_back(::std::move(value)) };
						  { container.back() } -> ::std::same_as<reference>;
					  })
		{
			container.push_back(::std::move(value));
		}
		else
		{
			container.push_front(::std::move(value));
		}
	}

	inline constexpr void reserve(size_type newcap)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().reserve(::std::declval<size_type>()))
	{
		container.reserve(newcap);
	}

	inline constexpr void push_unchecked(value_type const &value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_push_unchecked_may_throw<container_type, value_type const &>)
	{
		if constexpr (requires() {
						  { container.push_back(value) };
						  { container.back() } -> ::std::same_as<reference>;
					  })
		{
			container.push_back_unchecked(value);
		}
		else
		{
			container.push_front_unchecked(value);
		}
	}

	inline constexpr void push_unchecked(value_type &&value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_push_unchecked_may_throw<container_type, value_type>)
	{
		if constexpr (requires() {
						  { container.push_back_unchecked(::std::move(value)) };
						  { container.back() } -> ::std::same_as<reference>;
					  })
		{
			container.push_back_unchecked(::std::move(value));
		}
		else
		{
			container.push_front_unchecked(::std::move(value));
		}
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_emplace_may_throw<container_type, Args &&...>)
	{
		if constexpr (requires() {
						  { container.emplace_back(::std::forward<Args>(args)...) };
						  { container.back() } -> ::std::same_as<reference>;
					  })
		{
			return container.emplace_back(::std::forward<Args>(args)...);
		}
		else
		{
			return container.emplace_front(::std::forward<Args>(args)...);
		}
	}

	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_unchecked(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_emplace_unchecked_may_throw<container_type, Args &&...>)
	{
		if constexpr (requires() {
						  { container.emplace_back_unchecked(::std::forward<Args>(args)...) };
						  { container.back() } -> ::std::same_as<reference>;
					  })
		{
			return container.emplace_back_unchecked(::std::forward<Args>(args)...);
		}
		else
		{
			return container.emplace_front_unchecked(::std::forward<Args>(args)...);
		}
	}

	template <::std::ranges::range R>
	inline constexpr void push_range(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::std::declval<container_type &>().append_range(::std::declval<R &&>())) ||
									   !noexcept(::std::declval<container_type &>().prepend_range(::std::declval<R &&>())))
	{
		if constexpr (requires() {
						  { container.append_range(::std::forward<R>(rg)) };
					  })
		{
			container.append_range(::std::forward<R>(rg));
		}
		else
		{
			container.prepend_range(::std::forward<R>(rg));
		}
	}

	inline constexpr void pop()
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_pop_may_throw<container_type>)
	{
		if constexpr (requires() {
						  { container.pop_back() };
					  })
		{
			container.pop_back();
		}
		else
		{
			container.pop_front();
		}
	}

	inline constexpr void pop_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_pop_unchecked_may_throw<container_type>)
	{
		if constexpr (requires() {
						  { container.pop_back_unchecked() };
					  })
		{
			container.pop_back_unchecked();
		}
		else
		{
			container.pop_front_unchecked();
		}
	}

	inline constexpr value_type pop_element()
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_pop_element_may_throw<container_type>)
		requires(::std::move_constructible<value_type>)
	{
		value_type top(::std::move(container.back()));
		if constexpr (requires() {
						  { container.pop_back() };
					  })
		{
			if constexpr (requires() {
							  { container.pop_back_unchecked() };
						  })
			{
				container.pop_back_unchecked();
			}
			else
			{
				container.pop_back();
			}
		}
		else
		{
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
		}
		return top;
	}

	inline constexpr value_type pop_element_unchecked()
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::stack_pop_element_unchecked_may_throw<container_type>)
		requires(::std::move_constructible<value_type>)
	{
		value_type top(::std::move(container.back_unchecked()));
		if constexpr (requires() {
						  { container.pop_back() };
					  })
		{
			if constexpr (requires() {
							  { container.pop_back_unchecked() };
						  })
			{
				container.pop_back_unchecked();
			}
			else
			{
				container.pop_back();
			}
		}
		else
		{
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
		}
		return top;
	}

	inline constexpr void clear() noexcept
	{
		container.clear();
	}
	inline constexpr void clear_destroy() noexcept
	{
		container.clear_destroy();
	}

	inline constexpr void swap(::fast_io::containers::stack<Container> &b)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<container_type &>().swap(::std::declval<container_type &>()))
	{
		container.swap(b.container);
	}
};

template <typename Container>
inline constexpr bool operator==(::fast_io::containers::stack<Container> const &a, ::fast_io::containers::stack<Container> const &b)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<Container const &>() == ::std::declval<Container const &>())
{
	return a.container == b.container;
}

template <typename Container>
inline constexpr auto operator<=>(::fast_io::containers::stack<Container> const &a, ::fast_io::containers::stack<Container> const &b)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<Container const &>() <=> ::std::declval<Container const &>())
{
	return a.container <=> b.container;
}

template <typename Container>
inline constexpr void swap(::fast_io::containers::stack<Container> &a, ::fast_io::containers::stack<Container> &b)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::std::declval<::fast_io::containers::stack<Container> &>().swap(::std::declval<::fast_io::containers::stack<Container> &>()))
{
	a.swap(b);
}

} // namespace containers

} // namespace fast_io
