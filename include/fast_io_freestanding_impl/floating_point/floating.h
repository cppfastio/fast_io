#pragma once
#include"jiaendu_fp.h"
#include"floating_integer_part.h"
#include"easy_cases.h"
#include"dragonbox/dragonbox.h"

namespace fast_io
{

template<std::integral char_type,std::floating_point fp>
requires (std::same_as<fp,float>||std::same_as<fp,double>)
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,fp>) noexcept
{
	return 25;
}

namespace manipulators
{

template<rounding_mode round_mode=rounding_mode::nearest_to_even,
std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr 
std::conditional_t<round_mode==rounding_mode::nearest_to_even,fp_type,floating_format_t<fp_type,
floating_representation::general,
round_mode>>
general(fp_type f) noexcept
{
	return {f};
}

template<rounding_mode round_mode=rounding_mode::nearest_to_even,
std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr 
floating_format_t<fp_type,
floating_representation::fixed,
round_mode>
fixed(fp_type f) noexcept
{
	return {f};
}

template<rounding_mode round_mode=rounding_mode::nearest_to_even,
std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr 
floating_format_t<fp_type,
floating_representation::scientific,
round_mode>
scientific(fp_type f) noexcept
{
	return {f};
}

template<rounding_mode round_mode=rounding_mode::nearest_to_even,std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr 
comma_t<floating_format_t<fp_type,
floating_representation::general,
round_mode>>
comma_general(fp_type f) noexcept
{
	return {f};
}

template<rounding_mode round_mode=rounding_mode::nearest_to_even,std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr 
comma_t<floating_format_t<fp_type,
floating_representation::general,
round_mode>>
comma(fp_type f) noexcept
{
	return {f};
}

template<rounding_mode round_mode=rounding_mode::nearest_to_even,std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr 
comma_t<
floating_format_t<fp_type,
floating_representation::fixed,
round_mode>>
comma_fixed(fp_type f) noexcept
{
	return {f};
}

template<rounding_mode round_mode,std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr 
comma_t<floating_format_t<fp_type,
floating_representation::scientific,
round_mode>>
comma_scientific(fp_type f) noexcept
{
	return {f};
}

}

template<std::integral char_type,std::floating_point fp,
	manipulators::floating_representation rep,
	manipulators::rounding_mode round_mode>
requires (std::same_as<fp,float>||std::same_as<fp,double>)
inline constexpr std::size_t print_reserve_size(
	io_reserve_type_t<char_type,
	manipulators::floating_format_t<fp,
	rep,round_mode>>) noexcept
{
	if constexpr(rep==manipulators::floating_representation::fixed)
		return 360;
	else
		return 30;
}


template<std::integral char_type,std::floating_point fp,
	manipulators::floating_representation rep,
	manipulators::rounding_mode round_mode>
requires (std::same_as<fp,float>||std::same_as<fp,double>)
inline constexpr std::size_t print_reserve_size(
	io_reserve_type_t<char_type,
	manipulators::comma_t<manipulators::floating_format_t<fp,
	rep,round_mode>>>) noexcept
{
	if constexpr(rep==manipulators::floating_representation::fixed)
		return 360;
	else
		return 30;
}


template<std::integral char_type,std::floating_point fp>
requires (std::same_as<fp,float>||std::same_as<fp,double>)
inline constexpr std::size_t print_reserve_size(
	io_reserve_type_t<char_type,
	manipulators::comma_t<fp>>) noexcept
{
	return 30;
}


namespace details
{

template<bool comma,
::fast_io::manipulators::floating_representation rep,
::fast_io::manipulators::rounding_mode round,
bool uppercase_e,::fast_io::freestanding::random_access_iterator Iter,std::floating_point fp_type>
inline constexpr Iter fp_print_reserve_define_impl(Iter iter,fp_type f) noexcept
{

	if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_to_even)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_to_odd)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_to_odd)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_toward_plus_infinity)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_toward_plus_infinity)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_toward_minus_infinity)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_toward_minus_infinity)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_toward_zero)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_toward_zero)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_away_from_zero)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_away_from_zero)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_to_even_static_boundary)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_to_even_static_boundary)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_to_odd_static_boundary)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_to_odd_static_boundary)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_toward_plus_infinity_static_boundary)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_toward_plus_infinity_static_boundary)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::nearest_toward_minus_infinity_static_boundary)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::nearest_toward_minus_infinity_static_boundary)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::toward_plus_infinity)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::toward_plus_infinity)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::toward_minus_infinity)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::toward_minus_infinity)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::toward_zero)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::toward_zero)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
	else if constexpr(round==::fast_io::manipulators::rounding_mode::away_from_zero)
	{
		auto decm{::fast_io::jkj::dragonbox::to_decimal(f,jkj::dragonbox::policy::rounding_mode::away_from_zero)};
		return fp::normal_case<rep,uppercase_e,(comma?u8',':u8'.')>(iter,decm.is_negative,decm.significand,decm.exponent);
	}
}

}

template<::fast_io::freestanding::random_access_iterator Iter,std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,fp_type>,Iter iter,fp_type f) noexcept
{
	return details::fp_print_reserve_define_impl<false,
	manipulators::floating_representation::general,
	::fast_io::manipulators::rounding_mode::nearest_to_even,false>(iter,f);
}

template<::fast_io::freestanding::random_access_iterator Iter,std::floating_point fp_type>
requires (std::same_as<fp_type,float>||std::same_as<fp_type,double>)
inline constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,manipulators::comma_t<fp_type>>,Iter iter,manipulators::comma_t<fp_type> f) noexcept
{
	return details::fp_print_reserve_define_impl<true,manipulators::floating_representation::general,
	::fast_io::manipulators::rounding_mode::nearest_to_even
	,false>(iter,f);
}


template<::fast_io::freestanding::random_access_iterator Iter,std::floating_point fp,
	manipulators::floating_representation rep,
	manipulators::rounding_mode round_mode>
requires (std::same_as<fp,float>||std::same_as<fp,double>)
inline constexpr Iter print_reserve_define(
	io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,
	manipulators::floating_format_t<fp,rep,round_mode>>,Iter iter,
	manipulators::floating_format_t<fp,rep,round_mode> f) noexcept
{
	return details::fp_print_reserve_define_impl<false,
	rep,
	round_mode
	,false>(iter,f.reference);
}


template<::fast_io::freestanding::random_access_iterator Iter,std::floating_point fp,
	manipulators::floating_representation rep,
	manipulators::rounding_mode round_mode>
requires (std::same_as<fp,float>||std::same_as<fp,double>)
inline constexpr Iter print_reserve_define(
	io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,
	manipulators::comma_t<manipulators::floating_format_t<fp,rep,round_mode>>>,Iter iter,
	manipulators::comma_t<manipulators::floating_format_t<fp,rep,round_mode>> f) noexcept
{
	return details::fp_print_reserve_define_impl<true,
	rep,
	round_mode
	,false>(iter,f.reference.reference);
}


}