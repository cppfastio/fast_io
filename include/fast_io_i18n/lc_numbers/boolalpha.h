#pragma once

namespace fast_io
{

template<std::integral char_type,manipulators::scalar_flags flags>
requires (flags.alphabet)
inline constexpr basic_io_scatter_t<char_type> print_scatter_define(basic_lc_all<char_type> const* __restrict all,manipulators::scalar_manip_t<flags,bool> val) noexcept
{
	if(val.reference)
		return all->messages.yesstr;
	else
		return all->messages.nostr;
}

}
