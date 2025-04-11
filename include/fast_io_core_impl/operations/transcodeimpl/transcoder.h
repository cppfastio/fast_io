#pragma once

namespace fast_io
{

namespace manipulators
{

template <::std::integral chartype, typename T>
struct basic_transcoder_t
{
	using char_type = chartype;
	using transcoder_value_type = T;
	using manip_tag = ::fast_io::manip_tag_t;
	::fast_io::basic_io_scatter_t<char_type> reference;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#else
	[[no_unique_address]]
#endif
#endif
	transcoder_value_type transcoder;
};

template <typename stryp, typename T>
	requires alias_printable<stryp>
inline constexpr auto transcode(stryp &&sct, T &&t)
{
	return ::fast_io::manipulators::basic_transcoder_t<typename decltype(print_alias_define(sct))::value_type, T *>{print_alias_define(::std::forward<stryp>(sct)), __builtin_addressof(t)};
}

template <::std::integral to_char_type, ::std::integral from_char_type, typename T>
inline constexpr auto status_io_print_forward(::fast_io::io_alias_type_t<to_char_type>, ::fast_io::manipulators::basic_transcoder_t<from_char_type, T *> t)
{
	return ::fast_io::manipulators::basic_transcoder_t<from_char_type, decltype(transcode_forward_ref<from_char_type, to_char_type>(*t.transcode))>{t.reference, transcode_forward_ref<from_char_type, to_char_type>(*t.transcode)};
}
} // namespace manipulators

template <typename char_type, typename T>
concept transcode_imaginary_printable = ::std::integral<char_type> && requires() {
	typename T::char_type;
	typename T::transcoder_value_type;
	requires(::std::same_as<T, ::fast_io::manipulators::basic_transcoder_t<typename T::char_type, typename T::transcoder_value_type>> &&
			 (sizeof(typename T::transcoder_value_type::from_char_type) == sizeof(typename T::transcoder_value_type::to_char_type) &&
			  ::fast_io::operations::decay::defines::has_transcode_decay_define<typename T::transcoder_value_type> &&
			  ::fast_io::operations::decay::defines::has_transcode_min_tosize_decay_define<typename T::transcoder_value_type> &&
			  ::fast_io::operations::decay::defines::has_transcode_imaginary_decay_define<typename T::transcoder_value_type>)) ||
				((sizeof(typename T::transcoder_value_type::to_char_type) == 1) &&
				 ::fast_io::operations::decay::defines::has_transcode_bytes_decay_define<typename T::transcoder_value_type> &&
				 ::fast_io::operations::decay::defines::has_transcode_bytes_min_tosize_decay_define<typename T::transcoder_value_type> &&
				 ::fast_io::operations::decay::defines::has_transcode_bytes_imaginary_decay_define<typename T::transcoder_value_type>);
};

} // namespace fast_io
