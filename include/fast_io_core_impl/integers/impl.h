#pragma once
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC system_header
#endif

namespace fast_io
{

namespace manipulators
{

enum class scalar_placement:char8_t
{
none,left,middle,right,internal
};

enum class floating_format:char8_t
{
general,fixed,scientific,hexfloat
};

enum class lc_time_flag:std::uint_least8_t
{
none,
d_t_fmt,
d_fmt,
t_fmt,
t_fmt_ampm,
date_fmt,
era_d_t_fmt,
era_d_fmt,
era_t_fmt
};

struct scalar_flags
{
	std::size_t base{10};
	bool alphabet{};
	bool showbase{};
	bool showpos{};
	bool noskipws{};
	bool uppercase_showbase{};
	bool uppercase{};
	bool uppercase_e{};
	bool showpos_e{true};
	bool comma{};
	bool full{};
	scalar_placement placement{scalar_placement::none};
	floating_format floating{floating_format::general};
	lc_time_flag time_flag{};
	bool localeparse{};
	bool line{};
};

inline constexpr scalar_flags integral_default_scalar_flags{.floating=floating_format::fixed};
inline constexpr scalar_flags floating_point_default_scalar_flags{};
inline constexpr scalar_flags address_default_scalar_flags{.base=16,.showbase=true,.full=true,.floating=floating_format::fixed};

}

namespace details
{
template<std::size_t bs,bool upper,bool shbase,bool fll>
inline constexpr ::fast_io::manipulators::scalar_flags base_mani_flags_cache{.base=bs,.showbase=shbase,.uppercase=((bs<=10)?false:upper),.full=fll,.floating=::fast_io::manipulators::floating_format::fixed};

template<bool upper>
inline constexpr ::fast_io::manipulators::scalar_flags boolalpha_mani_flags_cache{.alphabet=true,.uppercase=upper};

template<bool uppercase,bool comma>
inline constexpr ::fast_io::manipulators::scalar_flags hexafloat_mani_flags_cache{.showbase=true,.uppercase_showbase=uppercase,.uppercase=uppercase,.uppercase_e=uppercase,.comma=comma,.floating=::fast_io::manipulators::floating_format::hexfloat};

template<bool uppercase,bool comma,::fast_io::manipulators::floating_format fm>
inline constexpr ::fast_io::manipulators::scalar_flags dcmfloat_mani_flags_cache{.uppercase=uppercase,.uppercase_e=uppercase,.comma=comma,.floating=fm};

template<bool uppercase,bool shbase>
inline constexpr ::fast_io::manipulators::scalar_flags cryptohash_mani_flags_cache{.base=16,.showbase=shbase,.uppercase_showbase=uppercase,.uppercase=uppercase,.floating=::fast_io::manipulators::floating_format::fixed};

template<std::size_t bs,bool noskipws>
inline constexpr ::fast_io::manipulators::scalar_flags base_scan_mani_flags_cache{.base=bs,.noskipws=noskipws,.floating=::fast_io::manipulators::floating_format::fixed};

}

namespace manipulators
{

template<scalar_flags flags,typename T>
struct scalar_manip_t
{
	using scalar_flags_type = scalar_flags;
	using manip_tag = manip_tag_t;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	T reference;
};

template<scalar_flags flags,typename T>
struct scalar_manip_precision_t
{
	using scalar_flags_type = scalar_flags;
	using manip_tag = manip_tag_t;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	T reference;
	std::size_t precision;
};

template<scalar_placement flags,typename T>
struct width_t
{
	using manip_tag = manip_tag_t;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	T reference;
	std::size_t width;
};

template<scalar_placement flags,typename T,std::integral ch_type>
struct width_ch_t
{
	using manip_tag = manip_tag_t;
	using char_type = ch_type;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	T reference;
	std::size_t width;
	char_type ch;
};

template<std::size_t bs,bool shbase=false,bool full=false,typename scalar_type>
requires ((2<=bs&&bs<=36)&&(::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>))
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<bs,false,shbase,full>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),std::remove_cvref_t<scalar_type>,std::uintptr_t>> base(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<std::size_t bs,bool shbase=false,bool full=false,typename scalar_type>
requires ((2<=bs&&bs<=36)&&(::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>))
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<bs,true,shbase,full>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),std::remove_cvref_t<scalar_type>,std::uintptr_t>> baseupper(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}


template<bool shbase=false,bool full=false,typename scalar_type>
requires (::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>)
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<16,false,shbase,full>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),std::remove_cvref_t<scalar_type>,std::uintptr_t>> hex(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<bool shbase=false,bool full=false,typename scalar_type>
requires (::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>)
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<16,true,shbase,full>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),std::remove_cvref_t<scalar_type>,std::uintptr_t>> hexupper(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}


template<bool shbase=false,typename scalar_type>
requires (::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>)
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<16,false,shbase,true>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),::fast_io::details::my_make_unsigned_t<std::remove_cvref_t<scalar_type>>,std::uintptr_t>> uhexfull(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<typename scalar_type>
requires (::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>)
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<16,false,true,true>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),::fast_io::details::my_make_unsigned_t<std::remove_cvref_t<scalar_type>>,std::uintptr_t>> addrvw(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<bool shbase=false,typename scalar_type>
requires (::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>)
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<16,true,shbase,true>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),::fast_io::details::my_make_unsigned_t<std::remove_cvref_t<scalar_type>>,std::uintptr_t>> uhexupperfull(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<bool shbase=false,bool full=false,typename scalar_type>
requires (::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>)
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<10,false,shbase,full>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),std::remove_cvref_t<scalar_type>,std::uintptr_t>> dec(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<bool shbase=false,bool full=false,typename scalar_type>
requires (::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>)
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<8,false,shbase,full>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),std::remove_cvref_t<scalar_type>,std::uintptr_t>> oct(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<bool shbase=false,bool full=false,typename scalar_type>
requires (::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>)
inline constexpr scalar_manip_t<::fast_io::details::base_mani_flags_cache<2,false,shbase,full>,std::conditional_t<(::fast_io::details::my_integral<scalar_type>),std::remove_cvref_t<scalar_type>,std::uintptr_t>> bin(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_integral<scalar_type>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<scalar_flags flags,typename scalar_type>
requires (((2<=flags.base&&flags.base<=36&&(::fast_io::details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>))||(flags.base==10&&::fast_io::details::my_floating_point<scalar_type>)))
inline constexpr scalar_manip_t<flags,std::conditional_t<(::fast_io::details::my_integral<scalar_type>||std::floating_point<scalar_type>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>),std::remove_cvref_t<scalar_type>,std::uintptr_t>> scalar(scalar_type t) noexcept
{
	if constexpr(std::same_as<scalar_type,std::nullptr_t>)
		return {};
	else if constexpr(::fast_io::details::my_floating_point<scalar_type>)
	{
		if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
		||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
		)
		{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
			if constexpr(sizeof(scalar_type)>sizeof(double))
				return scalar_manip_t<floating_point_default_scalar_flags,__float128>{static_cast<__float128>(t)};
			else
#endif
				return scalar_manip_t<floating_point_default_scalar_flags,double>{static_cast<double>(t)};
		}
		else
			return scalar_manip_t<floating_point_default_scalar_flags,std::remove_cvref_t<scalar_type>>{t};
	}
	else if constexpr(::fast_io::details::my_integral<scalar_type>||::fast_io::details::my_floating_point<scalar_type>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>)
		return {t};
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return {::fast_io::bit_cast<std::uintptr_t>(t)};
	else
		return {::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))};
}

template<bool upper=false>
inline constexpr scalar_manip_t<::fast_io::details::boolalpha_mani_flags_cache<upper>,bool> boolalpha(bool b) noexcept
{
	return {b};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto hexfloat(scalar_type t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,__float128>{static_cast<__float128>(t)};
		else
#endif
			return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,double>{static_cast<double>(t)};
	}
	else
		return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,std::remove_cvref_t<scalar_type>>{t};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto hexfloat(scalar_type t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,__float128>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,double>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,std::remove_cvref_t<scalar_type>>{t,n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_hexfloat(scalar_type t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,__float128>{static_cast<__float128>(t)};
		else
#endif
			return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,double>{static_cast<double>(t)};
	}
	else
		return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,std::remove_cvref_t<scalar_type>>{t};
}


template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_hexfloat(scalar_type t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,__float128>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,double>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,std::remove_cvref_t<scalar_type>>{t,n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto general(scalar_type t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,__float128>{static_cast<__float128>(t)};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,double>{static_cast<double>(t)};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,std::remove_cvref_t<scalar_type>>{t};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_general(scalar_type t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,__float128>{static_cast<__float128>(t)};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,double>{static_cast<double>(t)};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,std::remove_cvref_t<scalar_type>>{t};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto general(scalar_type t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,__float128>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,double>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,std::remove_cvref_t<scalar_type>>{t,n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_general(scalar_type t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,__float128>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,double>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,std::remove_cvref_t<scalar_type>>{t,n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto fixed(scalar_type t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,__float128>{static_cast<__float128>(t)};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,double>{static_cast<double>(t)};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,std::remove_cvref_t<scalar_type>>{t};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_fixed(scalar_type t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,__float128>{static_cast<__float128>(t)};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,double>{static_cast<double>(t)};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,std::remove_cvref_t<scalar_type>>{t};
}


template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto fixed(scalar_type t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,__float128>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,double>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,std::remove_cvref_t<scalar_type>>{t,n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_fixed(scalar_type t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,__float128>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,double>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,std::remove_cvref_t<scalar_type>>{t,n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto scientific(scalar_type t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,__float128>{static_cast<__float128>(t)};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,double>{static_cast<double>(t)};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,std::remove_cvref_t<scalar_type>>{t};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_scientific(scalar_type t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,__float128>{static_cast<__float128>(t)};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,double>{static_cast<double>(t)};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,std::remove_cvref_t<scalar_type>>{t};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto scientific(scalar_type t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,__float128>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,double>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,std::remove_cvref_t<scalar_type>>{t,n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_scientific(scalar_type t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,__float128>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,double>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,std::remove_cvref_t<scalar_type>>{t,n};
}

}

namespace details
{

template<std::integral char_type,std::size_t base>
inline constexpr auto generate_base_prefix_array() noexcept
{
	static_assert(2<=base&&base<=36);
	if constexpr(base<10)
	{
	//0[9]0000
		using char_type_unsigned = std::make_unsigned_t<char_type>;
		return ::fast_io::freestanding::array<char_type,4>{char_literal_v<u8'0',char_type>,char_literal_v<u8'[',char_type>,
		static_cast<char_type>(static_cast<char_type_unsigned>(char_literal_v<u8'0',char_type>+static_cast<char8_t>(base))),char_literal_v<u8']',char_type>};
	}
	else
	{
		constexpr char8_t decade{static_cast<char8_t>(static_cast<char8_t>(base)/static_cast<char8_t>(10u))},
			unit{static_cast<char8_t>(static_cast<char8_t>(base)%static_cast<char8_t>(10u))};
		using char_type_unsigned = std::make_unsigned_t<char_type>;
		return ::fast_io::freestanding::array<char_type,5>{char_literal_v<u8'0',char_type>,char_literal_v<u8'[',char_type>,
		static_cast<char_type>(static_cast<char_type_unsigned>(char_literal_v<u8'0',char_type>+static_cast<char8_t>(decade))),
		static_cast<char_type>(static_cast<char_type_unsigned>(char_literal_v<u8'0',char_type>+static_cast<char8_t>(unit))),
		char_literal_v<u8']',char_type>};
	}
}

template<std::integral char_type,std::size_t base>
inline constexpr auto base_prefix_array{generate_base_prefix_array<char_type,base>()};

template<std::size_t base,bool uppercase_showbase,::fast_io::freestanding::random_access_iterator Iter>
constexpr Iter print_reserve_show_base_impl(Iter iter)
{
	static_assert(2<=base&&base<=36);
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(base==2)
	{
		if constexpr(uppercase_showbase)
		{
			if constexpr(std::same_as<char_type,char>)
				iter=copy_string_literal("0B",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				iter=copy_string_literal(L"0B",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				iter=copy_string_literal(u"0B",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				iter=copy_string_literal(U"0B",iter);
			else
				iter=copy_string_literal(u8"0B",iter);
		}
		else
		{
			if constexpr(std::same_as<char_type,char>)
				iter=copy_string_literal("0b",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				iter=copy_string_literal(L"0b",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				iter=copy_string_literal(u"0b",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				iter=copy_string_literal(U"0b",iter);
			else
				iter=copy_string_literal(u8"0b",iter);
		}
	}
	else if constexpr(base==3)
	{
		if constexpr(uppercase_showbase)
		{
			if constexpr(std::same_as<char_type,char>)
				iter=copy_string_literal("0T",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				iter=copy_string_literal(L"0T",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				iter=copy_string_literal(u"0T",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				iter=copy_string_literal(U"0T",iter);
			else
				iter=copy_string_literal(u8"0T",iter);
		}
		else
		{
			if constexpr(std::same_as<char_type,char>)
				iter=copy_string_literal("0t",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				iter=copy_string_literal(L"0t",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				iter=copy_string_literal(u"0t",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				iter=copy_string_literal(U"0t",iter);
			else
				iter=copy_string_literal(u8"0t",iter);
		}
	}
	else if constexpr(base==8)
	{
		*iter=char_literal_v<u8'0',char_type>;
		++iter;
	}
	else if constexpr(base==16)
	{
		if constexpr(uppercase_showbase)
		{
			if constexpr(std::same_as<char_type,char>)
				iter=copy_string_literal("0X",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				iter=copy_string_literal(L"0X",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				iter=copy_string_literal(u"0X",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				iter=copy_string_literal(U"0X",iter);
			else
				iter=copy_string_literal(u8"0X",iter);
		}
		else
		{
			if constexpr(std::same_as<char_type,char>)
				iter=copy_string_literal("0x",iter);
			else if constexpr(std::same_as<char_type,wchar_t>)
				iter=copy_string_literal(L"0x",iter);
			else if constexpr(std::same_as<char_type,char16_t>)
				iter=copy_string_literal(u"0x",iter);
			else if constexpr(std::same_as<char_type,char32_t>)
				iter=copy_string_literal(U"0x",iter);
			else
				iter=copy_string_literal(u8"0x",iter);
		}
	}
	else if constexpr(base!=10)
	{
		constexpr auto arr{base_prefix_array<char_type,base>};
		constexpr std::size_t sz{arr.size()};
		iter=non_overlapped_copy_n(arr.data(),sz,iter);
	}
	return iter;
}

template<std::size_t base,bool uppercase,bool ryu_mode=false,::fast_io::freestanding::random_access_iterator Iter,typename T>
constexpr void print_reserve_integral_main_impl(Iter iter,T t,std::size_t len)
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(base<=10&&uppercase)
	{
		print_reserve_integral_main_impl<base,false,ryu_mode>(iter,t,len);//prevent duplications
	}
	else if constexpr(need_seperate_print<T>)
	{
		constexpr std::size_t basetdigits{::fast_io::details::cal_max_int_size<T,base>()};
		constexpr std::size_t sizetdigits{::fast_io::details::cal_max_int_size<optimal_print_unsigned_type,base>()};
		static_assert(basetdigits!=0&&sizetdigits!=0);
		if constexpr(base==2||base==4||base==16)
		{
			optimal_print_unsigned_type high;
			optimal_print_unsigned_type low{::fast_io::details::intrinsics::unpack_generic<T,optimal_print_unsigned_type>(t,high)};
			if(len>sizetdigits)
			{
				print_reserve_integral_main_impl<base,false,false>(iter-sizetdigits,high,len-sizetdigits);
				len=sizetdigits;
			}
			print_reserve_integral_main_impl<base,false,false>(iter,low,len);
		}
		else
		{
			constexpr std::size_t sizetdigitsm1{sizetdigits-1};
			constexpr std::size_t remain_digits{basetdigits-sizetdigitsm1*2};
			constexpr T maxhighdigits{compile_pow10<T,(sizetdigitsm1*2)>};
			if constexpr(!ryu_mode&&remain_digits!=0)
			{
				static_assert(remain_digits<3);
				if constexpr(remain_digits==1)
				{
					if(len==basetdigits)
					{
						T high{t/maxhighdigits};
						t=t%maxhighdigits;
						*(iter-basetdigits)=static_cast<char_type>(static_cast<char8_t>(high)+char_literal_v<u8'0',char_type>);
						--len;
					}
				}
				else
				{
					constexpr std::size_t least_digits{basetdigits-remain_digits};
					if(len>least_digits)
					{
						T high{t/maxhighdigits};
						t=t%maxhighdigits;
						std::uint_least8_t rem{static_cast<std::uint_least8_t>(high)};
						if(len==basetdigits)
						{
							constexpr auto tb{::fast_io::details::get_shared_inline_constexpr_base_table<char_type,base,uppercase>().element};
							non_overlapped_copy_n(tb[rem].element,2,iter-basetdigits);
							len-=2u;
						}
						else
						{
							*(iter+1-basetdigits)=static_cast<char_type>(static_cast<char8_t>(high)+char_literal_v<u8'0',char_type>);
							--len;
						}
					}
				}
			}
			optimal_print_unsigned_type low;
			if(len>sizetdigitsm1)
			{
				constexpr T halfdigits{compile_pow10<T,sizetdigitsm1>};
				optimal_print_unsigned_type high{static_cast<optimal_print_unsigned_type>(t/halfdigits)};
				low=static_cast<optimal_print_unsigned_type>(t%halfdigits);
				print_reserve_integral_main_impl<base,false,false>(iter-sizetdigitsm1,high,len-sizetdigitsm1);
				len=sizetdigitsm1;
			}
			else
			{
				low=static_cast<optimal_print_unsigned_type>(t);
			}
			print_reserve_integral_main_impl<base,false,false>(iter,low,len);
		}
	}
	else
	{
		if constexpr(ryu_mode)
		{
			print_reserve_integral_main_impl<base,uppercase,false>(iter,t,len);
		}
		else
		{
			constexpr auto tb{::fast_io::details::get_shared_inline_constexpr_base_table<char_type,base,uppercase>().element};
			constexpr T pw{static_cast<T>(base*base)};
			std::size_t const len2{len>>static_cast<std::size_t>(1u)};
			for(std::size_t i{};i!=len2;++i)
			{
				auto const rem{t%pw};
				t/=pw;
				non_overlapped_copy_n(tb[rem].element,2,iter-=2);
			}
			if((len&1))
			{
				if constexpr(base<=10)
					*--iter=static_cast<char_type>(t+char_literal_v<u8'0',char_type>);
				else
					*--iter=static_cast<char_type>(tb[t].element[1]);
			}
		}
	}
}

template<bool full,std::size_t base,bool uppercase,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral T>
constexpr Iter print_reserve_integral_withfull_main_impl(Iter first,T u)
{
	if constexpr(base<=10&&uppercase)
	{
		return print_reserve_integral_withfull_main_impl<full,base,false>(first,u);//prevent duplications
	}
	else
	{
		if constexpr(full)
		{
			constexpr std::size_t sz{::fast_io::details::cal_max_int_size<T,base>()};
			if constexpr(sizeof(u)<=sizeof(unsigned))
				print_reserve_integral_main_impl<base,uppercase>(first+=sz,static_cast<unsigned>(u),sz);
			else
				print_reserve_integral_main_impl<base,uppercase>(first+=sz,u,sz);
			return first;
		}
		else
		{
			if constexpr(::std::is_pointer_v<Iter>&&base==10&&(std::numeric_limits<std::uint_least32_t>::digits==32u))
			{
				return ::fast_io::details::jeaiii::jeaiii_main(first,u);
			}
			else if constexpr(::fast_io::freestanding::contiguous_iterator<Iter>&&base==10&&(std::numeric_limits<std::uint_least32_t>::digits==32u))
			{
				return ::fast_io::details::jeaiii::jeaiii_main(::fast_io::freestanding::to_address(first),u)-::fast_io::freestanding::to_address(first)+first;
			}
			else
			{
				std::size_t const sz{chars_len<base,false>(u)};
				if constexpr(sizeof(u)<=sizeof(unsigned))
					print_reserve_integral_main_impl<base,uppercase>(first+=sz,static_cast<unsigned>(u),sz);
				else
					print_reserve_integral_main_impl<base,uppercase>(first+=sz,u,sz);
				return first;
			}
		}
	
	}
}

template<std::size_t base,bool uppercase,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral T>
inline constexpr void print_reserve_integral_withfull_precise_main_impl(Iter last,T u,std::size_t n)
{
	if constexpr(sizeof(u)<=sizeof(unsigned)&&sizeof(unsigned)<=sizeof(std::size_t))
		print_reserve_integral_main_impl<base,uppercase>(last,static_cast<unsigned>(u),n);
	else
	{
		if constexpr(sizeof(std::size_t)<sizeof(u))
		{
			
		}
		else
		{
			print_reserve_integral_main_impl<base,uppercase>(last,u,n);
		}
	}
}

template<std::size_t base,
	bool showbase=false,
	bool uppercase_showbase=false,
	bool showpos=false,
	bool uppercase=false,
	bool full=false,
	typename int_type,::fast_io::freestanding::random_access_iterator Iter>
constexpr Iter print_reserve_integral_define(Iter first,int_type t)
{
	if constexpr(base<=10&&uppercase)
	{
		return print_reserve_integral_define<base,showbase,uppercase_showbase,showpos,false,full>(first,t);//prevent duplications
	}
	else
	{
		static_assert((2<=base)&&(base<=36));
		using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
		if constexpr(std::same_as<bool,std::remove_cvref_t<int_type>>)
		{
			if constexpr(showpos)
			{
				*first=char_literal_v<u8'+',char_type>;
				++first;
			}
			if constexpr(showbase&&(base!=10))
				first=print_reserve_show_base_impl<base,uppercase_showbase>(first);
			*first=t?char_literal_v<u8'1',char_type>:char_literal_v<u8'0',char_type>;
			++first;
			return first;
		}
		else
		{
			using unsigned_type = ::fast_io::details::my_make_unsigned_t<int_type>;
			unsigned_type u{static_cast<unsigned_type>(t)};
			if constexpr(showpos)
			{
				if constexpr(::fast_io::details::my_unsigned_integral<int_type>)
				{
					*first=char_literal_v<u8'+',char_type>;
				}
				else
				{
					if(t<0)
					{
						*first=char_literal_v<u8'-',char_type>;
						constexpr unsigned_type zero{};
						u=zero-u;
					}
					else
					{
						*first=char_literal_v<u8'+',char_type>;
					}
				}
				++first;
			}
			else
			{
				if constexpr(::fast_io::details::my_signed_integral<int_type>)
				{
					if(t<0)
					{
						*first=char_literal_v<u8'-',char_type>;
						++first;
						constexpr unsigned_type zero{};
						u=zero-u;
					}
				}
			}
			if constexpr(showbase&&(base!=10))
				first=print_reserve_show_base_impl<base,uppercase_showbase>(first);
			return print_reserve_integral_withfull_main_impl<full,base,uppercase>(first,u);
		}
	}
}

template<std::size_t base,
	bool showbase=false,
	bool uppercase_showbase=false,
	bool showpos=false,
	bool uppercase=false,
	typename int_type,::fast_io::freestanding::random_access_iterator Iter>
constexpr void print_reserve_integral_define_precise(Iter start,std::size_t n,int_type t)
{	
	if constexpr(base<=10&&uppercase)
	{
		return print_reserve_integral_define_precise<base,showbase,uppercase_showbase,showpos,false>(start,n,t);//prevent duplications
	}
	else
	{
		auto first{start};
		static_assert((2<=base)&&(base<=36));
		using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
		if constexpr(std::same_as<bool,std::remove_cvref_t<int_type>>)
		{
			if constexpr(showpos)
			{
				*first=char_literal_v<u8'+',char_type>;
				++first;
			}
			if constexpr(showbase&&(base!=10))
				first=print_reserve_show_base_impl<base,uppercase_showbase>(first);
			*first=t?char_literal_v<u8'1',char_type>:char_literal_v<u8'0',char_type>;
			++first;
			return first;
		}
		else
		{
			using unsigned_type = ::fast_io::details::my_make_unsigned_t<int_type>;
			unsigned_type u{static_cast<unsigned_type>(t)};
			if constexpr(showpos)
			{
				if constexpr(::fast_io::details::my_unsigned_integral<int_type>)
				{
					*first=char_literal_v<u8'+',char_type>;
				}
				else
				{
					if(t<0)
					{
						*first=char_literal_v<u8'-',char_type>;
						constexpr unsigned_type zero{};
						u=zero-u;
					}
					else
					{
						*first=char_literal_v<u8'+',char_type>;
					}
				}
				++first;
			}
			else
			{
				if constexpr(::fast_io::details::my_signed_integral<int_type>)
				{
					if(t<0)
					{
						*first=char_literal_v<u8'-',char_type>;
						++first;
						constexpr unsigned_type zero{};
						u=zero-u;
					}
				}
			}
			if constexpr(showbase&&(base!=10))
				first=print_reserve_show_base_impl<base,uppercase_showbase>(first);
			auto ed{start+n};
			if constexpr(my_unsigned_integral<int_type>&&!showbase&&!showpos)
				print_reserve_integral_withfull_precise_main_impl<base,uppercase>(ed,u,n);
			else
				print_reserve_integral_withfull_precise_main_impl<base,uppercase>(ed,u,static_cast<std::size_t>(ed-first));
		}
	}
}

template<
std::size_t base=10,
bool showbase=false,
bool showpos=false,
my_integral T>
inline constexpr std::size_t print_reserve_scalar_size_impl()
{
	std::size_t total_sum{};
	if constexpr(showpos)
		++total_sum;
	if constexpr(showbase)
	{
		if constexpr(base==2||base==3||base==16)
			total_sum+=2;	//0b 0t 0x
		else if constexpr(base==8)
			++total_sum;	//0
		else if constexpr(base<10)
			total_sum+=4;	//0[9]
		else if constexpr(10<base)
			total_sum+=5;	//0[36]
		//base==10 does not have showbase
	}
	if constexpr(std::same_as<std::remove_cvref_t<T>,bool>)
		++total_sum;
	else
	{
		if constexpr(!showpos)
		{
			if constexpr(my_signed_integral<T>)
				++total_sum;
		}
		total_sum+=::fast_io::details::cal_max_int_size<T,base>();
	}
	return total_sum;
}


template<std::size_t base,
bool showbase,
bool showpos,
my_integral T>
inline constexpr std::size_t print_integer_reserved_size_cache{print_reserve_scalar_size_impl<base,showbase,showpos,T>()};


template<
std::size_t base,
bool showbase,
bool showpos,
bool full,
my_integral T>
inline constexpr std::size_t print_reserve_scalar_cal_precise_cache_size_impl()
{
	std::size_t total_sum{};
	if constexpr(showpos)
		++total_sum;
	if constexpr(showbase)
	{
		if constexpr(base==2||base==3||base==16)
			total_sum+=2;	//0b 0t 0x
		else if constexpr(base==8)
			++total_sum;	//0
		else if constexpr(base<10)
			total_sum+=4;	//0[9]
		else if constexpr(10<base)
			total_sum+=5;	//0[36]
		//base==10 does not have showbase
		if constexpr(full)
		{
			if constexpr(std::same_as<std::remove_cvref_t<T>,bool>)
				++total_sum;
			else
			{
				if constexpr(!showpos)
				{
					if constexpr(my_signed_integral<T>)
						++total_sum;
				}
				total_sum+=::fast_io::details::cal_max_int_size<T,base>();
			}
		}
	}
	return total_sum;
}

template<std::size_t base,
bool showbase,
bool showpos,
bool full,
my_integral T>
inline constexpr std::size_t print_integer_reserved_precise_size_cache{print_reserve_scalar_cal_precise_cache_size_impl<base,showbase,showpos,full,T>()};


template<
std::size_t base,
bool showbase,
bool showpos,
bool full,
my_integral T>
inline constexpr std::size_t print_integer_reserved_precise_size(T t)
{
	if constexpr(std::same_as<std::remove_cvref_t<T>,bool>)
		return print_integer_reserved_size_cache<base,showbase,showpos,T>;
	else if constexpr(full)
	{
		if constexpr(!showpos&&my_signed_integral<T>)
		{
			std::size_t total_sum{print_integer_reserved_precise_size_cache<base,showbase,showpos,full,T>};
			if(t<0)
				++total_sum;
			return total_sum;
		}
		else
			return print_integer_reserved_precise_size_cache<base,showbase,showpos,full,T>;
	}
	else
	{
		std::size_t total_sum{print_integer_reserved_precise_size_cache<base,showbase,showpos,false,T>};
		if constexpr(my_signed_integral<T>)
		{
			using unsigned_type = my_make_unsigned_t<T>;
			unsigned_type u{static_cast<unsigned_type>(t)};
			if(t<0)
			{
				++total_sum;
				constexpr unsigned_type zero{};
				u=static_cast<unsigned_type>(zero-u);
			}
			return total_sum+chars_len<base,false>(u);
		}
		else
		{
			return total_sum+chars_len<base,false>(t);
		}
	}
}

template<std::integral char_type,
std::size_t base,
bool showbase,
bool uppercase_showbase,
bool showpos,
bool uppercase,
bool full>
inline constexpr std::size_t nullptr_print_optimization_call_size_impl() noexcept
{
	::fast_io::freestanding::array<char_type,print_integer_reserved_size_cache<base,showbase,showpos,std::uintptr_t>> arr;
	auto res{print_reserve_integral_define<base,showbase,uppercase_showbase,showpos,uppercase,full>(arr.data(),std::uintptr_t{})};
	return static_cast<std::size_t>(res-arr.data());
}

template<std::integral char_type,
std::size_t base,
bool showbase,
bool uppercase_showbase,
bool showpos,
bool uppercase,
bool full>
inline constexpr std::size_t nullptr_print_optimization_call_size_cache{
	nullptr_print_optimization_call_size_impl<char_type,base,showbase,uppercase_showbase,showpos,uppercase,full>()};

template<std::integral char_type,
std::size_t base,
bool showbase,
bool uppercase_showbase,
bool showpos,
bool uppercase,
bool full>
inline constexpr auto nullptr_print_optimization_call_impl() noexcept
{
	constexpr std::size_t sz{nullptr_print_optimization_call_size_cache<char_type,base,showbase,uppercase_showbase,showpos,uppercase,full>};
	::fast_io::freestanding::array<char_type,sz> arr{};
	auto res{print_reserve_integral_define<base,showbase,uppercase_showbase,showpos,uppercase,full>(arr.data(),std::uintptr_t{})};
	return arr;
}

template<std::integral char_type,
std::size_t base,
bool showbase,
bool uppercase_showbase,
bool showpos,
bool uppercase,
bool full
>
inline constexpr auto nullptr_print_optimization_call_cache{nullptr_print_optimization_call_impl<char_type,base,showbase,uppercase_showbase,showpos,uppercase,full>()};

template<bool uppercase,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter print_reserve_boolalpha_impl(Iter iter,bool b)
{
	using char_type = std::remove_cvref_t<::fast_io::freestanding::iter_value_t<Iter>>;
	if(b)
	{
		if constexpr(std::same_as<char_type,char>)
		{
			if constexpr(uppercase)
				return copy_string_literal("TRUE",iter);
			else
				return copy_string_literal("true",iter);
		}
		else if constexpr(std::same_as<char_type,wchar_t>)
		{
			if constexpr(uppercase)
				return copy_string_literal(L"TRUE",iter);
			else
				return copy_string_literal(L"true",iter);
		}
		else if constexpr(std::same_as<char_type,char16_t>)
		{
			if constexpr(uppercase)
				return copy_string_literal(u"TRUE",iter);
			else
				return copy_string_literal(u"true",iter);
		}
		else if constexpr(std::same_as<char_type,char32_t>)
		{
			if constexpr(uppercase)
				return copy_string_literal(U"TRUE",iter);
			else
				return copy_string_literal(U"true",iter);
		}
		else
		{
			if constexpr(uppercase)
				return copy_string_literal(u8"TRUE",iter);
			else
				return copy_string_literal(u8"true",iter);
		}
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
		{
			if constexpr(uppercase)
				return copy_string_literal("FALSE",iter);
			else
				return copy_string_literal("false",iter);
		}
		else if constexpr(std::same_as<char_type,wchar_t>)
		{
			if constexpr(uppercase)
				return copy_string_literal(L"FALSE",iter);
			else
				return copy_string_literal(L"false",iter);
		}
		else if constexpr(std::same_as<char_type,char16_t>)
		{
			if constexpr(uppercase)
				return copy_string_literal(u"FALSE",iter);
			else
				return copy_string_literal(u"false",iter);
		}
		else if constexpr(std::same_as<char_type,char32_t>)
		{
			if constexpr(uppercase)
				return copy_string_literal(U"FALSE",iter);
			else
				return copy_string_literal(U"false",iter);
		}
		else
		{
			if constexpr(uppercase)
				return copy_string_literal(u8"FALSE",iter);
			else
				return copy_string_literal(u8"false",iter);
		}
	}
}

template<bool uppercase,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter print_reserve_nullptr_alphabet_impl(Iter iter)
{
	using char_type = std::remove_cvref_t<::fast_io::freestanding::iter_value_t<Iter>>;
	if constexpr(std::same_as<char_type,char>)
	{
		if constexpr(uppercase)
			return copy_string_literal("NULLPTR",iter);
		else
			return copy_string_literal("nullptr",iter);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		if constexpr(uppercase)
			return copy_string_literal(L"NULLPTR",iter);
		else
			return copy_string_literal(L"nullptr",iter);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		if constexpr(uppercase)
			return copy_string_literal(u"NULLPTR",iter);
		else
			return copy_string_literal(u"nullptr",iter);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		if constexpr(uppercase)
			return copy_string_literal(U"NULLPTR",iter);
		else
			return copy_string_literal(U"nullptr",iter);
	}
	else
	{
		if constexpr(uppercase)
			return copy_string_literal(u8"NULLPTR",iter);
		else
			return copy_string_literal(u8"nullptr",iter);
	}
}

}

template<typename scalar_type>
requires (details::my_integral<scalar_type>||std::is_pointer_v<std::remove_cvref_t<scalar_type>>||::fast_io::freestanding::contiguous_iterator<scalar_type>||::fast_io::details::my_floating_point<scalar_type>||std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>)
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto print_alias_define(io_alias_t,scalar_type t) noexcept
{
	if constexpr(details::my_integral<scalar_type>)
		return manipulators::scalar_manip_t<manipulators::integral_default_scalar_flags,scalar_type>{t};
	else if constexpr(details::my_floating_point<scalar_type>)
	{
		if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
		||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
		)
		{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
			if constexpr(sizeof(scalar_type)>sizeof(double))
				return manipulators::scalar_manip_t<manipulators::floating_point_default_scalar_flags,__float128>{static_cast<__float128>(t)};
			else
#endif
				return manipulators::scalar_manip_t<manipulators::floating_point_default_scalar_flags,double>{static_cast<double>(t)};
		}
		else
			return manipulators::scalar_manip_t<manipulators::floating_point_default_scalar_flags,std::remove_cvref_t<scalar_type>>{t};
	}
	else if constexpr(std::same_as<std::nullptr_t,std::remove_cvref_t<scalar_type>>)
	{
		return manipulators::scalar_manip_t<manipulators::scalar_flags{.alphabet=true},std::nullptr_t>{};
	}
	else if constexpr(std::is_pointer_v<std::remove_cvref_t<scalar_type>>)
		return manipulators::scalar_manip_t<manipulators::address_default_scalar_flags,std::uintptr_t>{
			::fast_io::bit_cast<std::uintptr_t>(t)
		};
	else
		return manipulators::scalar_manip_t<manipulators::address_default_scalar_flags,std::uintptr_t>{
			::fast_io::bit_cast<std::uintptr_t>(::fast_io::freestanding::to_address(t))
		};
}

template<std::integral char_type,typename T>
requires (details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>)
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,T>) noexcept
{
	if constexpr(std::same_as<std::remove_cv_t<T>,std::byte>)
		return details::print_integer_reserved_size_cache<10,false,false,char8_t>;
	else
		return details::print_integer_reserved_size_cache<10,false,false,std::remove_cvref_t<T>>;
}

template<freestanding::random_access_iterator Iter,typename T>
requires (details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>)
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]//always inline to reduce inline depth in GCC and LLVM clang
#endif
inline constexpr Iter print_reserve_define(io_reserve_type_t<freestanding::iter_value_t<Iter>,T>,Iter iter,T t) noexcept
{
	if constexpr(std::same_as<std::remove_cv_t<T>,std::byte>)
		return details::print_reserve_integral_define<10,false,false,false,false,false>(iter,static_cast<char8_t>(t));
	else
		return details::print_reserve_integral_define<10,false,false,false,false,false>(iter,t);
}

template<std::integral char_type,manipulators::scalar_flags flags,typename T>
requires (details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>||std::same_as<std::remove_cvref_t<T>,std::nullptr_t>)
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,manipulators::scalar_manip_t<flags,T>>) noexcept
{
	if constexpr(flags.alphabet)
	{
		static_assert((std::same_as<std::remove_cvref_t<T>,bool>||std::same_as<std::remove_cvref_t<T>,std::nullptr_t>),"only bool and std::nullptr_t support alphabet output");
		if constexpr(std::same_as<std::remove_cvref_t<T>,bool>)
			return 5; // u8"false"
		else
			return 7; // u8"nullptr"
	}
	else if constexpr(std::same_as<std::remove_cvref_t<T>,std::nullptr_t>)
		return details::nullptr_print_optimization_call_size_cache<char_type,flags.base,flags.showbase,flags.uppercase_showbase,flags.showpos,flags.uppercase,flags.full>;
	else if constexpr(std::same_as<std::remove_cv_t<T>,std::byte>)
		return details::print_integer_reserved_size_cache<flags.base,flags.showbase,flags.showpos,char8_t>;
	else
		return details::print_integer_reserved_size_cache<flags.base,flags.showbase,flags.showpos,T>;
}

template<::fast_io::freestanding::random_access_iterator Iter,manipulators::scalar_flags flags,typename T>
requires (details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>||std::same_as<std::remove_cvref_t<T>,std::nullptr_t>)
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]//always inline to reduce inline depth in GCC and LLVM clang
#endif
constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,::fast_io::manipulators::scalar_manip_t<flags,T>>,Iter iter,::fast_io::manipulators::scalar_manip_t<flags,T> t) noexcept
{
	if constexpr(flags.alphabet)
	{
		static_assert((std::same_as<std::remove_cvref_t<T>,bool>||std::same_as<std::remove_cvref_t<T>,std::nullptr_t>),"only bool and std::nullptr_t support alphabet output");
		if constexpr(std::same_as<std::remove_cvref_t<T>,bool>)
			return details::print_reserve_boolalpha_impl<flags.uppercase>(iter,t.reference);
		else
			return details::print_reserve_nullptr_alphabet_impl<flags.uppercase>(iter);
	}
	else if constexpr(std::same_as<std::remove_cv_t<T>,std::nullptr_t>)
	{
		constexpr auto& cache{details::nullptr_print_optimization_call_cache<::fast_io::freestanding::iter_value_t<Iter>,flags.base,flags.showbase,flags.uppercase_showbase,flags.showpos,flags.uppercase,flags.full>};
		constexpr std::size_t n{cache.size()};
		return details::non_overlapped_copy_n(cache.element,n,iter);
	}
	else if constexpr(std::same_as<std::remove_cv_t<T>,std::byte>)
		return details::print_reserve_integral_define<flags.base,flags.showbase,flags.uppercase_showbase,flags.showpos,flags.uppercase,flags.full>(iter,static_cast<char8_t>(t.reference));
	else
		return details::print_reserve_integral_define<flags.base,flags.showbase,flags.uppercase_showbase,flags.showpos,flags.uppercase,flags.full>(iter,t.reference);
}


template<std::integral char_type,manipulators::scalar_flags flags,typename T>
requires ((details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>) && !flags.alphabet)
inline constexpr std::size_t print_reserve_precise_size(io_reserve_type_t<char_type,manipulators::scalar_manip_t<flags,T>>,manipulators::scalar_manip_t<flags,T> t) noexcept
{
	if constexpr(std::same_as<T,std::byte>)
		return details::print_integer_reserved_precise_size<flags.base,flags.showbase,flags.showpos,flags.full>(static_cast<char8_t>(t.reference));
	else
		return details::print_integer_reserved_precise_size<flags.base,flags.showbase,flags.showpos,flags.full>(t.reference);
}

template<std::integral char_type,::fast_io::freestanding::random_access_iterator Iter,manipulators::scalar_flags flags,typename T>
requires ((details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>) && !flags.alphabet)
inline constexpr void print_reserve_precise_define(io_reserve_type_t<char_type,manipulators::scalar_manip_t<flags,T>>,Iter iter,std::size_t n,manipulators::scalar_manip_t<flags,T> t) noexcept
{
	if constexpr(std::same_as<T,std::byte>)
		details::print_reserve_integral_define_precise<flags.base,flags.showbase,flags.uppercase_showbase,flags.showpos,flags.uppercase>(iter,n,static_cast<char8_t>(t.reference));
	else
		details::print_reserve_integral_define_precise<flags.base,flags.showbase,flags.uppercase_showbase,flags.showpos,flags.uppercase>(iter,n,t.reference);
}

template<std::integral char_type,manipulators::scalar_flags flags,typename T>
requires ((details::my_integral<T>||std::same_as<std::remove_cv_t<T>,std::byte>) && !flags.alphabet&&(
	flags.showpos||(details::my_signed_integral<T>&&!std::same_as<std::remove_cv_t<T>,bool>)
))
inline constexpr std::size_t print_define_internal_shift(io_reserve_type_t<char_type,manipulators::scalar_manip_t<flags,T>>,manipulators::scalar_manip_t<flags,T> t) noexcept
{
	if constexpr(flags.showpos)
	{
		return 1;
	}
	else
	{
		return t.reference<0;
	}
}

}
