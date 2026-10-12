#pragma once

#include <avr/sfr_defs.h>
#include <avr/io.h>

namespace fast_io
{
inline constexpr ::std::uint_least32_t avr_band_rate{9600};
inline constexpr ::std::uint_least32_t avr_f_cpu{
#if defined(F_CPU)
	F_CPU
#else
	16000000
#endif
};

inline constexpr ::std::uint_least32_t avr_band_prescale{avr_f_cpu / 16 / avr_band_rate - 1};

inline void avr_usart_initialize() noexcept
{
	UBRR0H = (avr_band_prescale >> 8);
	UBRR0L = avr_band_prescale;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

inline void avr_usart_send_byte(char unsigned c) noexcept
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

inline char unsigned avr_usart_recv_byte() noexcept
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}

template <::std::integral ch_type>
struct basic_avr_console
{
	using char_type = ch_type;
	using output_char_type = char_type;
};

inline void avr_usart_console_write(char const *first, ::std::size_t count) noexcept
{
	for (auto const e{first + count}; first != e; ++first)
	{
		avr_usart_send_byte(static_cast<char unsigned>(*first));
	}
}

inline void avr_usart_console_writev(fast_io::io_scatter_t const *scatter, ::std::size_t n) noexcept
{
	for (::std::size_t i{}; i != n; ++i)
	{
		auto b{reinterpret_cast<char const *>(scatter[i].base)};
		avr_usart_console_write(b, scatter[i].len);
	}
}

template <::std::integral char_type>
inline constexpr basic_avr_console<char_type> io_value_handle(basic_avr_console<char_type>) noexcept
{
	return {};
}

template <::std::integral char_type>
inline void write_all_overflow_define(basic_avr_console<char_type>, char_type const *first, ::std::size_t count) noexcept
{
	avr_usart_console_write(reinterpret_cast<char const *>(first), count * sizeof(char_type));
}

template <::std::integral char_type>
inline void scatter_write_all_overflow_define(basic_avr_console<char_type>,
											  ::fast_io::basic_io_scatter_t<char_type> const *pscatters,
											  ::std::size_t n) noexcept
{
	for (auto const e{pscatters + n}; pscatters != e; ++pscatters)
	{
		avr_usart_console_write(reinterpret_cast<char const *>(pscatters->base), pscatters->len * sizeof(char_type));
	}
}

using avr_console = basic_avr_console<char>;
using wavr_console = basic_avr_console<wchar_t>;
using u16avr_console = basic_avr_console<char16_t>;
using u32avr_console = basic_avr_console<char32_t>;
using u8avr_console = basic_avr_console<char8_t>;

} // namespace fast_io
