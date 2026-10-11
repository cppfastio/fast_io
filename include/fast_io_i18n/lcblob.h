#pragma once


// lcblob — the binary locale container (spec: fast_io_tools/binfmt/spec.md).
//
// The file IS the locale data: a flat, position-independent image of the
// same C-struct layout the runtime used to hold as pointers — every
// pointer is now an lc_rva<T> (a u32 file offset) and every
// basic_io_scatter_t is an lc_scatter<T> (lc_rva + u32 unit count).
// All fields are little-endian u32/s32, so the image is architecture
// neutral; on little-endian hosts it is read in place with zero parsing.
//
// Loading a locale is: mmap (private copy-on-write) + check magic and
// version + return a pointer to the lc_locale in the file. That is all —
// there is no decoding step. COW pages also mean a user may patch rva
// fields in their own mapping without touching the shared file image.
//
// File layout:
//   [lc_locale root]                    (40 bytes, offsets are constants)
//   [outer pool: name bytes, utf8]
//   [basic_lc_all<char>     charset ]   all    — the locale's declared
//   [basic_lc_all<char8_t>  utf8    ]   u8all    codeset for char-typed
//   [basic_lc_all<char16_t> utf16   ]   u16all   text (UTF-8, GB18030 or
//   [basic_lc_all<char32_t> utf32   ]   u32all   UTF-EBCDIC); all aliases
//                                                 u8all when it is UTF-8
// each followed by its own payload pool. Every rva is relative to the
// file base — resolve with lc_get_rva(base, field.ref) / read scalars
// with lc_u32(field).
//
// LEB128 is used ONLY inside compiled format programs (d_t_fmt & co) —
// never in the container itself.

namespace fast_io::l10n
{

inline constexpr ::std::uint_least32_t magic{0x314C4346}; // 'FCL1' LE
inline constexpr ::std::uint_least32_t blob_version{1};

// little-endian file scalars -> host
inline constexpr ::std::uint_least32_t lc_u32(::std::uint_least32_t v) noexcept
{
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		v = ::std::byteswap(v);
	}
	return v;
}

inline constexpr ::std::int_least32_t lc_s32(::std::uint_least32_t v) noexcept
{
	return static_cast<::std::int_least32_t>(lc_u32(v));
}

// a pointer in the file: u32 offset from the file base. T carries the
// pointee semantics — lc_rva<char16_t> resolves to char16_t const*.
template <typename T>
struct lc_rva
{
	::std::uint_least32_t off;
};

// a basic_io_scatter_t<T> in the file: rva + element count in T units
template <typename T>
struct lc_scatter
{
	lc_rva<T> ref;
	::std::uint_least32_t len;
};


// ---------------------------------------------------------------------------
// the file structs — basic_lc_* of the old runtime with pointers as rvas.
// Text fields are lc_scatter<char_type>; compiled programs and raw byte
// lists (grouping) are lc_scatter<char8_t> byte streams. Integer fields
// are s32. Layout is all-u32: deterministic on every ABI.
// ---------------------------------------------------------------------------

template <typename char_type>
struct basic_lc_identification
{
	lc_scatter<char_type> name, encoding, title, source, address, contact,
		email, tel, fax, language, territory, audience, application,
		abbreviation, revision, date;
};

template <typename char_type>
struct basic_lc_monetary
{
	lc_scatter<char_type> int_curr_symbol, currency_symbol, mon_decimal_point,
		mon_thousands_sep;
	lc_scatter<char8_t> mon_grouping; // byte list, charset-neutral
	lc_scatter<char_type> positive_sign, negative_sign;
	::std::int_least32_t int_frac_digits, frac_digits, p_cs_precedes,
		p_sep_by_space, n_cs_precedes, n_sep_by_space, int_p_cs_precedes,
		int_p_sep_by_space, int_n_cs_precedes, int_n_sep_by_space,
		p_sign_posn, n_sign_posn, int_p_sign_posn, int_n_sign_posn;
};

template <typename char_type>
struct basic_lc_numeric
{
	lc_scatter<char_type> decimal_point, thousands_sep;
	lc_scatter<char8_t> grouping; // byte list, charset-neutral
};

template <typename char_type>
struct basic_lc_time_era
{
	::std::int_least32_t direction; // +1 / -1
	::std::int_least32_t offset;
	::std::int_least32_t start_year;
	::std::uint_least32_t start_month;
	::std::uint_least32_t start_day;
	::std::int_least32_t end_year; // INT32_MIN/MAX = -*/+*
	::std::uint_least32_t end_month;
	::std::uint_least32_t end_day;
	lc_scatter<char_type> name;
	lc_scatter<char8_t> era_format; // compiled program
};

template <typename char_type>
struct basic_lc_time
{
	lc_scatter<char_type> abday[7];
	lc_scatter<char_type> day[7];
	lc_scatter<char_type> abmon[12];
	lc_scatter<char_type> ab_alt_mon[12];
	lc_scatter<char_type> mon[12];
	lc_scatter<char_type> alt_mon[12];
	lc_scatter<char8_t> d_t_fmt, d_fmt, t_fmt, t_fmt_ampm, date_fmt;
	lc_scatter<char_type> am_pm[2];
	lc_scatter<basic_lc_time_era<char_type>> era;
	lc_scatter<char8_t> era_d_fmt, era_d_t_fmt, era_t_fmt;
	lc_scatter<lc_scatter<char_type>> alt_digits;
	struct
	{
		::std::int_least32_t ndays;
		::std::int_least32_t first_day;
		::std::int_least32_t first_week;
	} week;
	::std::int_least32_t first_weekday, first_workday, cal_direction;
	lc_scatter<lc_scatter<char_type>> timezone;
};

template <typename char_type>
struct basic_lc_messages
{
	lc_scatter<char_type> yesexpr, noexpr, yesstr, nostr;
};

struct basic_lc_paper
{
	::std::int_least32_t width, height; // mm
};

template <typename char_type>
struct basic_lc_telephone
{
	lc_scatter<char8_t> tel_int_fmt, tel_dom_fmt; // programs
	lc_scatter<char_type> int_select, int_prefix;
};

template <typename char_type>
struct basic_lc_name
{
	lc_scatter<char8_t> name_fmt; // program
	lc_scatter<char_type> name_gen, name_miss, name_mr, name_mrs, name_ms;
};

template <typename char_type>
struct basic_lc_address
{
	lc_scatter<char8_t> postal_fmt; // program
	lc_scatter<char_type> country_name, country_post, country_ab2, country_ab3;
	::std::int_least32_t country_num;
	lc_scatter<char_type> country_car, country_isbn, lang_name, lang_ab,
		lang_term, lang_lib;
};

struct basic_lc_measurement
{
	::std::int_least32_t measurement; // 1 = metric, 2 = US
};

template <typename char_type>
struct basic_lc_keyboard
{
	lc_scatter<lc_scatter<char_type>> keyboards;
};

template <typename char_type>
struct basic_lc_all
{
	basic_lc_identification<char_type> identification;
	basic_lc_monetary<char_type> monetary;
	basic_lc_numeric<char_type> numeric;
	basic_lc_time<char_type> time;
	basic_lc_messages<char_type> messages;
	basic_lc_paper paper;
	basic_lc_telephone<char_type> telephone;
	basic_lc_name<char_type> name;
	basic_lc_address<char_type> address;
	basic_lc_measurement measurement;
	basic_lc_keyboard<char_type> keyboard;
};

struct lc_locale
{
	::std::uint_least32_t magic;
	::std::uint_least32_t version;
	::std::uint_least32_t total; // file size
	::std::uint_least32_t flags;
	::std::uint_least32_t codeset; // == locale_charset, the char view
	lc_scatter<char8_t> name;      // canonical locale name, utf8
	lc_rva<basic_lc_all<char>> all;
	lc_rva<basic_lc_all<char8_t>> u8all;
	lc_rva<basic_lc_all<char16_t>> u16all;
	lc_rva<basic_lc_all<char32_t>> u32all;
};

// resolve an rva against the file base — the lc_locale pointer itself.
// off 0 = absent -> nullptr; anything outside the image is a malformed
// file, so it throws rather than resolving a wild pointer
template <typename T>
inline constexpr T const *lc_get_rva(void const *base, lc_rva<T> r)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least32_t const off{lc_u32(r.off)};
	if (off == 0)
	{
		return nullptr;
	}
	if (off >= lc_u32(static_cast<lc_locale const *>(base)->total))
	{
		::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
	}
	return reinterpret_cast<T const *>(static_cast<char8_t const *>(base) + off);
}

// resolve a scatter to a host {base,len} pair — T units. {nullptr,0}
// when absent; the whole range must fit inside the image
template <typename T>
inline constexpr ::fast_io::basic_io_scatter_t<T>
lc_get_scatter(void const *base, lc_scatter<T> s)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least32_t const off{lc_u32(s.ref.off)};
	if (off == 0)
	{
		return {};
	}
	auto const total{lc_u32(static_cast<lc_locale const *>(base)->total)};
	auto const len{static_cast<::std::size_t>(lc_u32(s.len))};
	if (off >= total || len > (total - off) / sizeof(T))
	{
		::fast_io::herbceptions::throws_errc(::std::errc::invalid_argument);
	}
	return {reinterpret_cast<T const *>(static_cast<char8_t const *>(base) + off),
			len};
}


// the codesets a locale file can declare for its char view — the only
// three supported
enum class locale_charset : ::std::uint_least8_t
{
	utf8 = 0,
	gb18030 = 1,
	utf_ebcdic = 2,
};

inline constexpr char8_t const *locale_charset_name(locale_charset cs) noexcept
{
	switch (cs)
	{
	case locale_charset::gb18030:
		return u8"GB18030";
	case locale_charset::utf_ebcdic:
		return u8"UTF-EBCDIC";
	default:
		return u8"UTF-8";
	}
}

// the section struct for a stream char_type: char reads the charset
// slot (the locale's declared codeset), char8_t/16/32 their own UTF
// sections, wchar_t follows its size. nullptr when the slot is absent.
template <::std::integral char_type>
inline constexpr basic_lc_all<char_type> const *lc_get_all(lc_locale const *l)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (::std::same_as<char_type, char>)
	{
		return lc_get_rva(l, l->all);
	}
	else if constexpr (::std::same_as<char_type, char8_t>)
	{
		return lc_get_rva(l, l->u8all);
	}
	else if constexpr (::std::same_as<char_type, char16_t>)
	{
		return lc_get_rva(l, l->u16all);
	}
	else if constexpr (::std::same_as<char_type, char32_t>)
	{
		return lc_get_rva(l, l->u32all);
	}
	else if constexpr (::std::same_as<char_type, wchar_t>)
	{
		if constexpr (sizeof(wchar_t) == 1)
		{
			return reinterpret_cast<basic_lc_all<wchar_t> const *>(
				lc_get_rva(l, l->u8all));
		}
		else if constexpr (sizeof(wchar_t) == 2)
		{
			return reinterpret_cast<basic_lc_all<wchar_t> const *>(
				lc_get_rva(l, l->u16all));
		}
		else
		{
			return reinterpret_cast<basic_lc_all<wchar_t> const *>(
				lc_get_rva(l, l->u32all));
		}
	}
	else
	{
		return lc_get_rva(l, l->u32all);
	}
}


// load flags — bits controlling how much of the OS's own locale
// settings the load consults
enum class l10n_load_flags : ::std::uint_least32_t
{
	none = 0,
	// ignore the system's own user-locale data when resolving "" —
	// on Windows the HKCU\Control Panel\International registry value
	// is never read; GetUserDefaultLocaleName still resolves "" as
	// the normal OS lookup. POSIX env (FAST_IO_L10N_LANG/LC_ALL/LANG)
	// and FAST_IO_L10N_PATH are the normal mechanism, not overrides,
	// and stay active
	ignore_system_settings = static_cast<::std::uint_least32_t>(1) << 0,
};

inline constexpr l10n_load_flags operator&(l10n_load_flags x, l10n_load_flags y) noexcept
{
	using utype = typename ::std::underlying_type<l10n_load_flags>::type;
	return static_cast<l10n_load_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr l10n_load_flags operator|(l10n_load_flags x, l10n_load_flags y) noexcept
{
	using utype = typename ::std::underlying_type<l10n_load_flags>::type;
	return static_cast<l10n_load_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr l10n_load_flags &operator|=(l10n_load_flags &x, l10n_load_flags y) noexcept
{
	return x = x | y;
}


#if !defined(FAST_IO_FREESTANDING)

// ---------------------------------------------------------------------------
// shared-library loading API — implemented in src/locale/lcblob.cc.
// The compiled library owns the process-wide cache: thread-local map first,
// then the global map under a mutex; entries are leaked and never unloaded
// so reloads are pointer lookups, never remaps.
// ---------------------------------------------------------------------------

#if defined(FAST_IO_I18N_SHARED)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(FAST_IO_I18N_BUILDING)
#define FAST_IO_I18N_EXPORT __declspec(dllexport)
#else
#define FAST_IO_I18N_EXPORT __declspec(dllimport)
#endif
#else
#define FAST_IO_I18N_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define FAST_IO_I18N_EXPORT
#endif

// Returns a pointer to the lc_locale inside the mapped file — image data
// is never unloaded, so the pointer (and every pointer lc_get_rva hands
// out) stays valid for the process lifetime. native_file_loader is an
// implementation detail of the library and is not exposed.
//
//   name is a standard locale name: lang[_TERRITORY][.codeset][@modifier]
//     "de_DE.UTF-8" / "de_DE.gb18030" / "de_DE.UTF-EBCDIC" ...
//     the codeset names the char view only — the utf8/16/32 sections are
//     always in the same file. Supported codesets: UTF-8, GB18030,
//     UTF-EBCDIC; no codeset defaults to UTF-8.
//   name ""      -> system default locale (FAST_IO_L10N_LANG/LC_ALL/LANG on POSIX;
//                   GetUserDefaultLocaleName then the registry on Windows).
//   name "C"/"POSIX" resolve to the canonical POSIX.UTF-8 locale.
//   dir          from FAST_IO_L10N_PATH env, else the compile-time
//                FAST_IO_I18N_LOCALE_DIR macro — unset throws.
//   flags        ignore_system_settings skips the Windows registry fallback
//
// ---------------------------------------------------------------------------
// C ABI — the stable boundary for FFI and non-C++ consumers. Plain
// pointer + length + flag bits; no C++ types cross it.
//   name/name_len : locale-name bytes; name nullptr or name_len 0 -> ""
//                   (system default). nullptr with nonzero length is
//                   an error.
//   flags         : bitmask of l10n_load_flags as uint32
//
// Two forms ship, same convention as libwineunix:
//   fast_io_l10n_load_with_status : always exported, noexcept. The
//     failure rides the status field (0 = success, otherwise an
//     errno/errc value; locale is nullptr on failure). This is the
//     fallback path for consumers whose compiler has no herbceptions
//     error channel — a throws function cannot be called without one.
//   fast_io_l10n_load             : herbceptions builds only. The
//     failure rides the error channel as ::std::error with its full
//     domain — no errno round-trip.
// ---------------------------------------------------------------------------
struct l10n_load_status
{
	::std::int_least32_t status; // 0 = success, otherwise an errno value
	::fast_io::l10n::lc_locale const *locale;
};

extern "C" FAST_IO_I18N_EXPORT ::fast_io::l10n::l10n_load_status
fast_io_l10n_load_with_status(char8_t const *name, ::std::size_t name_len,
							  ::std::uint_least32_t flags) noexcept;

#if defined(__HERBCEPTIONS__)
extern "C" FAST_IO_I18N_EXPORT ::fast_io::l10n::lc_locale const *
fast_io_l10n_load(char8_t const *name, ::std::size_t name_len,
				  ::std::uint_least32_t flags) FAST_IO_HERBCEPTIONS_THROWS;
#endif

// C++ wrapper — typed view + flag enum. inline over the C ABI so the
// DLL never exports an Itanium-mangled symbol (breaks MSVC consumers).
// Without __HERBCEPTIONS__ the throws entry is not callable, so this
// goes through the status form and reports the errno value the same
// way every other non-herbceptions failure in the library does.
inline ::fast_io::l10n::lc_locale const *
load_l10n(::fast_io::u8string_view name,
		  l10n_load_flags flags = l10n_load_flags::none) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	return fast_io_l10n_load(name.data(), name.size(),
							 static_cast<::std::uint_least32_t>(flags));
#else
	auto const r{fast_io_l10n_load_with_status(name.data(), name.size(),
											   static_cast<::std::uint_least32_t>(flags))};
	if (r.status != 0)
	{
		::fast_io::throw_posix_error(static_cast<int>(r.status));
	}
	return r.locale;
#endif
}

#endif

} // namespace fast_io::l10n
