#pragma once

namespace fast_io
{

namespace manipulators
{
template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto hexfloat(std::complex<scalar_type> t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))}};
		else
#endif
			return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))}};
	}
	else
		return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))}};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto hexfloat(std::complex<scalar_type> t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))},n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))},n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,false>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))},n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_hexfloat(std::complex<scalar_type> t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))}};
		else
#endif
			return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))}};
	}
	else
		return scalar_manip_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))}};
}


template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_hexfloat(std::complex<scalar_type> t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))},n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))},n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::hexafloat_mani_flags_cache<uppercase,true>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))},n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto general(std::complex<scalar_type> t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))}};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))}};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))}};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_general(std::complex<scalar_type> t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))}};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))}};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))}};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto general(std::complex<scalar_type> t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))},n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))},n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::general>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))},n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_general(std::complex<scalar_type> t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))},n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))},n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::general>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))},n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto fixed(std::complex<scalar_type> t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))}};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))}};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))}};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_fixed(std::complex<scalar_type> t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))}};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))}};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))}};
}


template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto fixed(std::complex<scalar_type> t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))},n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))},n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::fixed>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))},n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_fixed(std::complex<scalar_type> t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))},n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))},n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::fixed>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))},n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto scientific(std::complex<scalar_type> t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))}};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))}};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))}};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_scientific(std::complex<scalar_type> t) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))}};
		else
#endif
			return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))}};
	}
	else
		return scalar_manip_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))}};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto scientific(std::complex<scalar_type> t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,std::complex<__float128>>{{static_cast<__float128>(::std::real(t)),static_cast<__float128>(::std::imag(t))},n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,std::complex<double>>{{static_cast<double>(::std::real(t)),static_cast<double>(::std::imag(t))},n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,false,manipulators::floating_format::scientific>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))},n};
}

template<bool uppercase=false,typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
inline constexpr auto comma_scientific(std::complex<scalar_type> t,std::size_t n) noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<scalar_type>,long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
	||std::same_as<std::remove_cvref_t<scalar_type>,__float128>
#endif
	)
	{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
		if constexpr(sizeof(scalar_type)>sizeof(double))
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,::std::complex<__float128>>{static_cast<__float128>(t),n};
		else
#endif
			return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,::std::complex<double>>{static_cast<double>(t),n};
	}
	else
		return scalar_manip_precision_t<::fast_io::details::dcmfloat_mani_flags_cache<uppercase,true,manipulators::floating_format::scientific>,std::complex<std::remove_cvref_t<scalar_type>>>{{static_cast<std::remove_cvref_t<scalar_type>>(::std::real(t)),static_cast<std::remove_cvref_t<scalar_type>>(::std::imag(t))},n};
}

}



template<typename scalar_type>
requires (::fast_io::details::my_floating_point<scalar_type>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto print_alias_define(io_alias_t,::std::complex<scalar_type> t) noexcept
{
	return ::fast_io::mnp::general<false>(t);
}

namespace details
{
template<::fast_io::manipulators::scalar_flags flags,typename T,::std::integral char_type>
inline constexpr char_type* print_reserve_complex_impl(char_type* iter,T real,T imag) noexcept
{
	*iter=char_literal_v<u8'(',char_type>;
	++iter;
	iter=print_reserve_define(io_reserve_type<char_type,::fast_io::manipulators::scalar_manip_t<flags,T>>,iter,::fast_io::manipulators::scalar_manip_t<flags,T>{real});
	*iter=char_literal_v<(flags.comma?u8'.':u8','),char_type>;
	++iter;
	iter=print_reserve_define(io_reserve_type<char_type,::fast_io::manipulators::scalar_manip_t<flags,T>>,iter,::fast_io::manipulators::scalar_manip_t<flags,T>{imag});
	*iter=char_literal_v<u8')',char_type>;
	++iter;
	return iter;
}
}

template<std::integral char_type,manipulators::scalar_flags flags,details::my_floating_point flt>
requires (flags.base==10)
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,manipulators::scalar_manip_t<flags,std::complex<flt>>>) noexcept
{
	constexpr std::size_t v{print_reserve_size(io_reserve_type<char_type,manipulators::scalar_manip_t<flags,flt>>)};
	constexpr std::size_t res{(v*2u)+3};
	return res;
}

template<::std::integral char_type,manipulators::scalar_flags flags,details::my_floating_point flt>
requires (flags.base==10)
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,manipulators::scalar_manip_t<flags,std::complex<flt>>>,char_type* iter,manipulators::scalar_manip_t<flags,std::complex<flt>> f) noexcept
{
	return ::fast_io::details::print_reserve_complex_impl<flags>(iter,::std::real(f.reference),::std::imag(f.reference));
}

}
