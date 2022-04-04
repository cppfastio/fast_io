#pragma once

namespace fast_io
{

template<typename char_type>
struct basic_lc_identification
{
	basic_io_scatter_t<char_type> name{};
	basic_io_scatter_t<char_type> encoding{};
	basic_io_scatter_t<char_type> title{};
	basic_io_scatter_t<char_type> source{};
	basic_io_scatter_t<char_type> address{};
	basic_io_scatter_t<char_type> contact{};
	basic_io_scatter_t<char_type> email{};
	basic_io_scatter_t<char_type> tel{};
	basic_io_scatter_t<char_type> fax{};
	basic_io_scatter_t<char_type> language{};
	basic_io_scatter_t<char_type> territory{};
	basic_io_scatter_t<char_type> audience{};
	basic_io_scatter_t<char_type> application{};
	basic_io_scatter_t<char_type> abbreviation{};
	basic_io_scatter_t<char_type> revision{};
	basic_io_scatter_t<char_type> date{};
};

using lc_identification=basic_lc_identification<char>;
using wlc_identification=basic_lc_identification<wchar_t>;
using u8lc_identification=basic_lc_identification<char8_t>;
using u16lc_identification=basic_lc_identification<char16_t>;
using u32lc_identification=basic_lc_identification<char32_t>;

template<typename char_type>
struct basic_lc_monetary
{
	basic_io_scatter_t<char_type> int_curr_symbol{};
	basic_io_scatter_t<char_type> currency_symbol{};
	basic_io_scatter_t<char_type> mon_decimal_point{};
	basic_io_scatter_t<char_type> mon_thousands_sep{};
	basic_io_scatter_t<std::size_t> mon_grouping{};
	basic_io_scatter_t<char_type> positive_sign{};
	basic_io_scatter_t<char_type> negative_sign{};
	std::size_t int_frac_digits{};
	std::size_t frac_digits{};
	std::size_t p_cs_precedes{};
	std::size_t p_sep_by_space{};
	std::size_t n_cs_precedes{};
	std::size_t n_sep_by_space{};
	std::size_t int_p_cs_precedes{};
	std::size_t int_p_sep_by_space{};
	std::size_t int_n_cs_precedes{};
	std::size_t int_n_sep_by_space{};
	std::size_t p_sign_posn{};
	std::size_t n_sign_posn{};
	std::size_t int_p_sign_posn{};
	std::size_t int_n_sign_posn{};
};

using lc_monetary=basic_lc_monetary<char>;
using wlc_monetary=basic_lc_monetary<wchar_t>;
using u8lc_monetary=basic_lc_monetary<char8_t>;
using u16lc_monetary=basic_lc_monetary<char16_t>;
using u32lc_monetary=basic_lc_monetary<char32_t>;

template<typename char_type>
struct basic_lc_numeric
{
	basic_io_scatter_t<char_type> decimal_point{};
	basic_io_scatter_t<char_type> thousands_sep{};
	basic_io_scatter_t<std::size_t> grouping{};
};

using lc_numeric=basic_lc_numeric<char>;
using wlc_numeric=basic_lc_numeric<wchar_t>;
using u8lc_numeric=basic_lc_numeric<char8_t>;
using u16lc_numeric=basic_lc_numeric<char16_t>;
using u32lc_numeric=basic_lc_numeric<char32_t>;

template<typename char_type>
struct basic_lc_time_era
{
	bool direction{};//+ is true, - is false
	std::int_least64_t offset{};
	std::int_least64_t start_date_year{};
	std::uint_least8_t start_date_month{};
	std::uint_least8_t start_date_day{};
	std::int_least8_t  end_date_special{};//-1 is -*, 0 means end_date exist, 1 is +*
	std::int_least64_t end_date_year{};
	std::uint_least8_t end_date_month{};
	std::uint_least8_t end_date_day{};
	basic_io_scatter_t<char_type> era_name;
	basic_io_scatter_t<char_type> era_format;
	basic_io_scatter_t<char_type> era;
};

template<typename char_type>
struct basic_lc_time
{
	basic_io_scatter_t<char_type> abday[7]{};
	basic_io_scatter_t<char_type> day[7]{};
	basic_io_scatter_t<char_type> abmon[12]{};
	basic_io_scatter_t<char_type> ab_alt_mon[12]{};
	basic_io_scatter_t<char_type> mon[12]{};
	basic_io_scatter_t<char_type> d_t_fmt{};
	basic_io_scatter_t<char_type> d_fmt{};
	basic_io_scatter_t<char_type> t_fmt{};
	basic_io_scatter_t<char_type> t_fmt_ampm{};
	basic_io_scatter_t<char_type> date_fmt{};
	basic_io_scatter_t<char_type> am_pm[2]{};
	basic_io_scatter_t<basic_lc_time_era<char_type>> era{};
	basic_io_scatter_t<char_type> era_d_fmt{};
	basic_io_scatter_t<char_type> era_d_t_fmt{};
	basic_io_scatter_t<char_type> era_t_fmt{};
	basic_io_scatter_t<basic_io_scatter_t<char_type>> alt_digits{};
	struct
	{
		std::size_t ndays{7};
		std::int_least64_t first_day{19971201};
		std::size_t first_week{4};
	}week{};
	std::size_t first_weekday{};
	std::size_t first_workday{};
	std::size_t cal_direction{};
	basic_io_scatter_t<basic_io_scatter_t<char_type>> timezone{};
};

using lc_time=basic_lc_time<char>;
using wlc_time=basic_lc_time<wchar_t>;
using u8lc_time=basic_lc_time<char8_t>;
using u16lc_time=basic_lc_time<char16_t>;
using u32lc_time=basic_lc_time<char32_t>;

template<typename char_type>
struct basic_lc_messages
{
	basic_io_scatter_t<char_type> yesexpr{};
	basic_io_scatter_t<char_type> noexpr{};
	basic_io_scatter_t<char_type> yesstr{};
	basic_io_scatter_t<char_type> nostr{};
};

using lc_messages=basic_lc_messages<char>;
using wlc_messages=basic_lc_messages<wchar_t>;
using u8lc_messages=basic_lc_messages<char8_t>;
using u16lc_messages=basic_lc_messages<char16_t>;
using u32lc_messages=basic_lc_messages<char32_t>;

template<typename char_type>
struct basic_lc_paper
{
	std::uint_least64_t width{};
	std::uint_least64_t height{};
};

using lc_paper=basic_lc_paper<char>;
using wlc_paper=basic_lc_paper<wchar_t>;
using u8lc_paper=basic_lc_paper<char8_t>;
using u16lc_paper=basic_lc_paper<char16_t>;
using u32lc_paper=basic_lc_paper<char32_t>;

template<typename char_type>
struct basic_lc_telephone
{
	basic_io_scatter_t<char_type> tel_int_fmt{};
	basic_io_scatter_t<char_type> tel_dom_fmt{};
	basic_io_scatter_t<char_type> int_select{};
	basic_io_scatter_t<char_type> int_prefix{};
};

using lc_telephone=basic_lc_telephone<char>;
using wlc_telephone=basic_lc_telephone<wchar_t>;
using u8lc_telephone=basic_lc_telephone<char8_t>;
using u16lc_telephone=basic_lc_telephone<char16_t>;
using u32lc_telephone=basic_lc_telephone<char32_t>;

template<typename char_type>
struct basic_lc_name
{
	basic_io_scatter_t<char_type> name_fmt{};
	basic_io_scatter_t<char_type> name_gen{};
	basic_io_scatter_t<char_type> name_miss{};
	basic_io_scatter_t<char_type> name_mr{};
	basic_io_scatter_t<char_type> name_mrs{};
	basic_io_scatter_t<char_type> name_ms{};
};

using lc_name=basic_lc_name<char>;
using wlc_name=basic_lc_name<wchar_t>;
using u8lc_name=basic_lc_name<char8_t>;
using u16lc_name=basic_lc_name<char16_t>;
using u32lc_name=basic_lc_name<char32_t>;

template<typename char_type>
struct basic_lc_address
{
	basic_io_scatter_t<char_type> postal_fmt{};
	basic_io_scatter_t<char_type> country_name{};
	basic_io_scatter_t<char_type> country_post{};
	basic_io_scatter_t<char_type> country_ab2{};
	basic_io_scatter_t<char_type> country_ab3{};
	std::uint_least64_t country_num{};
	basic_io_scatter_t<char_type> country_car{};
	basic_io_scatter_t<char_type> country_isbn{};
	basic_io_scatter_t<char_type> lang_name{};
	basic_io_scatter_t<char_type> lang_ab{};
	basic_io_scatter_t<char_type> lang_term{};
	basic_io_scatter_t<char_type> lang_lib{};
};

using lc_address=basic_lc_address<char>;
using wlc_address=basic_lc_address<wchar_t>;
using u8lc_address=basic_lc_address<char8_t>;
using u16lc_address=basic_lc_address<char16_t>;
using u32lc_address=basic_lc_address<char32_t>;

template<typename char_type>
struct basic_lc_measurement
{
	std::uint_least64_t measurement{};
};

using lc_measurement=basic_lc_measurement<char>;
using wlc_measurement=basic_lc_measurement<wchar_t>;
using u8lc_measurement=basic_lc_measurement<char8_t>;
using u16lc_measurement=basic_lc_measurement<char16_t>;
using u32lc_measurement=basic_lc_measurement<char32_t>;

template<typename char_type>
struct basic_lc_keyboard
{
	basic_io_scatter_t<basic_io_scatter_t<char_type>> keyboards{};
};

using lc_keyboard=basic_lc_keyboard<char>;
using wlc_keyboard=basic_lc_keyboard<wchar_t>;
using u8lc_keyboard=basic_lc_keyboard<char8_t>;
using u16lc_keyboard=basic_lc_keyboard<char16_t>;
using u32lc_keyboard=basic_lc_keyboard<char32_t>;

template<typename char_type>
struct basic_lc_all
{
	basic_lc_identification<char_type> identification{};
	basic_lc_monetary<char_type> monetary{};
	basic_lc_numeric<char_type> numeric{};
	basic_lc_time<char_type> time{};
	basic_lc_messages<char_type> messages{};
	basic_lc_paper<char_type> paper{};
	basic_lc_telephone<char_type> telephone{};
	basic_lc_name<char_type> name{};
	basic_lc_address<char_type> address{};
	basic_lc_measurement<char_type> measurement{};
	basic_lc_keyboard<char_type> keyboard{};
};

using lc_all=basic_lc_all<char>;
using wlc_all=basic_lc_all<wchar_t>;
using u8lc_all=basic_lc_all<char8_t>;
using u16lc_all=basic_lc_all<char16_t>;
using u32lc_all=basic_lc_all<char32_t>;

struct lc_locale
{
	lc_all const* all{};
	wlc_all const* wall{};
	u8lc_all const* u8all{};
	u16lc_all const* u16all{};
	u32lc_all const* u32all{};
};

template<std::integral char_type>
requires (std::same_as<char_type,char>||std::same_as<char_type,wchar_t>||
	std::same_as<char_type,char8_t>||std::same_as<char_type,char16_t>||std::same_as<char_type,char32_t>)
inline constexpr basic_lc_all<char_type> const* get_all(lc_locale const& loc) noexcept
{
	if constexpr(std::same_as<char_type,char>)
		return loc.all;
	else if constexpr(std::same_as<char_type,wchar_t>)
		return loc.wall;
	else if constexpr(std::same_as<char_type,char8_t>)
		return loc.u8all;
	else if constexpr(std::same_as<char_type,char16_t>)
		return loc.u16all;
	else if constexpr(std::same_as<char_type,char32_t>)
		return loc.u32all;
	else
		return {};
}

}
