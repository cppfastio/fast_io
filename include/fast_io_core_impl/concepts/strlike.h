#pragma once

namespace fast_io
{

template <::std::integral char_type, typename T>
struct io_strlike_type_t
{
	inline explicit constexpr io_strlike_type_t() noexcept = default;
};

template <::std::integral char_type, typename T>
inline constexpr io_strlike_type_t<char_type, T> io_strlike_type{};

namespace details
{
template <typename char_type, typename T>
concept buffer_strlike_impl = requires(T &t) {
	{ strlike_begin(::fast_io::io_strlike_type<char_type, T>, t) };
	{ strlike_curr(::fast_io::io_strlike_type<char_type, T>, t) };
	{ strlike_end(::fast_io::io_strlike_type<char_type, T>, t) };
	requires requires(decltype(strlike_begin(::fast_io::io_strlike_type<char_type, T>, t)) ptr) {
		{ strlike_set_curr(::fast_io::io_strlike_type<char_type, T>, t, ptr) };
	};
	requires requires(::std::size_t n) {
		{ strlike_reserve(::fast_io::io_strlike_type<char_type, T>, t, n) };
	};
};
} // namespace details

template <typename char_type, typename T>
concept strlike =
	::std::integral<char_type> && ::std::is_default_constructible_v<T> && (requires(char_type const *first) {
		strlike_construct_define(::fast_io::io_strlike_type<char_type, T>, first, first);
	} || ::fast_io::details::buffer_strlike_impl<char_type, T>);

template <typename char_type, typename T>
concept nothrow_strlike = ::fast_io::strlike<char_type, T> && requires(char_type const *first) {
	{ strlike_construct_define(::fast_io::io_strlike_type<char_type, T>, first, first) } noexcept;
};

template <typename char_type, typename T>
concept single_character_constructible_strlike = ::fast_io::strlike<char_type, T> && requires(char_type ch) {
	strlike_construct_single_character_define(::fast_io::io_strlike_type<char_type, T>, ch);
};

template <typename char_type, typename T>
concept alias_strlike = requires(T &t) { strlike_alias_define(io_alias, t); };

template <typename char_type, typename T>
concept buffer_strlike = strlike<char_type, T> && requires(T &t) {
	{ strlike_begin(::fast_io::io_strlike_type<char_type, T>, t) };
	{ strlike_curr(::fast_io::io_strlike_type<char_type, T>, t) };
	{ strlike_end(::fast_io::io_strlike_type<char_type, T>, t) };
	requires requires(decltype(strlike_begin(::fast_io::io_strlike_type<char_type, T>, t)) ptr) {
		{ strlike_set_curr(::fast_io::io_strlike_type<char_type, T>, t, ptr) };
	};
	requires requires(::std::size_t n) {
		{ strlike_reserve(::fast_io::io_strlike_type<char_type, T>, t, n) };
	};
};

template <typename char_type, typename T>
concept auxiliary_strlike = strlike<char_type, T> && requires(T &t, char_type ch, char_type const *ptr) {
	strlike_push_back(::fast_io::io_strlike_type<char_type, T>, t, ch);
	strlike_append(::fast_io::io_strlike_type<char_type, T>, t, ptr, ptr);
};

template <typename char_type, typename T>
concept sso_buffer_strlike = buffer_strlike<char_type, T> &&
							 requires {
								 {
									 strlike_sso_size(::fast_io::io_strlike_type<char_type, T>)
								 } -> ::std::same_as<::std::size_t>;
							 };

namespace details
{

// whether mutating a strlike (reserve/push_back/append) can report failure
template <typename char_type, typename T>
concept strlike_reserve_may_throw = !requires(T &t, ::std::size_t n) {
	{ strlike_reserve(::fast_io::io_strlike_type<char_type, T>, t, n) } noexcept;
};

template <typename char_type, typename T>
concept strlike_push_back_may_throw = !requires(T &t, char_type ch) {
	{ strlike_push_back(::fast_io::io_strlike_type<char_type, T>, t, ch) } noexcept;
};

template <typename char_type, typename T>
concept strlike_append_may_throw = !requires(T &t, char_type const *ptr) {
	{ strlike_append(::fast_io::io_strlike_type<char_type, T>, t, ptr, ptr) } noexcept;
};

// whether writing through a strlike used as an output buffer can report failure
template <typename char_type, typename T>
concept strlike_output_ops_may_throw =
	strlike_reserve_may_throw<char_type, T> ||
	(::fast_io::auxiliary_strlike<char_type, T> &&
	 (strlike_push_back_may_throw<char_type, T> || strlike_append_may_throw<char_type, T>));

} // namespace details

} // namespace fast_io
