#pragma once

namespace fast_io
{

template<typename handletype,::std::integral chtype>
class basic_generic_posix_dev_urandom
{
public:
	using handle_type = handletype;
	using observer_type = decltype(::fast_io::manipulators::io_stream_ref(*((handle_type*)nullptr)));
	using input_char_type = chtype;
	handle_type handle;
	template<typename... Args>
	requires ((sizeof...(Args)!=0)&&std::constructible_from<handle_type,Args...>)
	explicit constexpr basic_generic_posix_dev_urandom(Args&& ...args):handle(::std::forward<Args>(args)...){}

	constexpr basic_generic_posix_dev_urandom():handle(u8"/dev/urandom",::fast_io::open_mode::in){}
	explicit constexpr basic_generic_posix_dev_urandom(::std::nullptr_t){}

	constexpr void close()
	{
		handle.close();
	}
};

template<typename observetype,::std::integral chtype>
class basic_generic_posix_dev_urandom_ref:public observetype
{
public:
	using observer_type = observetype;
	using native_handle_type = typename observer_type::native_handle_type;
	using input_char_type = typename native_handle_type::input_char_type;
	using output_char_type = typename native_handle_type::output_char_type;
	constexpr basic_generic_posix_dev_urandom_ref() noexcept = default;
	explicit constexpr basic_generic_posix_dev_urandom_ref(native_handle_type hd) noexcept:
		observer_type{hd}{}
};

template<typename handletype,::std::integral char_type>
inline constexpr auto io_stream_ref_define(
	basic_generic_posix_dev_urandom<handletype,char_type>& pdur) noexcept
{
	return basic_generic_posix_dev_urandom_ref<typename basic_generic_posix_dev_urandom<handletype,char_type>::observer_type,char_type>{
		pdur.handle.native_handle()};
}

template<typename observetype,::std::integral char_type>
inline constexpr basic_generic_posix_dev_urandom_ref<observetype,char_type> io_stream_ref_define(
	basic_generic_posix_dev_urandom_ref<observetype,char_type> pdur) noexcept
{
	return pdur;
}

template<typename observetype,::std::integral char_type>
inline constexpr void io_stream_require_secure_clear_define(basic_generic_posix_dev_urandom_ref<observetype,char_type>){}


template<::std::integral char_type>
using basic_posix_dev_urandom = basic_generic_posix_dev_urandom<::fast_io::basic_posix_file<char_type>,
			char_type>;

template<::std::integral char_type>
using basic_posix_dev_urandom_ref = basic_generic_posix_dev_urandom_ref<
	typename basic_posix_dev_urandom<char_type>::observer_type,
			char_type>;

using posix_dev_urandom = basic_posix_dev_urandom<char>;
using posix_dev_urandom_ref = basic_posix_dev_urandom_ref<char>;

}
