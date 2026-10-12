#pragma once


namespace fast_io
{

// boolalpha bool — yesstr/nostr from LC_MESSAGES
template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags>
	requires(flags.alphabet)
inline constexpr ::fast_io::basic_io_scatter_t<char_type>
print_scatter_define(lc_ctx<char_type> const *ctx,
					 ::fast_io::manipulators::scalar_manip_t<flags, bool> val)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ctx->sc(val.reference ? ctx->all->messages.yesstr
								 : ctx->all->messages.nostr);
}

} // namespace fast_io

