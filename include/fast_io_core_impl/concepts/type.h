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

using io_scatter_t = ::fast_io::basic_io_scatter_t<void>;
using io_scatters_t = ::fast_io::basic_io_scatter_t<io_scatter_t>;

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

using message_hdr = ::fast_io::basic_message_hdr<void>;
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
	::fast_io::basic_io_scatter_t<value_type> scatter;
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
	::fast_io::basic_io_scatter_t<char_type> *scatters_pos_ptr;
	char_type *reserve_pos_ptr;
};

struct io_alias_t
{
	inline explicit constexpr io_alias_t() noexcept = default;
};

inline constexpr ::fast_io::io_alias_t io_alias{};

template <::std::integral char_type>
struct io_alias_type_t
{
	inline explicit constexpr io_alias_type_t() noexcept = default;
};

template <::std::integral char_type>
inline constexpr ::fast_io::io_alias_type_t<char_type> io_alias_type{};

template <::std::integral char_type>
struct try_get_result
{
	char_type ch;
	bool eof;
};

enum class seekdir : ::std::uint_least8_t
{
	beg = 0, // SEEK_SET
	cur = 1, // SEEK_CUR
	end = 2, // SEEK_END
};

using uintfpos_t = ::std::conditional_t<(sizeof(::std::size_t) < sizeof(::std::uint_fast64_t)), ::std::uint_fast64_t, ::std::size_t>;
using intfpos_t = ::std::make_signed_t<uintfpos_t>;
using uint32_size_based_t = ::std::conditional_t<(sizeof(::std::size_t) < sizeof(::std::uint_fast32_t)), ::std::size_t, ::std::uint_fast32_t>;
using int32_size_based_t = ::std::make_signed_t<uint32_size_based_t>;

// the smaller of uint_least32_t and size_t: 32-bit on hosted targets, size_t on
// platforms where size_t is already no wider than uint_least32_t
using size32_t = ::std::conditional_t<(sizeof(::std::uint_least32_t) < sizeof(::std::size_t)),
									  ::std::uint_least32_t, ::std::size_t>;

/*
A nullable file position carried by value, for interfaces where the
position is submitted to the kernel rather than read back through a
pointer (async submission, io_uring-style sqe offsets):
- has_opt == false means "use and advance the object's own file position";
- has_opt == true means "start at opt"; the object's own position is
  untouched.
*/
struct intfpos_opt
{
	::fast_io::intfpos_t opt{};
	bool has_opt{false};
	inline constexpr intfpos_opt() noexcept = default;
	inline constexpr intfpos_opt(::fast_io::intfpos_t fpos) noexcept
		: opt{fpos}, has_opt{true}
	{
	}
	inline constexpr intfpos_opt(::fast_io::intfpos_t fpos, bool has) noexcept
		: opt{fpos}, has_opt{has}
	{
	}
};

/*
A nullable file-position pointer. It carries the same semantics as the
`off_t *_Nullable` parameters of copy_file_range(2)/splice(2):
- a null pointer means "use and advance the object's own file position";
- a non-null pointer means "start at *ptr"; *ptr is updated to point just past
  the last byte transferred, while the object's own position stays untouched.
*/
struct fpos_nullable_ptr
{
	::fast_io::intfpos_t *ptr{};
	inline constexpr fpos_nullable_ptr() noexcept = default;
	inline constexpr fpos_nullable_ptr(::std::nullptr_t) noexcept {}
	inline constexpr fpos_nullable_ptr(::fast_io::intfpos_t *fposptr) noexcept
		: ptr{fposptr}
	{}
	inline constexpr fpos_nullable_ptr(::fast_io::intfpos_t &fposref) noexcept
		: ptr{__builtin_addressof(fposref)}
	{}
	inline constexpr explicit operator bool() const noexcept
	{
		return ptr != nullptr;
	}
};

/*
An optional byte count for transmit_all_bytes: has_opt == false means
"transmit until EOF"; has_opt == true means "transmit up to opt bytes".
*/
struct size_t_opt
{
	::std::size_t opt{};
	bool has_opt{false};
	inline constexpr size_t_opt() noexcept = default;
	inline constexpr size_t_opt(::std::size_t optsize) noexcept
		: opt{optsize}, has_opt{true}
	{}
	inline constexpr size_t_opt(::std::size_t optsize, bool has) noexcept
		: opt{optsize}, has_opt{has}
	{}
};

struct posix_statx_timestamp64
{
	::std::int_least64_t tv_sec;   // Seconds since the Epoch (UNIX time)
	::std::uint_least32_t tv_nsec; // Nanoseconds since tv_sec

	template <::std::floating_point flt_type>
	inline explicit constexpr operator flt_type() const noexcept
	{
		// I know this is not accurate. but it is better than nothing
		return static_cast<flt_type>(tv_sec) + static_cast<flt_type>(tv_nsec) / static_cast<flt_type>(1000000000u);
	}
};

inline constexpr bool operator==(posix_statx_timestamp64 a, posix_statx_timestamp64 b) noexcept
{
	return (a.tv_sec == b.tv_sec) & (a.tv_nsec == b.tv_nsec);
}

#if defined(__cpp_lib_three_way_comparison) && __cpp_lib_three_way_comparison >= 201907L
inline constexpr auto operator<=>(posix_statx_timestamp64 a, posix_statx_timestamp64 b) noexcept
{
	auto v{a.tv_sec <=> b.tv_sec};
	if (v == ::std::strong_ordering::equal)
	{
		return a.tv_nsec <=> b.tv_nsec;
	}
	return v;
}
#endif

/*
An optional timeout for asynchronous operations: has_opt == false means
"no timeout"; has_opt == true means the operation fails with a timeout
error once opt elapses (or, where the backend interprets it as a
timepoint, once opt passes — posix_statx_timestamp64 carries both).
*/
struct posix_statx_timestamp_opt
{
	::fast_io::posix_statx_timestamp64 opt{};
	bool has_opt{false};
	inline constexpr posix_statx_timestamp_opt() noexcept = default;
	inline constexpr posix_statx_timestamp_opt(::fast_io::posix_statx_timestamp64 ts) noexcept
		: opt{ts}, has_opt{true}
	{
	}
	inline constexpr posix_statx_timestamp_opt(::fast_io::posix_statx_timestamp64 ts, bool has) noexcept
		: opt{ts}, has_opt{has}
	{
	}
};

struct io_construct_t
{
	inline explicit constexpr io_construct_t() noexcept = default;
};

inline constexpr ::fast_io::io_construct_t io_construct{};

template <typename T>
struct io_cookie_type_t
{
	explicit inline constexpr io_cookie_type_t() noexcept = default;
};

template <typename T>
inline constexpr ::fast_io::io_cookie_type_t<T> io_cookie_type{};

struct io_cookie_t
{
	explicit inline constexpr io_cookie_t() noexcept = default;
};

inline constexpr ::fast_io::io_cookie_t io_cookie{};

struct io_null_t
{
	explicit inline constexpr io_null_t() noexcept = default;
};

inline constexpr ::fast_io::io_null_t io_null{};

struct io_nothrow_tag
{
	explicit inline constexpr io_nothrow_tag() noexcept = default;
};

struct scan_some_result_t
{
	::std::size_t remained_args{};
	constexpr operator bool() const noexcept
	{
		return !remained_args;
	}
};

#if defined(__HERBCEPTIONS__)
namespace details
{

/* callback-type aliases for the async has_*_callback_define probes. The
 * lambdas are spelled at namespace scope on purpose: a lambda declared
 * inside a requires-clause has the constrained function as its decl
 * context, so mangling the trait specialization drags the still-dependent
 * function pattern into the ABI mangler — which MSVC's mangler rejects
 * when the pattern still contains a pack expansion */
using async_io_callback = decltype([](::std::cxx_std_error, ::std::size_t) noexcept {});
using async_io_scatter_callback =
	decltype([](::std::cxx_std_error, io_scatter_status_t) noexcept {});
using async_io_error_callback = decltype([](::std::cxx_std_error) noexcept {});
template <typename handletype>
using async_io_accept_callback = decltype([](::std::cxx_std_error, handletype) noexcept {});

} // namespace details
#endif

} // namespace fast_io
