#pragma once

namespace fast_io
{

template <::std::integral fromchtype, ::std::integral tochtype>
struct transcode_result
{
	using from_char_type = fromchtype;
	using to_char_type = tochtype;
	from_char_type const *from_ptr;
	to_char_type *to_ptr;
};

template <typename T, typename fromchtype, typename tochtype>
concept transcoder = ::std::integral<fromchtype> && ::std::integral<tochtype> &&
					 requires(T t, fromchtype const *fromfirst, fromchtype const *fromlast,
							  tochtype *tofirst, tochtype *tolast) {
						 { t.transcode(fromfirst, fromlast, tofirst, tolast) } -> ::std::same_as<transcode_result<fromchtype, tochtype>>;
					 };

template <typename T, typename fromchtype, typename tochtype>
concept eof_transcoder = transcoder<T, fromchtype, tochtype> &&
						 requires(T t, fromchtype const *fromfirst, fromchtype const *fromlast,
								  tochtype *tofirst, tochtype *tolast) {
							 { t.transcode_eof(fromfirst, fromlast, tofirst, tolast) } -> ::std::same_as<transcode_result<fromchtype, tochtype>>;
						 };

struct transcode_bytes_result
{
	::std::byte const *from_ptr;
	::std::byte *to_ptr;
};

template <typename T>
concept byte_transcoder = requires(T t, ::std::byte const *fromfirst, ::std::byte const *fromlast,
								   ::std::byte *tofirst, ::std::byte *tolast) {
	{ t.transcode_bytes(fromfirst, fromlast, tofirst, tolast) } -> ::std::same_as<transcode_bytes_result>;
};

template <typename T>
concept byte_eof_transcoder = byte_transcoder<T> &&
							  requires(T t, ::std::byte const *fromfirst, ::std::byte const *fromlast,
									   ::std::byte *tofirst, ::std::byte *tolast) {
								  { t.transcode_bytes_eof(fromfirst, fromlast, tofirst, tolast) } -> ::std::same_as<transcode_bytes_result>;
							  };

} // namespace fast_io
