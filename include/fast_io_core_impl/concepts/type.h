#pragma once

namespace fast_io
{

template <typename T>
struct basic_io_scatter_t
{
	using value_type = T;
	T const *base;
	::std::size_t len;
};

// should be binary compatible with POSIX's iovec

using io_scatter_t = basic_io_scatter_t<void>;
using io_scatters_t = basic_io_scatter_t<io_scatter_t>;

struct io_scatter_status_t
{
	::std::size_t position;
	::std::size_t position_in_scatter;
};

template <typename T>
struct basic_message_hdr
{
	T const *name;                    /* Optional address */
	::std::size_t namelen;            /* Size of address */
	basic_io_scatter_t<T> const *iov; /* Scatter/gather array */
	::std::size_t iovlen;             /* # elements in msg_iov */
	T const *control;                 /* Ancillary data, see below */
	::std::size_t controllen;         /* Ancillary data buffer len */
	int flags;                        /* Flags (unused) */
	
	inline operator basic_message_hdr<void>() const noexcept
		requires(!::std::same_as<T, void>)
	{
		/// @error: Should modify the internal size of basic_io_scatter_t instead of multiplying by the size of T
		return {name, namelen * sizeof(T), iov, iovlen, control, controllen, flags};
	}
};

using message_hdr = basic_message_hdr<void>;
// should be binary compatible with POSIX's msghdr

template <typename T>
struct io_type_t
{
	using type = T;
};
template <typename T>
inline constexpr io_type_t<T> io_type{};

template <::std::integral char_type>
struct cross_code_cvt_t
{
	using value_type = char_type;
	basic_io_scatter_t<value_type> scatter;
};

template <::std::integral char_type, typename T>
struct io_reserve_type_t
{
	inline explicit constexpr io_reserve_type_t() noexcept = default;
};
template <::std::integral char_type, typename T>
inline constexpr io_reserve_type_t<char_type, T> io_reserve_type{};

struct reserve_scatters_size_result
{
	::std::size_t scatters_size;
	::std::size_t reserve_size;
};

template <::std::integral char_type>
struct basic_reserve_scatters_define_result
{
	basic_io_scatter_t<char_type> *scatters_pos_ptr;
	char_type *reserve_pos_ptr;
};

struct io_alias_t
{
	inline explicit constexpr io_alias_t() noexcept = default;
};

inline constexpr io_alias_t io_alias{};

template <::std::integral char_type>
struct io_alias_type_t
{
	inline explicit constexpr io_alias_type_t() noexcept = default;
};

template <::std::integral char_type>
inline constexpr io_alias_type_t<char_type> io_alias_type{};

template <::std::integral char_type>
struct try_get_result
{
	char_type ch;
	bool eof;
};

template <typename in_char_type, typename out_char_type>
struct deco_result
{
	in_char_type const *input_result_ptr{};
	out_char_type *output_result_ptr{};
};

enum class seekdir
{
	beg = 0, // SEEK_SET
	cur = 1, // SEEK_CUR
	end = 2, // SEEK_END
};

using uintfpos_t = ::std::uintmax_t;
using intfpos_t = ::std::intmax_t;

struct io_construct_t
{
	inline explicit constexpr io_construct_t() noexcept = default;
};

inline constexpr io_construct_t io_construct{};

template <typename T>
struct io_cookie_type_t
{
	explicit inline constexpr io_cookie_type_t() noexcept = default;
};

template <typename T>
inline constexpr io_cookie_type_t<T> io_cookie_type{};

struct io_cookie_t
{
	explicit inline constexpr io_cookie_t() noexcept = default;
};

inline constexpr io_cookie_t io_cookie{};

struct io_null_t
{
	explicit inline constexpr io_null_t() noexcept = default;
};

inline constexpr io_null_t io_null{};

} // namespace fast_io
