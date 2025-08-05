#pragma once

#include <cstddef>
#include <concepts>
#include <utility>

namespace fast_io::containers
{

namespace details
{

#if __cpp_pack_indexing < 202311L

template <::std::size_t I, typename First, typename... Rest>
	requires(I <= sizeof...(Rest))
struct pack_indexing_before_cxx26_
{
	using type = typename pack_indexing_before_cxx26_<I - 1, Rest...>::type;
};

template <typename First, typename... Rest>
struct pack_indexing_before_cxx26_<0, First, Rest...>
{
	using type = First;
};

#endif // __cpp_pack_indexing < 202311L

template <::std::size_t I, typename... Args>
struct pack_indexing_
{
#if __cpp_pack_indexing >= 202311L
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++26-extensions"
#endif
	using type = Args...[I];
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#else  // ^^^ __cpp_pack_indexing >= 202311L / vvv __cpp_pack_indexing < 202311L
	using type = typename pack_indexing_before_cxx26_<I, Args...>::type;
#endif // ^^^ __cpp_pack_indexing < 202311L
};

template <::std::size_t I, typename... Args>
	requires(I < sizeof...(Args))
using pack_indexing_t_ = typename pack_indexing_<I, Args...>::type;

template <::std::size_t I, typename T>
struct tuple_element_impl_
{
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	T val_;
};

template <typename T>
struct pass_type_
{
	using type = T;
};

template <typename... Args, ::std::size_t... Index>
	requires(sizeof...(Args) == sizeof...(Index))
[[nodiscard]]
constexpr auto get_tuple_impl_(::std::index_sequence<Index...>) noexcept
{
	struct tuple_impl_ : tuple_element_impl_<Index, Args>...
	{};

	return ::fast_io::containers::details::pass_type_<tuple_impl_>();
}

} // namespace details

template <typename... Args>
struct tuple : ::std::remove_cvref_t<typename decltype(::fast_io::containers::details::get_tuple_impl_<Args...>(
				   ::std::make_index_sequence<sizeof...(Args)>{}))::type>
{};

template <>
struct tuple<>
{};

template <typename... Args>
tuple(Args &&...) -> tuple<Args...>;

template <::std::size_t I, typename... Args>
FAST_IO_GNU_ALWAYS_INLINE
[[nodiscard]]
constexpr auto&& get(::fast_io::containers::tuple<Args...> &self) noexcept
{
	return static_cast<::fast_io::containers::details::tuple_element_impl_<I, ::fast_io::containers::details::pack_indexing_t_<I, Args...>> &>(self).val_;
}

template <::std::size_t I, typename... Args>
FAST_IO_GNU_ALWAYS_INLINE
[[nodiscard]]
constexpr auto&& get(::fast_io::containers::tuple<Args...> const &self) noexcept
{
	return static_cast<::fast_io::containers::details::tuple_element_impl_<I, ::fast_io::containers::details::pack_indexing_t_<I, Args...>> const &>(self).val_;
}

template <::std::size_t I, typename... Args>
FAST_IO_GNU_ALWAYS_INLINE
[[nodiscard]]
constexpr auto&& get(::fast_io::containers::tuple<Args...> &&self) noexcept
{
	return ::std::move(
		static_cast<::fast_io::containers::details::tuple_element_impl_<I, ::fast_io::containers::details::pack_indexing_t_<I, Args...>> &&>(self).val_);
}

template <::std::size_t I, typename... Args>
FAST_IO_GNU_ALWAYS_INLINE
[[nodiscard]]
constexpr auto&& get(::fast_io::containers::tuple<Args...> const &&self) noexcept
{
	return ::std::move(
		static_cast<::fast_io::containers::details::tuple_element_impl_<I, ::fast_io::containers::details::pack_indexing_t_<I, Args...>> const &&>(self).val_);
}

namespace details
{

template <typename T, ::std::size_t I, typename Current, typename... Args>
constexpr auto get_tuple_element_by_type_() noexcept
{
	if constexpr (::std::same_as<T, Current>)
	{
		return ::fast_io::containers::details::pass_type_<tuple_element_impl_<I, Current>>{};
	}
	else
	{
		return ::fast_io::containers::details::get_tuple_element_by_type_<T, I + 1, Args...>();
	}
}

} // namespace details

template <typename T, typename... Args>
	requires((::std::same_as<T, Args> + ...) == 1)
FAST_IO_GNU_ALWAYS_INLINE
[[nodiscard]]
constexpr auto&& get(::fast_io::containers::tuple<Args...> const &self) noexcept
{
	return static_cast<decltype(::fast_io::containers::details::get_tuple_element_by_type_<T, 0, Args...>())::type const &>(self).val_;
}

template <typename T, typename... Args>
	requires((::std::same_as<T, Args> + ...) == 1)
FAST_IO_GNU_ALWAYS_INLINE
[[nodiscard]]
constexpr auto&& get(::fast_io::containers::tuple<Args...> const &&self) noexcept
{
	return ::std::move(
		static_cast<decltype(::fast_io::containers::details::get_tuple_element_by_type_<T, 0, Args...>())::type const &&>(self).val_);
}

namespace details
{

template <typename T>
constexpr bool is_tuple_ = false;

template <typename... Args>
constexpr bool is_tuple_<tuple<Args...>> = true;

} // namespace details

template <typename T>
concept is_tuple = ::fast_io::containers::details::is_tuple_<::std::remove_cvref_t<T>>;

template<typename... Args>
[[nodiscard]]
constexpr auto forward_as_tuple(Args&&... args)
{
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
	return ::fast_io::containers::tuple<Args&&...>{::std::forward<Args>(args)...};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

} // namespace fast_io::containers

template <::std::size_t I, typename... Args>
struct std::tuple_element<I, ::fast_io::containers::tuple<Args...>>
{
	using type = ::fast_io::containers::details::pack_indexing_t_<I, Args...>;
};

template <typename... Args>
struct std::tuple_size<::fast_io::containers::tuple<Args...>>
{
	static constexpr ::std::size_t value = sizeof...(Args);
};
