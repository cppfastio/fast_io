#pragma once

/*
 * HLS playlists (RFC 8216, the ".m3u8" files an HLS server hands out).
 *
 * A playlist is either a MASTER playlist — variant streams plus their
 * alternative renditions — or a MEDIA playlist — an ordered list of media
 * segments. Both shapes are parsed into plain aggregates; every URI is kept
 * exactly as written in the file and the caller resolves it against the URL
 * the playlist was fetched from with resolve_uri(). Nothing here fetches,
 * caches or interprets media.
 *
 * Parsing is lenient about unknown tags and attributes — they are skipped, as
 * is an attribute-list entry without "=" — but strict about numbers: a
 * numeric field that is not a plain decimal value aborts the parse rather
 * than quietly reading as zero, because a playlist that cannot be trusted
 * about its own numbers is not one to schedule against. A missing optional
 * field stays distinguishable from a present one through the "has_" flags.
 *
 * Durations are microseconds rather than floating point. EXTINF in the wild
 * carries up to six decimal places (8.333333, 11.583333, 15.400000), so
 * microseconds represent every value a real playlist uses exactly and leave
 * stream scheduling without floating-point rounding error.
 *
 * The caller, not this parser, decides what a URI means: a media playlist's
 * segment URIs resolve against the playlist URL, a master playlist's variant
 * URIs against the master URL, and a playlist reached through redirects
 * against the FINAL url, not the requested one — resolving against the
 * requested URL is what makes segment requests 404 on the redirecting
 * IPTV/CDN endpoints these playlists usually come from.
 */

#include "fast_io_dsal/string.h"
#include "fast_io_dsal/string_view.h"
#include "fast_io_dsal/vector.h"

namespace fast_io::m3u8
{

using ::fast_io::u8string;
using ::fast_io::u8string_view;

/* segment.key_index / segment.map_index when no key or init section applies */
inline constexpr ::std::size_t no_index{static_cast<::std::size_t>(-1)};

enum class playlist_kind : ::std::uint_least8_t
{
	master,
	media
};

/* EXT-X-BYTERANGE:<length>[@<offset>] */
struct byte_range
{
	::std::uint_least64_t length{};
	::std::uint_least64_t offset{};
	bool has_offset{};
};

/* EXT-X-STREAM-INF RESOLUTION */
struct resolution
{
	::std::uint_least32_t width{};
	::std::uint_least32_t height{};
};

enum class encryption_method : ::std::uint_least8_t
{
	none,
	aes_128,
	sample_aes,
	other /* method_text carries what was actually written */
};

/* EXT-X-KEY — one entry per distinct key; segments index into the list */
struct key_info
{
	encryption_method method{encryption_method::none};
	u8string method_text{}; /* METHOD verbatim, for methods we do not model */
	u8string uri{};
	u8string iv{};         /* "0x…" verbatim, empty when absent */
	u8string key_format{}; /* empty means "identity" */
	u8string key_format_versions{};
};

/* EXT-X-MAP — the init section (fMP4 moov, or a TS header) for what follows */
struct map_info
{
	u8string uri{};
	byte_range range{};
	bool has_range{};
};

struct segment
{
	u8string uri{};
	::std::uint_least64_t duration_microseconds{}; /* EXTINF, seconds * 1e6 */
	u8string title{};                              /* EXTINF, after the comma */
	byte_range range{};                            /* EXT-X-BYTERANGE */
	bool has_range{};
	bool discontinuity{};         /* an EXT-X-DISCONTINUITY precedes this segment */
	bool gap{};                   /* EXT-X-GAP — a hole, decoders must not fetch it */
	u8string program_date_time{}; /* EXT-X-PROGRAM-DATE-TIME, verbatim */
	::std::size_t key_index{no_index};
	::std::size_t map_index{no_index};
};

/* EXT-X-MEDIA — an alternative rendition belonging to a variant's group */
struct rendition
{
	u8string type{}; /* AUDIO | VIDEO | SUBTITLES | CLOSED-CAPTIONS */
	u8string uri{};
	u8string group_id{};
	u8string language{};
	u8string assoc_language{};
	u8string name{};
	u8string channels{};
	u8string characteristics{};
	u8string instream_id{};
	bool is_default{};
	bool autoselect{};
	bool forced{};
};

/* EXT-X-STREAM-INF / EXT-X-I-FRAME-STREAM-INF */
struct variant
{
	u8string uri{};
	bool iframe{}; /* from EXT-X-I-FRAME-STREAM-INF (its URI is an attribute) */
	bool has_bandwidth{};
	::std::uint_least64_t bandwidth{};
	bool has_average_bandwidth{};
	::std::uint_least64_t average_bandwidth{};
	bool has_resolution{};
	::std::uint_least32_t width{};
	::std::uint_least32_t height{};
	bool has_frame_rate{};
	::std::uint_least64_t frame_rate_milli{}; /* FRAME-RATE * 1000 */
	u8string codecs{};
	u8string hdcp_level{};
	u8string audio{};
	u8string video{};
	u8string subtitles{};
	u8string closed_captions{};
	u8string name{};
	u8string stable_variant_id{};
};

struct playlist
{
	playlist_kind kind{playlist_kind::media};

	bool has_version{};
	::std::uint_least8_t version{};
	bool has_target_duration{};
	::std::uint_least32_t target_duration_seconds{};
	bool has_media_sequence{};
	::std::uint_least64_t media_sequence{};
	bool has_discontinuity_sequence{};
	::std::uint_least64_t discontinuity_sequence{};
	bool has_playlist_type{};
	u8string playlist_type{}; /* VOD | EVENT */
	bool has_endlist{};
	bool i_frames_only{};
	bool independent_segments{};
	bool has_start{};
	::std::int_least64_t start_offset_microseconds{};
	bool start_precise{};

	::fast_io::vector<variant> variants{};
	::fast_io::vector<rendition> renditions{};
	::fast_io::vector<key_info> keys{};
	::fast_io::vector<map_info> maps{};
	::fast_io::vector<segment> segments{};
};

namespace details
{

inline constexpr bool is_digit(char8_t c) noexcept
{
	return c >= u8'0' && c <= u8'9';
}

inline constexpr char8_t to_lower(char8_t c) noexcept
{
	return (c >= u8'A' && c <= u8'Z') ? static_cast<char8_t>(c - u8'A' + u8'a') : c;
}

/* ASCII case-insensitive compare — HLS writes tags and attributes upper
 * case, but playlists in the wild are not always that well behaved */
inline constexpr bool ieq(u8string_view a, u8string_view b) noexcept
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (::std::size_t i{}; i != a.size(); ++i)
	{
		if (to_lower(a[i]) != to_lower(b[i]))
		{
			return false;
		}
	}
	return true;
}

inline constexpr bool ieq_prefix(u8string_view a, u8string_view b) noexcept
{
	if (a.size() < b.size())
	{
		return false;
	}
	return ieq(u8string_view{a.data(), b.size()}, b);
}

inline constexpr u8string_view trim(u8string_view v) noexcept
{
	while (!v.is_empty() && (v.front() == u8' ' || v.front() == u8'\t'))
	{
		v.remove_prefix(1);
	}
	while (!v.is_empty() &&
		   (v.back() == u8' ' || v.back() == u8'\t' || v.back() == u8'\r'))
	{
		v.remove_suffix(1);
	}
	return v;
}

inline constexpr ::std::uint_least64_t pow10[19]{
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000,
	10000000000,
	100000000000,
	1000000000000,
	10000000000000,
	100000000000000,
	1000000000000000,
	10000000000000000,
	100000000000000000,
	1000000000000000000};

/* Every numeric field here goes through fast_io's decimal scanner, which
 * throws when it cannot read a value. The scanner stops at the first byte
 * that cannot continue the number, so a tag value — which must be nothing but
 * digits — additionally needs the scanner to have consumed all of v; a value
 * that is not entirely digits aborts the parse. Leading zeros are legal
 * (RFC 8216 decimal-integer is "a sequence of digits 0-9", and packagers do
 * zero-pad MEDIA-SEQUENCE), which is why the scan carries mnp::dec_get's
 * skipzero flag: the default integral scan rejects "007". */
inline constexpr ::std::uint_least64_t scan_uint(u8string_view v) throws
{
	::std::uint_least64_t out{};
	::fast_io::u8ibuffer_view ibv{v};
	::fast_io::scan(ibv, ::fast_io::mnp::dec_get<false, true>(out));
	if (ibv.curr_ptr != ibv.end_ptr)
	{
		::fast_io::herbceptions::throws_parse_errc(
			::fast_io::freestanding::parse_errc::invalid);
	}
	return out;
}

/* TIME-OFFSET is a signed decimal-floating-point (or signed decimal-integer)
 * number of seconds. The sign is taken off here because the digits below are
 * shared with seconds_to_microseconds, and the scanner's sign phase does not
 * accept a leading '+'. */
inline constexpr ::std::int_least64_t time_offset_microseconds(u8string_view v) throws;

/* decimal seconds -> microseconds, exact through six decimal places:
 * "11.583333" -> 11583333, "15" -> 15000000, "15.4" -> 15400000.
 * Digits past microsecond precision are dropped, not rounded. The integer
 * part must be all digits and, when a '.' is present, so must the fractional
 * part: a value the parser cannot read aborts the parse. */
inline constexpr ::std::uint_least64_t seconds_to_microseconds(u8string_view v) throws
{
	::fast_io::u8ibuffer_view ibv{v};
	::std::uint_least64_t whole{};
	::fast_io::scan(ibv, ::fast_io::mnp::dec_get<false, true>(whole));
	auto const *const last{ibv.end_ptr};
	auto const *it{ibv.curr_ptr};
	constexpr auto max{static_cast<::std::uint_least64_t>(-1)};
	if (whole > max / 1000000)
	{
		return max; /* an absurd duration; clamping beats wrapping */
	}
	auto micros{whole * 1000000};
	if (it == last)
	{
		return micros;
	}
	if (*it != u8'.')
	{
		::fast_io::herbceptions::throws_parse_errc(
			::fast_io::freestanding::parse_errc::invalid);
	}
	auto const *const fdigits{it + 1};
	::std::uint_least64_t fraction{};
	::fast_io::u8ibuffer_view fv{fdigits, last};
	::fast_io::scan(fv, ::fast_io::mnp::dec_get<false, true>(fraction));
	if (fv.curr_ptr != last)
	{
		::fast_io::herbceptions::throws_parse_errc(
			::fast_io::freestanding::parse_errc::invalid);
	}
	auto const digits{static_cast<::std::size_t>(last - fdigits)};
	if (digits > 18)
	{
		/* more fraction digits than a 64-bit integer holds */
		::fast_io::herbceptions::throws_parse_errc(
			::fast_io::freestanding::parse_errc::invalid);
	}
	if (digits <= 6)
	{
		micros += fraction * pow10[6 - digits];
	}
	else
	{
		micros += fraction / pow10[digits - 6];
	}
	return micros;
}

inline constexpr ::std::int_least64_t time_offset_microseconds(u8string_view v) throws
{
	bool negative{};
	if (!v.is_empty() && (v.front() == u8'-' || v.front() == u8'+'))
	{
		negative = v.front() == u8'-';
		v.remove_prefix(1);
	}
	auto const magnitude{seconds_to_microseconds(v)};
	return negative ? -static_cast<::std::int_least64_t>(magnitude)
					: static_cast<::std::int_least64_t>(magnitude);
}

/* FRAME-RATE keeps three decimals in the wild (29.970, 59.940) */
inline constexpr ::std::uint_least64_t seconds_to_milliseconds(u8string_view v) throws
{
	return seconds_to_microseconds(v) / 1000;
}

inline constexpr bool is_yes(u8string_view v) noexcept
{
	return ieq(v, u8string_view{u8"YES"});
}

/* split "TAG:ATTRS" — value is empty for a tag with no colon */
inline constexpr void split_tag(u8string_view line, u8string_view &tag,
								u8string_view &value) noexcept
{
	::std::size_t colon{line.size()};
	for (::std::size_t i{}; i != line.size(); ++i)
	{
		if (line[i] == u8':')
		{
			colon = i;
			break;
		}
	}
	tag = u8string_view{line.data(), colon};
	value = colon == line.size()
				? u8string_view{}
				: u8string_view{line.data() + colon + 1, line.size() - colon - 1};
}

/* "1280x720" -> 1280, 720; a value with no "x" or a side that is not a plain
 * decimal integer aborts the parse */
inline constexpr resolution parse_resolution(u8string_view v) throws
{
	::std::size_t x{v.size()};
	for (::std::size_t i{}; i != v.size(); ++i)
	{
		if (v[i] == u8'x' || v[i] == u8'X')
		{
			x = i;
			break;
		}
	}
	if (x == v.size())
	{
		::fast_io::herbceptions::throws_parse_errc(
			::fast_io::freestanding::parse_errc::invalid);
	}
	return {static_cast<::std::uint_least32_t>(scan_uint(trim(u8string_view{v.data(), x}))),
			static_cast<::std::uint_least32_t>(
				scan_uint(trim(u8string_view{v.data() + x + 1, v.size() - x - 1})))};
}

/* "<length>[@<offset>]" */
inline constexpr byte_range parse_byte_range(u8string_view v) throws
{
	byte_range r{};
	::std::size_t at{v.size()};
	for (::std::size_t i{}; i != v.size(); ++i)
	{
		if (v[i] == u8'@')
		{
			at = i;
			break;
		}
	}
	r.length = scan_uint(trim(u8string_view{v.data(), at}));
	if (at != v.size())
	{
		r.offset = scan_uint(trim(u8string_view{v.data() + at + 1, v.size() - at - 1}));
		r.has_offset = true;
	}
	return r;
}

inline constexpr bool has_scheme(u8string_view v) noexcept
{
	::std::size_t colon{v.size()};
	for (::std::size_t i{}; i != v.size(); ++i)
	{
		auto const c{v[i]};
		if (c == u8':')
		{
			colon = i;
			break;
		}
		if (c == u8'/' || c == u8'?' || c == u8'#')
		{
			return false;
		}
	}
	if (colon == 0 || colon == v.size() || v.size() < colon + 3)
	{
		return false;
	}
	if (!((v[0] >= u8'a' && v[0] <= u8'z') || (v[0] >= u8'A' && v[0] <= u8'Z')))
	{
		return false;
	}
	for (::std::size_t i{1}; i != colon; ++i)
	{
		auto const c{v[i]};
		if (!((c >= u8'a' && c <= u8'z') || (c >= u8'A' && c <= u8'Z') ||
			  is_digit(c) || c == u8'+' || c == u8'-' || c == u8'.'))
		{
			return false;
		}
	}
	return v[colon + 1] == u8'/' && v[colon + 2] == u8'/';
}

/* index just past "scheme://", 0 when the url carries no scheme */
inline constexpr ::std::size_t after_scheme(u8string_view v) noexcept
{
	if (!has_scheme(v))
	{
		return 0;
	}
	::std::size_t colon{};
	while (v[colon] != u8':')
	{
		++colon;
	}
	return colon + 3;
}

inline constexpr bool same_key(key_info const &a, key_info const &b) noexcept
{
	return a.method == b.method &&
		   ieq(u8string_view{::fast_io::freestanding::from_range, a.uri},
			   u8string_view{::fast_io::freestanding::from_range, b.uri}) &&
		   ieq(u8string_view{::fast_io::freestanding::from_range, a.iv},
			   u8string_view{::fast_io::freestanding::from_range, b.iv}) &&
		   ieq(u8string_view{::fast_io::freestanding::from_range, a.key_format},
			   u8string_view{::fast_io::freestanding::from_range, b.key_format}) &&
		   ieq(u8string_view{::fast_io::freestanding::from_range, a.key_format_versions},
			   u8string_view{::fast_io::freestanding::from_range, b.key_format_versions});
}

inline constexpr bool same_map(map_info const &a, map_info const &b) noexcept
{
	return ieq(u8string_view{::fast_io::freestanding::from_range, a.uri},
			   u8string_view{::fast_io::freestanding::from_range, b.uri}) &&
		   a.has_range == b.has_range && a.range.length == b.range.length &&
		   a.range.offset == b.range.offset &&
		   a.range.has_offset == b.range.has_offset;
}

/* one attribute of an attribute list, spaces trimmed, quoting removed.
 * Quoted values keep their commas — CODECS="avc1.4d401f,mp4a.40.2" is a
 * single attribute, not two. */
struct attribute_iterator
{
	u8string_view remaining{};
	u8string_view name{};
	u8string_view value{};
	bool quoted{};

	inline constexpr bool next() noexcept
	{
		for (;;)
		{
			while (!remaining.is_empty() &&
				   (remaining.front() == u8',' || remaining.front() == u8' ' ||
					remaining.front() == u8'\t'))
			{
				remaining.remove_prefix(1);
			}
			if (remaining.is_empty())
			{
				return false;
			}
			::std::size_t eq{};
			while (eq != remaining.size() && remaining[eq] != u8'=' &&
				   remaining[eq] != u8',')
			{
				++eq;
			}
			if (eq == remaining.size() || remaining[eq] == u8',')
			{
				/* an entry with no "=" — skip it */
				if (eq == remaining.size())
				{
					remaining = {};
					return false;
				}
				remaining =
					u8string_view{remaining.data() + eq + 1, remaining.size() - eq - 1};
				continue;
			}
			name = trim(u8string_view{remaining.data(), eq});
			auto rest{
				u8string_view{remaining.data() + eq + 1, remaining.size() - eq - 1}};
			quoted = !rest.is_empty() && rest.front() == u8'"';
			if (quoted)
			{
				rest.remove_prefix(1);
				::std::size_t close{rest.size()};
				for (::std::size_t i{}; i != rest.size(); ++i)
				{
					if (rest[i] == u8'"')
					{
						close = i;
						break;
					}
				}
				value = u8string_view{rest.data(), close};
				remaining = close == rest.size()
								? u8string_view{}
								: u8string_view{rest.data() + close + 1,
												rest.size() - close - 1};
			}
			else
			{
				::std::size_t comma{rest.size()};
				for (::std::size_t i{}; i != rest.size(); ++i)
				{
					if (rest[i] == u8',')
					{
						comma = i;
						break;
					}
				}
				value = trim(u8string_view{rest.data(), comma});
				remaining = comma == rest.size()
								? u8string_view{}
								: u8string_view{rest.data() + comma + 1,
												rest.size() - comma - 1};
			}
			return true;
		}
	}
};

inline ::std::size_t intern_key(::fast_io::vector<key_info> &keys,
								u8string_view attrs)
	throws
{
	key_info k{};
	{
		attribute_iterator it{attrs};
		while (it.next())
		{
			if (ieq(it.name, u8string_view{u8"METHOD"}))
			{
				k.method_text.assign(it.value);
				if (ieq(it.value, u8string_view{u8"NONE"}))
				{
					k.method = encryption_method::none;
				}
				else if (ieq(it.value, u8string_view{u8"AES-128"}))
				{
					k.method = encryption_method::aes_128;
				}
				else if (ieq(it.value, u8string_view{u8"SAMPLE-AES"}))
				{
					k.method = encryption_method::sample_aes;
				}
				else
				{
					k.method = encryption_method::other;
				}
			}
			else if (ieq(it.name, u8string_view{u8"URI"}))
			{
				k.uri.assign(it.value);
			}
			else if (ieq(it.name, u8string_view{u8"IV"}))
			{
				k.iv.assign(it.value);
			}
			else if (ieq(it.name, u8string_view{u8"KEYFORMAT"}))
			{
				k.key_format.assign(it.value);
			}
			else if (ieq(it.name, u8string_view{u8"KEYFORMATVERSIONS"}))
			{
				k.key_format_versions.assign(it.value);
			}
		}
	}
	if (k.method == encryption_method::none && k.method_text.is_empty())
	{
		/* a bare EXT-X-KEY: malformed, index nothing */
		return no_index;
	}
	if (ieq(u8string_view{::fast_io::freestanding::from_range, k.method_text},
			u8string_view{u8"NONE"}))
	{
		/* back to clear — nothing to index */
		return no_index;
	}
	for (::std::size_t i{}; i != keys.size(); ++i)
	{
		if (same_key(keys[i], k))
		{
			return i;
		}
	}
	keys.push_back(k);
	return keys.size() - 1;
}

inline ::std::size_t intern_map(::fast_io::vector<map_info> &maps,
								u8string_view attrs)
	throws
{
	map_info m{};
	{
		attribute_iterator it{attrs};
		while (it.next())
		{
			if (ieq(it.name, u8string_view{u8"URI"}))
			{
				m.uri.assign(it.value);
			}
			else if (ieq(it.name, u8string_view{u8"BYTERANGE"}))
			{
				m.range = parse_byte_range(it.value);
				m.has_range = true;
			}
		}
	}
	if (m.uri.is_empty())
	{
		return no_index;
	}
	for (::std::size_t i{}; i != maps.size(); ++i)
	{
		if (same_map(maps[i], m))
		{
			return i;
		}
	}
	maps.push_back(m);
	return maps.size() - 1;
}

inline ::fast_io::m3u8::variant parse_variant(u8string_view attrs, bool iframe)
	throws
{
	::fast_io::m3u8::variant v{};
	v.iframe = iframe;
	attribute_iterator it{attrs};
	while (it.next())
	{
		if (ieq(it.name, u8string_view{u8"BANDWIDTH"}))
		{
			v.bandwidth = scan_uint(it.value);
			v.has_bandwidth = true;
		}
		else if (ieq(it.name, u8string_view{u8"AVERAGE-BANDWIDTH"}))
		{
			v.average_bandwidth = scan_uint(it.value);
			v.has_average_bandwidth = true;
		}
		else if (ieq(it.name, u8string_view{u8"RESOLUTION"}))
		{
			auto const r{parse_resolution(it.value)};
			v.has_resolution = true;
			v.width = r.width;
			v.height = r.height;
		}
		else if (ieq(it.name, u8string_view{u8"FRAME-RATE"}))
		{
			v.frame_rate_milli = seconds_to_milliseconds(it.value);
			v.has_frame_rate = true;
		}
		else if (ieq(it.name, u8string_view{u8"CODECS"}))
		{
			v.codecs.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"HDCP-LEVEL"}))
		{
			v.hdcp_level.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"AUDIO"}))
		{
			v.audio.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"VIDEO"}))
		{
			v.video.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"SUBTITLES"}))
		{
			v.subtitles.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"CLOSED-CAPTIONS"}))
		{
			v.closed_captions.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"NAME"}))
		{
			v.name.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"STABLE-VARIANT-ID"}))
		{
			v.stable_variant_id.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"URI"}))
		{
			/* only EXT-X-I-FRAME-STREAM-INF carries its uri here */
			v.uri.assign(it.value);
		}
	}
	return v;
}

inline ::fast_io::m3u8::rendition parse_rendition(u8string_view attrs)
	throws
{
	::fast_io::m3u8::rendition r{};
	attribute_iterator it{attrs};
	while (it.next())
	{
		if (ieq(it.name, u8string_view{u8"TYPE"}))
		{
			r.type.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"URI"}))
		{
			r.uri.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"GROUP-ID"}))
		{
			r.group_id.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"LANGUAGE"}))
		{
			r.language.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"ASSOC-LANGUAGE"}))
		{
			r.assoc_language.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"NAME"}))
		{
			r.name.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"CHANNELS"}))
		{
			r.channels.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"CHARACTERISTICS"}))
		{
			r.characteristics.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"INSTREAM-ID"}))
		{
			r.instream_id.assign(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"DEFAULT"}))
		{
			r.is_default = is_yes(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"AUTOSELECT"}))
		{
			r.autoselect = is_yes(it.value);
		}
		else if (ieq(it.name, u8string_view{u8"FORCED"}))
		{
			r.forced = is_yes(it.value);
		}
	}
	return r;
}

/* an optional UTF-8 BOM may sit in front of the prologue */
inline constexpr u8string_view skip_bom(u8string_view text) noexcept
{
	if (text.size() >= 3 && static_cast<::std::uint_least8_t>(text[0]) == 0xEF &&
		static_cast<::std::uint_least8_t>(text[1]) == 0xBB &&
		static_cast<::std::uint_least8_t>(text[2]) == 0xBF)
	{
		text.remove_prefix(3);
	}
	return text;
}

/* the path part of a reference: everything before '?' or '#' */
inline constexpr u8string_view path_part(u8string_view reference) noexcept
{
	for (::std::size_t i{}; i != reference.size(); ++i)
	{
		if (reference[i] == u8'?' || reference[i] == u8'#')
		{
			return u8string_view{reference.data(), i};
		}
	}
	return reference;
}

/*
 * RFC 3986 §5.2.4 remove_dot_segments over an absolute path:
 * "/a/b/../c" -> "/a/c", "/a/b/./" -> "/a/b/", "/../x" -> "/x", "/a/b/.."
 * -> "/a/", "/" -> "/". Empty segments ("/a//b") collapse, which no playlist
 * in the wild relies on.
 */
inline u8string remove_dot_segments(u8string_view path)
	throws
{
	if (path.is_empty())
	{
		return u8string{};
	}
	::fast_io::vector<u8string_view> segments{};
	bool const trailing{path.back() == u8'/'};
	bool dot_tail{};
	::std::size_t i{};
	while (i != path.size())
	{
		while (i != path.size() && path[i] == u8'/')
		{
			++i;
		}
		::std::size_t end{i};
		while (end != path.size() && path[end] != u8'/')
		{
			++end;
		}
		if (end == i)
		{
			break;
		}
		auto const segment{u8string_view{path.data() + i, end - i}};
		bool const last{end == path.size()};
		if (segment == u8string_view{u8"."})
		{
			dot_tail = last;
		}
		else if (segment == u8string_view{u8".."})
		{
			if (!segments.is_empty())
			{
				segments.pop_back();
			}
			dot_tail = last;
		}
		else
		{
			segments.push_back(segment);
			dot_tail = false;
		}
		i = end;
	}
	u8string out{};
	for (::std::size_t s{}; s != segments.size(); ++s)
	{
		out.push_back(u8'/');
		out.append(segments[s]);
	}
	if (out.is_empty() || trailing || dot_tail)
	{
		out.push_back(u8'/');
	}
	return out;
}

} // namespace details

/* true when the text opens with the "#EXTM3U" prologue (an optional UTF-8
 * BOM and leading whitespace are tolerated). A body that fails this is not
 * a playlist — typically an HTML error page from an upstream that answered
 * 200 with the wrong thing. */
inline bool is_playlist(u8string_view text) noexcept
{
	text = details::skip_bom(text);
	while (!text.is_empty() &&
		   (text.front() == u8' ' || text.front() == u8'\t' ||
			text.front() == u8'\r' || text.front() == u8'\n'))
	{
		text.remove_prefix(1);
	}
	return details::ieq_prefix(text, u8string_view{u8"#EXTM3U"});
}

/*
 * RFC 3986 §5 reference resolution, the subset a playlist needs: absolute
 * urls ("http(s)://…", or any other scheme), network-path ("//host/…"),
 * absolute-path ("/…"), query-only ("?…") and relative-path references.
 *
 * The base's own query and fragment are dropped before matching, because a
 * relative reference never inherits them: resolving "byt/0088.ts" against
 * "http://cdn:88/applive/byt.m3u8?u=…&t=…&k=…" must answer
 * "http://cdn:88/applive/byt/0088.ts" — neither the signature nor any '/'
 * inside a query parameter may reach the result.
 *
 * The paths this splices together come out with their dot segments removed,
 * so "../../audio/en.m3u8" from "/v/720/index.m3u8" answers
 * "/audio/en.m3u8". A reference that already carries a scheme is handed back
 * untouched: it is a complete url, and reprocessing it buys nothing.
 */
inline u8string resolve_uri(u8string_view base, u8string_view reference)
	throws
{
	if (details::has_scheme(reference))
	{
		return u8string{reference};
	}
	auto const scheme_end{details::after_scheme(base)};
	if (scheme_end == 0)
	{
		/* base is not absolute either — nothing sensible to splice */
		u8string out{base};
		out.append(reference);
		return out;
	}
	::std::size_t authority_end{base.size()};
	for (::std::size_t i{scheme_end}; i != base.size(); ++i)
	{
		if (base[i] == u8'/')
		{
			authority_end = i;
			break;
		}
	}
	::std::size_t base_end{base.size()};
	for (::std::size_t i{scheme_end}; i != base.size(); ++i)
	{
		if (base[i] == u8'?' || base[i] == u8'#')
		{
			base_end = i;
			break;
		}
	}
	/* past host_end comes the base's path. A query that starts right after
	 * the authority ("http://h?a=b") makes authority_end overshoot base_end,
	 * hence the clamp. */
	auto const host_end{authority_end < base_end ? authority_end : base_end};
	auto const path{details::path_part(reference)};
	auto const tail{u8string_view{reference.data() + path.size(), reference.size() - path.size()}};
	if (reference.size() > 1 && reference.front() == u8'/' && reference[1] == u8'/')
	{
		/* "//host/path" — inherit the base's scheme */
		::std::size_t host_of_ref{path.size()};
		for (::std::size_t i{2}; i != path.size(); ++i)
		{
			if (path[i] == u8'/')
			{
				host_of_ref = i;
				break;
			}
		}
		u8string out{u8string_view{base.data(), scheme_end - 2}}; /* "scheme:" */
		out.append(u8string_view{path.data(), host_of_ref});
		auto const normalised{details::remove_dot_segments(
			u8string_view{path.data() + host_of_ref, path.size() - host_of_ref})};
		out.append(u8string_view{::fast_io::freestanding::from_range, normalised});
		out.append(tail);
		return out;
	}
	if (!reference.is_empty() && reference.front() == u8'/')
	{
		/* absolute path — the base contributes scheme and authority */
		u8string out{u8string_view{base.data(), host_end}};
		auto const normalised{details::remove_dot_segments(path)};
		out.append(u8string_view{::fast_io::freestanding::from_range, normalised});
		out.append(tail);
		return out;
	}
	if (!reference.is_empty() && reference.front() == u8'?')
	{
		/* query-only reference: same path, new query */
		u8string out{u8string_view{base.data(), base_end}};
		out.append(reference);
		return out;
	}
	if (reference.is_empty())
	{
		return u8string{u8string_view{base.data(), base_end}};
	}
	/* relative path — replace everything after the base's last '/' */
	::std::size_t slash{base_end};
	for (::std::size_t i{base_end}; i != scheme_end;)
	{
		--i;
		if (base[i] == u8'/')
		{
			slash = i;
			break;
		}
	}
	u8string merged{};
	if (slash == base_end)
	{
		/* no path at all in the base — the reference hangs off "/" */
		merged.push_back(u8'/');
	}
	else
	{
		merged.assign(u8string_view{base.data() + host_end, slash + 1 - host_end});
	}
	merged.append(path);
	u8string out{u8string_view{base.data(), host_end}};
	auto const normalised{
		details::remove_dot_segments(u8string_view{::fast_io::freestanding::from_range, merged})};
	out.append(u8string_view{::fast_io::freestanding::from_range, normalised});
	out.append(tail);
	return out;
}

/*
 * Parse a playlist body. Throws parse_errc::invalid when the "#EXTM3U"
 * prologue is missing — that is the only thing worth failing on, since an
 * upstream that answers an m3u8 request with an HTML error page must not be
 * mistaken for an empty playlist.
 */
[[nodiscard]] inline playlist parse(u8string_view text)
	throws
{
	if (!is_playlist(text))
	{
		::fast_io::herbceptions::throws_parse_errc(
			::fast_io::freestanding::parse_errc::invalid);
	}
	/* the prologue check tolerates a BOM; the line loop must not see it */
	text = details::skip_bom(text);
	playlist pl{};
	bool master_tags_seen{};
	bool pending_variant{};
	variant pending_var{};
	segment cur{};
	::std::size_t cur_key{no_index};
	::std::size_t cur_map{no_index};

	for (::std::size_t off{}; off <= text.size();)
	{
		::std::size_t eol{text.size()};
		for (::std::size_t i{off}; i != text.size(); ++i)
		{
			if (text[i] == u8'\n')
			{
				eol = i;
				break;
			}
		}
		if (eol == text.size() && off == text.size())
		{
			break;
		}
		auto line{u8string_view{text.data() + off, eol - off}};
		off = eol + 1;
		line = details::trim(line);
		if (line.is_empty())
		{
			continue;
		}

		if (line.front() != u8'#')
		{
			/* a URI line resolves either a pending variant or a segment */
			if (pending_variant)
			{
				pending_var.uri.assign(line);
				pl.variants.push_back(pending_var);
				pending_var = variant{};
				pending_variant = false;
			}
			else
			{
				cur.uri.assign(line);
				cur.key_index = cur_key;
				cur.map_index = cur_map;
				pl.segments.push_back(cur);
				cur = segment{};
			}
			continue;
		}

		u8string_view tag{};
		u8string_view value{};
		details::split_tag(line, tag, value);

		if (details::ieq(tag, u8string_view{u8"#EXTM3U"}))
		{
			/* prologue, already checked */
		}
		else if (details::ieq(tag, u8string_view{u8"#EXTINF"}))
		{
			::std::size_t comma{value.size()};
			for (::std::size_t i{}; i != value.size(); ++i)
			{
				if (value[i] == u8',')
				{
					comma = i;
					break;
				}
			}
			cur.duration_microseconds = details::seconds_to_microseconds(
				details::trim(u8string_view{value.data(), comma}));
			if (comma != value.size())
			{
				cur.title.assign(
					u8string_view{value.data() + comma + 1, value.size() - comma - 1});
			}
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-BYTERANGE"}))
		{
			cur.range = details::parse_byte_range(value);
			cur.has_range = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-DISCONTINUITY"}))
		{
			cur.discontinuity = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-GAP"}))
		{
			cur.gap = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-PROGRAM-DATE-TIME"}))
		{
			cur.program_date_time.assign(value);
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-KEY"}))
		{
			cur_key = details::intern_key(pl.keys, value);
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-MAP"}))
		{
			cur_map = details::intern_map(pl.maps, value);
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-STREAM-INF"}))
		{
			master_tags_seen = true;
			pending_var = details::parse_variant(value, false);
			pending_variant = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-I-FRAME-STREAM-INF"}))
		{
			master_tags_seen = true;
			pl.variants.push_back(details::parse_variant(value, true));
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-MEDIA"}))
		{
			master_tags_seen = true;
			pl.renditions.push_back(details::parse_rendition(value));
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-TARGETDURATION"}))
		{
			pl.target_duration_seconds =
				static_cast<::std::uint_least32_t>(details::scan_uint(details::trim(value)));
			pl.has_target_duration = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-MEDIA-SEQUENCE"}))
		{
			pl.media_sequence = details::scan_uint(details::trim(value));
			pl.has_media_sequence = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-DISCONTINUITY-SEQUENCE"}))
		{
			pl.discontinuity_sequence = details::scan_uint(details::trim(value));
			pl.has_discontinuity_sequence = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-VERSION"}))
		{
			pl.version =
				static_cast<::std::uint_least8_t>(details::scan_uint(details::trim(value)));
			pl.has_version = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-PLAYLIST-TYPE"}))
		{
			pl.has_playlist_type = true;
			pl.playlist_type.assign(details::trim(value));
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-ENDLIST"}))
		{
			pl.has_endlist = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-I-FRAMES-ONLY"}))
		{
			pl.i_frames_only = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-INDEPENDENT-SEGMENTS"}))
		{
			pl.independent_segments = true;
		}
		else if (details::ieq(tag, u8string_view{u8"#EXT-X-START"}))
		{
			details::attribute_iterator it{value};
			while (it.next())
			{
				if (details::ieq(it.name, u8string_view{u8"TIME-OFFSET"}))
				{
					pl.has_start = true;
					pl.start_offset_microseconds =
						details::time_offset_microseconds(it.value);
				}
				else if (details::ieq(it.name, u8string_view{u8"PRECISE"}))
				{
					pl.start_precise = details::is_yes(it.value);
				}
			}
		}
		/* EXT-X-DATERANGE, EXT-X-SERVER-CONTROL, EXT-X-PART, the LL-HLS set,
		 * EXT-X-ALLOW-CACHE and every vendor tag fall through here */
	}

	pl.kind = master_tags_seen ? playlist_kind::master : playlist_kind::media;
	return pl;
}

} // namespace fast_io::m3u8
