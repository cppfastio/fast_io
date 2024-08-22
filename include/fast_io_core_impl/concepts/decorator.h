#pragma once

namespace fast_io
{

template <typename T>
concept decorator = requires(T t, ::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::byte const *tofirst,
							 ::std::byte const *tolast) {
	{ t.deco_define(fromfirst, fromlast, tofirst, tolast) } -> ::std::same_as<context_print_result<::std::byte *>>;
	{ t.deco_unshift_define(tofirst, tolast) } -> ::std::same_as<context_print_result<::std::byte *>>;
};

} // namespace fast_io
