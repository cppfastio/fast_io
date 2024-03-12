#pragma once

namespace fast_io
{

namespace containers
{

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

	constexpr stack() noexcept = default;

#ifdef __cpp_lib_containers_ranges
	template <::std::ranges::range R>
	explicit constexpr stack(::std::from_range_t, R &&rg)
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

	constexpr reference top() noexcept
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
	constexpr const_reference top() const noexcept
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

	constexpr reference top_unchecked() noexcept
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
	constexpr const_reference top_unchecked() const noexcept
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

	constexpr void push(value_type &&value)
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

	constexpr void push_unchecked(value_type const &value)
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

	constexpr void push_unchecked(value_type &&value)
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
	constexpr reference emplace(Args &&...args) noexcept
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
	constexpr reference emplace_unchecked(Args &&...args) noexcept
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
	constexpr void push_range(R &&rg)
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

	constexpr void pop() noexcept
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

	constexpr void pop_unchecked() noexcept
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

	constexpr value_type pop_element() noexcept
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

	constexpr value_type pop_element_unchecked() noexcept
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

	constexpr void clear() noexcept
	{
		container.clear();
	}
	constexpr void clear_destroy() noexcept
	{
		container.clear_destroy();
	}

	constexpr void swap(::fast_io::containers::stack<Container> &b) noexcept
	{
		container.swap(b.container);
	}
};

template <typename Container>
constexpr bool operator==(::fast_io::containers::stack<Container> const &a, ::fast_io::containers::stack<Container> const &b) noexcept
{
	return a.container == b.container;
}

template <typename Container>
constexpr auto operator<=>(::fast_io::containers::stack<Container> const &a, ::fast_io::containers::stack<Container> const &b) noexcept
{
	return a.container <=> b.container;
}

template <typename Container>
constexpr void swap(::fast_io::containers::stack<Container> &a, ::fast_io::containers::stack<Container> &b) noexcept
{
	a.swap(b);
}

} // namespace containers

} // namespace fast_io
