// lcblob.cc — compiled locale-blob cache for the fast_io_i18n library.
//
// Two-level cache of mmap'd lcblob files; entries are leaked on purpose —
// locale data is never unloaded, so a reload is a pointer lookup:
//
//   thread_local map  (no lock, no syscall)
//        |
//        miss
//        v
//   global map        (native_mutex; one entry per file per process)
//        |
//        miss
//        v
//   native_file_loader mmap -> lc_verify the whole image -> publish the
//   lc_locale pointer straight into the mapping
//
// The mapping is private copy-on-write (the default native_file_loader
// mode: PROT_READ|PROT_WRITE|MAP_PRIVATE on POSIX, PAGE_WRITECOPY/
// FILE_MAP_COPY on Windows). The file image is still shared across
// processes through the OS page cache, but no process can write back —
// users may also patch rva fields in their own mapping to customize
// fields without touching the shared image.
//
// name "" resolves to the system default locale: L10N/FAST_IO_L10N_LANG/LC_ALL/LANG on
// POSIX, GetUserDefaultLocaleName + the International registry key on
// Windows. "C"/"POSIX" are loaded from their blob files like any locale.

#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/str_swiss_map.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_i18n/lcblob.h>
// win32 user-override path transcodes registry text through
// code_cvt<utf_le, gb18030> — the gb18030 tables are a unit
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
#include <fast_io_unit/gb18030.h>
#endif

#if !defined(FAST_IO_FREESTANDING)

namespace fast_io::l10n
{

namespace lcblob = ::fast_io::l10n;


namespace details
{

#if (!defined(_WIN32) || defined(__WINE__)) && !defined(__CYGWIN__)
#if defined(_GNU_SOURCE) && !defined(__ANDROID__)
extern char const *libc_secure_getenv(char const *) noexcept __asm__("secure_getenv");
#else
extern char const *libc_getenv(char const *) noexcept __asm__("getenv");
#endif

inline char const *lc_getenv(char const *env) noexcept
{
	return
#if defined(_GNU_SOURCE) && !defined(__ANDROID__)
		libc_secure_getenv(env);
#else
		libc_getenv(env);
#endif
}
#endif

// One cached locale: the COW mapping. The published pointer targets the
// mapping itself — the loader just keeps it alive. Entries are
// allocated once through our own allocator and leaked on purpose:
// locale data is never unloaded, so a rehash can never invalidate
// anything a caller was handed.
struct locale_entry
{
	inline explicit locale_entry(::fast_io::native_file_loader &&l) noexcept
		: loader(static_cast<::fast_io::native_file_loader &&>(l))
	{
	}
	::fast_io::native_file_loader loader;
	// "" user-locale loads (Windows): a private copy of the image with
	// the registry overrides appended — the published lc_locale points
	// into it, not into the mapping
	::fast_io::vector<unsigned char> img;
};

using cache_map = ::fast_io::u8str_swiss_map<::fast_io::l10n::lc_locale const *>;

// allocate + construct one object through fast_io's own allocator —
// cache entries are never freed by design
template <typename T, typename... Args>
inline T *lc_new(Args &&...args) FAST_IO_HERBCEPTIONS_THROWS
{
	return new (::fast_io::native_typed_global_allocator<T>::allocate(1))
		T{::fast_io::freestanding::forward<Args>(args)...};
}

// plain globals. TLS first (no lock), then the process-wide map.
::fast_io::native_mutex global_mtx;
cache_map global_map;
thread_local cache_map tls_map;

struct cache_guard
{
	inline explicit cache_guard(::fast_io::native_mutex &m) FAST_IO_HERBCEPTIONS_THROWS : mtx{m}
	{
		mtx.lock();
	}
	inline ~cache_guard() noexcept
	{
		mtx.unlock();
	}
	::fast_io::native_mutex &mtx;
	cache_guard(cache_guard const &) = delete;
	cache_guard &operator=(cache_guard const &) = delete;
};

[[noreturn]] inline void throw_einval() FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::throw_posix_error(EINVAL);
}

// validate a resolved/normalized locale name against path injection
inline void check_name(::fast_io::u8string_view name) FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::size_t size_restriction{256u};
	if (name.is_empty() || name.size() >= size_restriction)
	{
		throw_einval();
	}
	for (char8_t ch : name)
	{
		if (ch == 0 || ch == u8'/' || ch == u8'\\')
		{
			throw_einval();
		}
	}
}

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)

inline ::fast_io::u8string u16_to_u8(char16_t const *s, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	// dst scheme defaults to execution_charset (utf8 for u8) — a
	// utf_le->utf_le cvt would dump the raw u16 units into the string
	return ::fast_io::u8concat_fast_io(
		::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le>(
			::fast_io::basic_io_scatter_t<char16_t>{s, n}));
}

// code_cvt concat must live in non-template functions — instantiating
// first_print_define_index_range for code_cvt_t inside a template
// context fails constexpr evaluation (clang expansion-statement bug)
inline ::fast_io::u8string u16_to_gb18030(char16_t const *s, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::u8concat_fast_io(
		::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
								 ::fast_io::encoding_scheme::gb18030>(
			::fast_io::basic_io_scatter_t<char16_t>{s, n}));
}

inline ::fast_io::u8string u16_to_ebcdic(char16_t const *s, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::u8concat_fast_io(
		::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
								 ::fast_io::encoding_scheme::utf_ebcdic>(
			::fast_io::basic_io_scatter_t<char16_t>{s, n}));
}

// env var as u8 text: W + transcode on NT, raw ANSI bytes on 9x.
// empty string means unset
inline ::fast_io::u8string env_u8([[maybe_unused]] char16_t const *wname, [[maybe_unused]] char const *aname) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(_WIN32_WINDOWS)
	char buf[512];
	auto const n{::fast_io::win32::GetEnvironmentVariableA(aname, buf, 512)};
	if (n == 0 || n >= 512)
	{
		return {};
	}
	::fast_io::u8string s;
	s.append(reinterpret_cast<char8_t const *>(buf), n);
	return s;
#else
	char16_t buf[512];
	auto const n{::fast_io::win32::GetEnvironmentVariableW(wname, buf, 512)};
	if (n == 0 || n >= 512)
	{
		return {};
	}
	return u16_to_u8(buf, n);
#endif
}

inline ::fast_io::u8string default_locale_name_win32(bool ignore_system_settings) FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::size_t locale_name_max{85}; // LOCALE_NAME_MAX_LENGTH
	char16_t buf[locale_name_max];
	if (int r{::fast_io::win32::GetUserDefaultLocaleName(buf, locale_name_max)}; r > 1)
	{
		return u16_to_u8(buf, static_cast<::std::size_t>(r - 1));
	}
	// registry fallback — skipped under ignore_system_settings
	if (!ignore_system_settings)
	{
		constexpr ::std::size_t hkcu{0x80000001u};
		::std::size_t hkey{};
		if (::fast_io::win32::RegOpenKeyW(hkcu, u"Control Panel\\International",
										  __builtin_addressof(hkey)) == 0)
		{
			::std::uint_least32_t bytes{sizeof(buf)};
			auto const res{::fast_io::win32::RegQueryValueExW(
				hkey, u"LocaleName", nullptr, nullptr, buf, __builtin_addressof(bytes))};
			::fast_io::win32::RegCloseKey(hkey);
			if (res == 0 && bytes >= 4)
			{
				return u16_to_u8(buf, (bytes / 2) - 1);
			}
		}
	}
	return {}; // "" resolution falls back to C like other platforms
}

#endif

// codeset text -> locale charset; normalized: lowercase, '-'/'_'/' '
// dropped (UTF-8, gb18030, UTF_EBCDIC ...). Only the three supported
// codesets are accepted.
inline void parse_codeset(::fast_io::u8string_view cs, lcblob::locale_charset &out) FAST_IO_HERBCEPTIONS_THROWS
{
	char buf[16];
	::std::size_t n{};
	for (char8_t ch : cs)
	{
		if (ch == u8'-' || ch == u8'_' || ch == u8' ')
		{
			continue;
		}
		if (n >= sizeof(buf))
		{
			throw_einval();
		}
		buf[n++] = static_cast<char>(ch >= u8'A' && ch <= u8'Z' ? ch + 0x20 : ch);
	}
	::fast_io::string_view sv{buf, n};
	if (sv == "utf8")
	{
		out = lcblob::locale_charset::utf8;
	}
	else if (sv == "gb18030")
	{
		out = lcblob::locale_charset::gb18030;
	}
	else if (sv == "utfebcdic")
	{
		out = lcblob::locale_charset::utf_ebcdic;
	}
	else
	{
		throw_einval();
	}
}

// "" -> system default. Parses the standard
// lang[_TERRITORY][.codeset][@modifier] form: '-' -> '_' (BCP-47 vs posix
// spellings), codeset is normalized (absent -> utf8), the modifier stays
// attached to the file basename.
// out = canonical file basename  base.codeset[@mod]

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)

// ---------------------------------------------------------------------------
// user-locale overrides ("" loads only): the image is copied into a
// process-private vector, the HKCU\Control Panel\International values
// are appended after it, and the affected field rvas are repointed —
// the blob is position-independent so a byte copy + append is all it
// takes. Each section gets its own charset encoding of every string.
// ---------------------------------------------------------------------------

enum class user_enc : ::std::uint_least8_t
{
	utf8,
	gb18030,
	utf16,
	utf32,
	ebcdic
};

// registry integer text -> i64; false on empty/non-numeric
inline bool parse_reg_int(char16_t const *buf, ::std::uint_least32_t n,
						  ::std::int_least64_t &out) noexcept
{
	if (n == 0)
	{
		return false;
	}
	::std::uint_least32_t i{};
	bool neg{};
	if (buf[0] == u'-')
	{
		neg = true;
		i = 1;
	}
	else if (buf[0] == u'+')
	{
		i = 1;
	}
	if (i >= n)
	{
		return false;
	}
	::std::int_least64_t v{};
	for (; i < n; ++i)
	{
		if (buf[i] < u'0' || buf[i] > u'9')
		{
			return false;
		}
		v = v * 10 + static_cast<::std::int_least64_t>(buf[i] - u'0');
	}
	out = neg ? -v : v;
	return true;
}

struct user_blob_builder
{
	::fast_io::vector<unsigned char> &img;

	inline ::std::uint_least32_t app(unsigned char const *p, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto const off{static_cast<::std::uint_least32_t>(img.size())};
		img.resize(img.size() + n);
		::fast_io::details::my_memcpy(img.data() + off, p, n);
		return off;
	}

	// encode registry u16 text into the section charset and append it;
	// returns the field value {off, units} (units in section units)
	template <user_enc enc>
	inline ::std::pair<::std::uint_least32_t, ::std::uint_least32_t>
	put_str(char16_t const *s, ::std::uint_least32_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if constexpr (enc == user_enc::utf16)
		{
			return {app(reinterpret_cast<unsigned char const *>(s), n * 2), n};
		}
		else if constexpr (enc == user_enc::utf32)
		{
			// u16 -> u32 code points, surrogate pairs decoded
			::fast_io::vector<unsigned char> tmp;
			tmp.reserve(n * 4 + 4);
			for (::std::uint_least32_t i{}; i < n;)
			{
				::std::uint_least32_t cp{static_cast<char16_t>(s[i++])};
				if (cp >= 0xD800u && cp <= 0xDBFFu && i < n)
				{
					auto const lo{static_cast<char16_t>(s[i])};
					if (lo >= 0xDC00u && lo <= 0xDFFFu)
					{
						++i;
						cp = 0x10000u + ((cp - 0xD800u) << 10u) + (lo - 0xDC00u);
					}
				}
				auto const le{lcblob::lc_u32(cp)};
				auto const pos{tmp.size()};
				tmp.resize(pos + 4);
				::fast_io::details::my_memcpy(tmp.data() + pos, __builtin_addressof(le), 4);
			}
			return {app(tmp.data(), tmp.size()),
					static_cast<::std::uint_least32_t>(tmp.size() / 4)};
		}
		else
		{
			::fast_io::u8string u8;
			if constexpr (enc == user_enc::gb18030)
			{
				u8 = u16_to_gb18030(s, n);
			}
			else if constexpr (enc == user_enc::ebcdic)
			{
				u8 = u16_to_ebcdic(s, n);
			}
			else
			{
				u8 = u16_to_u8(s, n);
			}
			return {app(reinterpret_cast<unsigned char const *>(u8.data()), u8.size()),
					static_cast<::std::uint_least32_t>(u8.size())};
		}
	}

	inline ::std::pair<::std::uint_least32_t, ::std::uint_least32_t>
	put_bytes(unsigned char const *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		return {app(p, n), static_cast<::std::uint_least32_t>(n)};
	}
};

struct intl_key
{
	inline intl_key() noexcept
	{
		constexpr ::std::size_t hkcu{0x80000001u};
		if (::fast_io::win32::RegOpenKeyW(hkcu, u"Control Panel\\International",
										  __builtin_addressof(hkey)) != 0)
		{
			hkey = 0;
		}
	}
	inline ~intl_key() noexcept
	{
		if (hkey != 0)
		{
			::fast_io::win32::RegCloseKey(hkey);
		}
	}
	::std::size_t hkey{};
};

// one registry value into buf; returns the u16 unit count, 0 when
// absent — every RegQueryValueExW failure absorbs to 0 (optional data)
inline ::std::uint_least32_t intl_get(::std::size_t hkey, char16_t const *vname,
									  char16_t *buf, ::std::uint_least32_t cap) noexcept
{
	if (hkey == 0)
	{
		return 0;
	}
	::std::uint_least32_t type{}, bytes{cap * 2};
	FAST_IO_HERBCEPTIONS_TRY
	{
		auto const res{::fast_io::win32::RegQueryValueExW(
			hkey, vname, nullptr, __builtin_addressof(type), buf,
			__builtin_addressof(bytes))};
		if (res == 0 && bytes >= 2)
		{
			auto const n{bytes / 2};
			return buf[n - 1] == 0 ? n - 1 : n; // strip the trailing NUL
		}
	}
	FAST_IO_HERBCEPTIONS_CATCH_ALL
	{
	}
	return 0;
}

// patch one section in place inside the private image. all is a
// basic_lc_all<char>* pointing into img — its rva fields are rewritten
// to point at the appended bytes
template <user_enc enc>
inline void patch_user_section(lcblob::basic_lc_all<char> *a,
							   user_blob_builder &b,
							   ::std::size_t hkey) FAST_IO_HERBCEPTIONS_THROWS
{
	char16_t buf[160];
	constexpr ::std::uint_least32_t cap{160};

	auto S{[&](char16_t const *vn, lcblob::lc_scatter<char> &f)
			   FAST_IO_HERBCEPTIONS_THROWS {
				   if (auto const n{intl_get(hkey, vn, buf, cap)}; n != 0)
				   {
					   auto const r{b.template put_str<enc>(buf, n)};
					   f.ref.off = lcblob::lc_u32(r.first);
					   f.len = lcblob::lc_u32(r.second);
				   }
			   }};
	S(u"sDecimal", a->numeric.decimal_point);
	S(u"sThousand", a->numeric.thousands_sep);
	S(u"sCurrency", a->monetary.currency_symbol);
	S(u"sMonDecimalSep", a->monetary.mon_decimal_point);
	S(u"sMonThousandSep", a->monetary.mon_thousands_sep);
	S(u"sPositiveSign", a->monetary.positive_sign);
	S(u"sNegativeSign", a->monetary.negative_sign);
	S(u"sCountry", a->address.country_name);
	S(u"s1159", a->time.am_pm[0]);
	S(u"s2359", a->time.am_pm[1]);

	// grouping byte lists: "3;0" -> {3} — the win32 trailing 0 means
	// "repeat the last value", glibc repeats too, so it drops out
	auto G{[&](char16_t const *vn, lcblob::lc_scatter<char8_t> &f)
			   FAST_IO_HERBCEPTIONS_THROWS {
				   auto const n{intl_get(hkey, vn, buf, cap)};
				   if (n == 0)
				   {
					   return;
				   }
				   char8_t gb[16];
				   ::std::size_t m{};
				   ::std::uint_least32_t v{};
				   bool has{};
				   for (::std::uint_least32_t i{}; i <= n; ++i)
				   {
					   if (i == n || buf[i] == u';')
					   {
						   if (!has)
						   {
							   return;
						   }
						   if (m < sizeof(gb))
						   {
							   gb[m++] = static_cast<char8_t>(v > 127 ? 127 : v);
						   }
						   v = 0;
						   has = false;
					   }
					   else if (buf[i] >= u'0' && buf[i] <= u'9')
					   {
						   v = v * 10 + static_cast<::std::uint_least32_t>(buf[i] - u'0');
						   has = true;
					   }
					   else
					   {
						   return;
					   }
				   }
				   while (m != 0 && gb[m - 1] == 0)
				   {
					   --m;
				   }
				   if (m != 0)
				   {
					   auto const r{b.put_bytes(reinterpret_cast<unsigned char const *>(gb), m)};
					   f.ref.off = lcblob::lc_u32(r.first);
					   f.len = lcblob::lc_u32(r.second);
				   }
			   }};
	G(u"sGrouping", a->numeric.grouping);
	G(u"sMonGrouping", a->monetary.mon_grouping);

	// integer fields
	auto I{[&](char16_t const *vn, ::std::int_least64_t &out) noexcept {
		auto const n{intl_get(hkey, vn, buf, cap)};
		return n != 0 && parse_reg_int(buf, n, out);
	}};
	auto const set_int{[](::std::int_least32_t &f, ::std::int_least64_t v) noexcept {
		f = static_cast<::std::int_least32_t>(
			lcblob::lc_u32(static_cast<::std::uint_least32_t>(v)));
	}};
	::std::int_least64_t v{};
	if (I(u"iMeasure", v) && (v == 0 || v == 1))
	{
		set_int(a->measurement.measurement, v + 1); // win32 0/1 -> glibc 1/2
	}
	if (I(u"iDigits", v))
	{
		set_int(a->monetary.frac_digits, v);
	}
	if (I(u"iCurrDigits", v))
	{
		set_int(a->monetary.int_frac_digits, v);
	}
	if (I(u"iCurrency", v) && v >= 0 && v <= 3)
	{
		// 0 "$1.1" 1 "1.1$" 2 "$ 1.1" 3 "1.1 $"
		auto const prec{static_cast<::std::int_least64_t>(v == 0 || v == 2)};
		auto const sep{v >= 2 ? 1 : 0};
		set_int(a->monetary.p_cs_precedes, prec);
		set_int(a->monetary.p_sep_by_space, sep);
		set_int(a->monetary.int_p_cs_precedes, prec);
		set_int(a->monetary.int_p_sep_by_space, sep);
	}
	if (I(u"iNegCurr", v) && v >= 0 && v <= 15)
	{
		// -> {n_cs_precedes, n_sep_by_space, n_sign_posn}
		constexpr ::std::int_least8_t tab[16][3]{
			{1, 0, 0}, // ($1.1)
			{1, 0, 3}, // -$1.1
			{1, 0, 4}, // $-1.1
			{1, 0, 2}, // $1.1-
			{0, 0, 0}, // (1.1$)
			{0, 0, 1}, // -1.1$
			{0, 0, 2}, // 1.1-$
			{0, 0, 2}, // 1.1$-
			{0, 1, 1}, // -1.1 $
			{1, 1, 3}, // -$ 1.1
			{0, 1, 2}, // 1.1 $-
			{1, 1, 2}, // $ 1.1-
			{1, 1, 4}, // $ -1.1
			{0, 1, 2}, // 1.1- $
			{1, 1, 0}, // ($ 1.1)
			{0, 1, 0}, // (1.1 $)
		};
		set_int(a->monetary.n_cs_precedes, tab[v][0]);
		set_int(a->monetary.n_sep_by_space, tab[v][1]);
		set_int(a->monetary.n_sign_posn, tab[v][2]);
		set_int(a->monetary.int_n_cs_precedes, tab[v][0]);
		set_int(a->monetary.int_n_sep_by_space, tab[v][1]);
		set_int(a->monetary.int_n_sign_posn, tab[v][2]);
	}
	if (I(u"iFirstDayOfWeek", v) && v >= 0 && v <= 6)
	{
		set_int(a->time.first_weekday, v + 1); // win32 0=Mon..6=Sun -> 1..7
	}
	if (I(u"iFirstWorkday", v) && v >= 0 && v <= 6)
	{
		set_int(a->time.first_workday, v + 1);
	}
	if (I(u"iPaperSize", v))
	{
		::std::int_least64_t hw{};
		switch (v)
		{
		case 1:
			hw = 279 * 1000 + 216;
			break; // Letter
		case 5:
			hw = 356 * 1000 + 216;
			break; // Legal
		case 8:
			hw = 420 * 1000 + 297;
			break; // A3
		case 9:
			hw = 297 * 1000 + 210;
			break; // A4
		default:
			break;
		}
		if (hw != 0)
		{
			set_int(a->paper.height, hw / 1000);
			set_int(a->paper.width, hw % 1000);
		}
	}
}

#endif

inline void resolve_locale_name(::fast_io::u8string_view name, ::fast_io::u8string &out,
								lcblob::locale_charset &out_cs,
								[[maybe_unused]] bool ignore_system_settings = false,
								bool *from_os_default = nullptr) FAST_IO_HERBCEPTIONS_THROWS
{
	if (from_os_default != nullptr)
	{
		*from_os_default = false;
	}
	::fast_io::u8string raw;
	if (name.is_empty())
	{
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
		// L10N then FAST_IO_L10N_LANG then LC_ALL env wins over the OS
		// default, same as posix; ignore_system_settings only gates
		// the registry fallback — the GetUserDefaultLocaleName API
		// is the standard OS lookup
		raw = env_u8(u"L10N", "L10N");
		if (raw.is_empty())
		{
			raw = env_u8(u"FAST_IO_L10N_LANG", "FAST_IO_L10N_LANG");
		}
		if (raw.is_empty())
		{
			raw = env_u8(u"LC_ALL", "LC_ALL");
		}
		if (raw.is_empty())
		{
			raw = default_locale_name_win32(ignore_system_settings);
			if (raw.is_empty())
			{
				raw.append(u8"C", 1);
			}
			// the name came from the OS itself — only this result is
			// the user locale the HKCU\International overrides belong to
			if (from_os_default != nullptr)
			{
				*from_os_default = true;
			}
		}
#else
		char const *env{};
		for (char const *var : {"L10N", "FAST_IO_L10N_LANG", "LC_ALL", "LANG"})
		{
			if (char const *v{lc_getenv(var)}; v != nullptr && *v != 0)
			{
				env = v;
				break;
			}
		}
		if (env == nullptr)
		{
			raw.append(u8"C", 1);
		}
		else
		{
			auto *e8{reinterpret_cast<char8_t const *>(env)};
			raw.append(e8, ::fast_io::cstr_len(env));
		}
#endif
	}
	else
	{
		raw.append(name.data(), name.size());
	}
	for (char8_t &ch : raw) // '-' -> '_' (BCP-47 vs posix spellings)
	{
		if (ch == u8'-')
		{
			ch = u8'_';
		}
	}
	::std::size_t n{raw.size()};
	::std::size_t mod{n}, dot{n};
	for (::std::size_t i{}; i < n; ++i)
	{
		char8_t ch{raw.data()[i]};
		if (ch == u8'@' && mod == n)
		{
			mod = i;
		}
		else if (ch == u8'.' && dot == n && mod == n)
		{
			dot = i;
		}
	}
	out_cs = lcblob::locale_charset::utf8; // no codeset -> UTF-8
	if (dot != n)
	{
		parse_codeset(::fast_io::u8string_view{raw.data() + dot + 1, mod - dot - 1}, out_cs);
	}
	::std::size_t const base_end{dot == n ? mod : dot};
	// C and POSIX are the same UTF-8 locale — canonical name POSIX.UTF-8
	if ((base_end == 1 && raw.data()[0] == u8'C') ||
		(base_end == 5 && raw.data()[0] == u8'P' && raw.data()[1] == u8'O' &&
		 raw.data()[2] == u8'S' && raw.data()[3] == u8'I' && raw.data()[4] == u8'X'))
	{
		out.append(u8"POSIX.UTF-8", 11);
		out_cs = lcblob::locale_charset::utf8;
	}
	else
	{
		auto const *csn{lcblob::locale_charset_name(out_cs)};
		out.append(raw.data(), base_end); // base name
		out.push_back(u8'.');
		out.append(csn, ::fast_io::cstr_len(csn)); // canonical codeset
	}
	out.append(raw.data() + mod, n - mod); // @modifier, if any
	check_name(::fast_io::u8string_view{out.data(), out.size()});
}

// the locale data directory: FAST_IO_L10N_PATH, or the build-time
// FAST_IO_I18N_LOCALE_DIR default. Nothing configured is an error —
// there is no system-wide default to silently guess.
inline ::fast_io::u8string_view locale_dir() FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
	// read once into a process-lifetime string; the view into it is
	// what callers take (env_u8 returns an owned u8string — the
	// static keeps it alive, no dangling view)
	static ::fast_io::u8string const env_dir{env_u8(u"FAST_IO_L10N_PATH",
													"FAST_IO_L10N_PATH")};
	if (!env_dir.is_empty())
	{
		return ::fast_io::u8string_view{env_dir.data(), env_dir.size()};
	}
#else
	if (char const *v{lc_getenv("FAST_IO_L10N_PATH")}; v != nullptr && *v != 0)
	{
		return ::fast_io::u8string_view{reinterpret_cast<char8_t const *>(v),
										::fast_io::cstr_len(v)};
	}
#endif
#ifdef FAST_IO_I18N_LOCALE_DIR
	return ::fast_io::u8string_view{u8"" FAST_IO_I18N_LOCALE_DIR};
#else
	throw_einval();
#endif
}

// the locale directory fd — opened lazily on the first miss and kept
// open for the process's lifetime; every locale file is opened relative
// to it (openat semantics), never by string concatenation
inline ::fast_io::dir_file *locale_dir_file{};

inline ::fast_io::l10n::lc_locale const *load_blob_impl(::fast_io::u8string_view lname,
														bool user)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// one file per locale+codeset — key = canonical file name; the
	// user-locale (""-resolved) copy gets a \x01 suffix so it can never
	// collide with the plain load of the same file
	// the file is lname.bin; the user-locale copy carries a \x01 cache
	// suffix so it never collides with a plain load of the same file
	::fast_io::u8string file{::fast_io::u8concat_fast_io(lname, u8".bin")};
	::fast_io::u8string path{file};
	if (user)
	{
		path.push_back(1);
	}
	::fast_io::u8string_view key{path.data(), path.size()};

	if (auto it{tls_map.find_key(key)}; it != tls_map.end())
	{
		return it->mapped();
	}

	::fast_io::l10n::lc_locale const *p;
	{
		cache_guard g{global_mtx};
		if (auto it{global_map.find_key(key)}; it != global_map.end())
		{
			p = it->mapped();
		}
		else
		{
			if (locale_dir_file == nullptr)
			{
				locale_dir_file = lc_new<::fast_io::dir_file>(
					::fast_io::mnp::os_c_str(locale_dir().data()));
			}
			// private copy-on-write pages — the default loader mode.
			// verify the complete image once, before it is published into
			// either cache; formatting then trusts every rva/scatter.
			::fast_io::native_file_loader loader{::fast_io::at(*locale_dir_file),
												 file};
			::fast_io::u8string_view bv{reinterpret_cast<char8_t const *>(loader.data()),
										loader.size()};
			auto const *loc{reinterpret_cast<lcblob::lc_locale const *>(bv.data())};
			if (!lcblob::lc_verify(loc, bv.size()))
			{
				throw_einval();
			}
			auto *entry{lc_new<locale_entry>(static_cast<::fast_io::native_file_loader &&>(loader))};
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
			if (user)
			{
				// private copy: every process pays its own bytes for the
				// user locale — the registry can differ per user and
				// shared mappings cannot be patched, so there is no
				// shared form of this
				auto const *fb{reinterpret_cast<unsigned char const *>(loc)};
				entry->img.resize(bv.size());
				::fast_io::details::my_memcpy(entry->img.data(), fb, bv.size());
				/* every appended override lands past the copy — reserve
				 * headroom NOW so the appends can never reallocate:
				 * patch_user_section holds basic_lc_all* and scatter
				 * refs INTO img — a move would dangle them mid-call and
				 * the rva writes would land in freed memory */
				entry->img.reserve(bv.size() + 16384);
				user_blob_builder b{entry->img};
				intl_key ik{};
				// patch each present section in its own charset. The
				// slot offsets and codeset are read before any append —
				// img can reallocate while strings are being added
				::std::uint_least32_t slot_off[4], codeset;
				{
					auto const *L{reinterpret_cast<lcblob::lc_locale const *>(
						entry->img.data())};
					slot_off[0] = lcblob::lc_u32(L->all.off);
					slot_off[1] = lcblob::lc_u32(L->u8all.off);
					slot_off[2] = lcblob::lc_u32(L->u16all.off);
					slot_off[3] = lcblob::lc_u32(L->u32all.off);
					codeset = lcblob::lc_u32(L->codeset);
				}
				auto const at{[&](::std::uint_least32_t off) noexcept {
					return reinterpret_cast<lcblob::basic_lc_all<char> *>(
						entry->img.data() + off);
				}};
				if (slot_off[0] != 0)
				{
					switch (static_cast<lcblob::locale_charset>(codeset))
					{
					case lcblob::locale_charset::gb18030:
						patch_user_section<user_enc::gb18030>(at(slot_off[0]), b, ik.hkey);
						break;
					case lcblob::locale_charset::utf_ebcdic:
						patch_user_section<user_enc::ebcdic>(at(slot_off[0]), b, ik.hkey);
						break;
					default:
						patch_user_section<user_enc::utf8>(at(slot_off[0]), b, ik.hkey);
						break;
					}
				}
				if (slot_off[1] != 0)
				{
					patch_user_section<user_enc::utf8>(at(slot_off[1]), b, ik.hkey);
				}
				if (slot_off[2] != 0)
				{
					patch_user_section<user_enc::utf16>(at(slot_off[2]), b, ik.hkey);
				}
				if (slot_off[3] != 0)
				{
					patch_user_section<user_enc::utf32>(at(slot_off[3]), b, ik.hkey);
				}
				// the appended overrides live past the file's own total —
				// the private image's boundary is the vector's size
				*reinterpret_cast<::std::uint_least32_t *>(
					entry->img.data() + offsetof(lcblob::lc_locale, total)) =
					lcblob::lc_u32(static_cast<::std::uint_least32_t>(entry->img.size()));
				p = reinterpret_cast<lcblob::lc_locale const *>(entry->img.data());
			}
			else
#endif
			{
				p = reinterpret_cast<lcblob::lc_locale const *>(entry->loader.data());
			}
			// re-verify the private image after the Windows registry
			// overrides were appended (rva fields and total changed).
			// normal loads were already verified above.
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
			if (user && !lcblob::lc_verify(p, entry->img.size()))
			{
				throw_einval();
			}
#endif
			global_map.insert_key(key, p);
		}
	}
	tls_map.insert_key(key, p);
	return p;
}

inline ::fast_io::l10n::lc_locale const *
load_l10n_impl(char8_t const *name, ::std::size_t name_len,
			   ::std::uint_least32_t cflags) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string_view name_sv;
	if (name_len != 0)
	{
		if (name == nullptr)
		{
			details::throw_einval();
		}
		name_sv = ::fast_io::u8string_view{name, name_len};
	}
	l10n_load_flags const flags{static_cast<l10n_load_flags>(cflags)};
	::fast_io::u8string lname;
	lcblob::locale_charset cs{};
	bool const ignore_system_settings{
		(flags & l10n_load_flags::ignore_system_settings) != l10n_load_flags::none};
	bool from_os_default{};
	details::resolve_locale_name(name_sv, lname, cs, ignore_system_settings,
								 __builtin_addressof(from_os_default));
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
	// registry user overrides only belong to the OS-default locale —
	// an env- or caller-named locale is an explicit choice and must
	// load its blob unpatched
	bool const user{from_os_default && !ignore_system_settings};
#else
	bool const user{false};
#endif
	return details::load_blob_impl(::fast_io::u8string_view{lname.data(), lname.size()},
								   user);
}

#if !defined(__HERBCEPTIONS__) && defined(FAST_IO_CPP_EXCEPTIONS)
#if defined(_WIN32) || defined(__CYGWIN__)
// small win32->errno map covering the file/registry failures the
// locale loader can raise; anything unmapped degrades to EIO
inline int win32_code_to_errno(::std::uint_least32_t code) noexcept
{
	switch (code)
	{
	case 2: // ERROR_FILE_NOT_FOUND
	case 3: // ERROR_PATH_NOT_FOUND
		return ENOENT;
	case 4: // ERROR_TOO_MANY_OPEN_FILES
		return EMFILE;
	case 5: // ERROR_ACCESS_DENIED
		return EACCES;
	case 6: // ERROR_INVALID_HANDLE
		return EBADF;
	case 8:  // ERROR_NOT_ENOUGH_MEMORY
	case 14: // ERROR_OUTOFMEMORY
		return ENOMEM;
	case 32: // ERROR_SHARING_VIOLATION
		return EBUSY;
	case 50: // ERROR_NOT_SUPPORTED
		return ENOTSUP;
	case 80:  // ERROR_FILE_EXISTS
	case 183: // ERROR_ALREADY_EXISTS
		return EEXIST;
	case 87:  // ERROR_INVALID_PARAMETER
	case 123: // ERROR_INVALID_NAME
		return EINVAL;
	case 112: // ERROR_DISK_FULL
		return ENOSPC;
	case 145: // ERROR_DIR_NOT_EMPTY
		return ENOTEMPTY;
	case 206: // ERROR_FILENAME_EXCED_RANGE
		return ENAMETOOLONG;
	default:
		return EIO;
	}
}
#endif

// errno projection of a thrown ::fast_io::error for the status ABI:
// the posix domain already carries errno; nt status codes ride the
// same win32 map through RtlNtStatusToDosError. no other domain is
// reachable from the locale loader
inline ::std::int_least32_t l10n_error_to_errno(::fast_io::error const &e) noexcept
{
#if defined(_WIN32) || defined(__CYGWIN__)
	if (e.domain == ::fast_io::herbceptions::details::nt_domain_value)
	{
		return win32_code_to_errno(::fast_io::win32::nt::rtl_nt_status_to_dos_error(
			static_cast<::std::uint_least32_t>(e.code)));
	}
	if (e.domain == ::fast_io::herbceptions::details::win32_domain_value)
	{
		return win32_code_to_errno(static_cast<::std::uint_least32_t>(e.code));
	}
#endif
	if (e.domain == ::fast_io::herbceptions::details::posix_domain_value)
	{
		return static_cast<::std::int_least32_t>(e.code);
	}
	// unreachable from the locale loader — do not pass a foreign
	// domain's code through as if it were errno
	return EIO;
}
#endif

} // namespace details

// errno-channel twin of the throws entry for consumers without a
// herbceptions channel — the same ABI boundary libwineunix uses:
// {status, ptr}, 0 on success, an errno/errc value on failure.
extern "C" FAST_IO_I18N_EXPORT ::fast_io::l10n::l10n_load_status
fast_io_l10n_load_with_status(char8_t const *name, ::std::size_t name_len,
							  ::std::uint_least32_t cflags) noexcept
{
#if defined(__HERBCEPTIONS__)
	try
	{
		return {0, details::load_l10n_impl(name, name_len, cflags)};
	}
	catch throws(::std::error e)
	{
		return {static_cast<::std::int_least32_t>(e.to_errc()), nullptr};
	}
#elif defined(FAST_IO_CPP_EXCEPTIONS)
	try
	{
		return {0, details::load_l10n_impl(name, name_len, cflags)};
	}
	catch (::fast_io::error const &e)
	{
		return {details::l10n_error_to_errno(e), nullptr};
	}
#else
	// no exception mechanism at all: the loader's own error paths
	// terminate inside the library — nothing to translate
	return {0, details::load_l10n_impl(name, name_len, cflags)};
#endif
}

#if defined(__HERBCEPTIONS__)
extern "C" FAST_IO_I18N_EXPORT ::fast_io::l10n::lc_locale const *
fast_io_l10n_load(char8_t const *name, ::std::size_t name_len,
				  ::std::uint_least32_t cflags) FAST_IO_HERBCEPTIONS_THROWS
{
	return details::load_l10n_impl(name, name_len, cflags);
}
#endif

} // namespace fast_io::l10n

#endif

#include <fast_io_dsal/impl/misc/pop_macros.h>
