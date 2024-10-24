#pragma once

namespace fast_io
{

using i18n_scatter_size_type = std::uint_least32_t;

template <typename T>
struct basic_scatter
{
	using size_type = i18n_scatter_size_type;
	[[nodiscard]] inline static size_type buffer_size(std::vector<T> const &which) noexcept
	{
		return static_cast<size_type>(which.size());
	}
	template <std::ranges::range rg>
	[[nodiscard]] inline static basic_scatter append_range(std::vector<T> &which, rg &&r) noexcept
	{
		auto rva{buffer_size(which)};
		which.append_range(r);
		return {rva, buffer_size(which) - rva};
	}
	[[nodiscard]] inline std::span<T> get_from(std::vector<T> const &which) const noexcept
	{
		return {which.data() + rva, length};
	}
	size_type rva;
	size_type length;
};

template <typename char_type>
struct basic_lc_identification
{
	basic_scatter<char_type> title{};
	basic_scatter<char_type> source{};
	basic_scatter<char_type> address{};
	basic_scatter<char_type> contact{};
	basic_scatter<char_type> email{};
	basic_scatter<char_type> tel{};
	basic_scatter<char_type> fax{};
	basic_scatter<char_type> language{};
	basic_scatter<char_type> territory{};
	basic_scatter<char_type> audience{};
	basic_scatter<char_type> application{};
	basic_scatter<char_type> abbreviation{};
	basic_scatter<char_type> revision{};
	basic_scatter<char_type> date{};
};

using lc_identification = basic_lc_identification<char>;
using wlc_identification = basic_lc_identification<wchar_t>;
using u8lc_identification = basic_lc_identification<char8_t>;
using u16lc_identification = basic_lc_identification<char16_t>;
using u32lc_identification = basic_lc_identification<char32_t>;

template <typename char_type>
struct basic_lc_monetary
{
	basic_scatter<char_type> int_curr_symbol{};
	basic_scatter<char_type> currency_symbol{};
	basic_scatter<char_type> mon_decimal_point{};
	basic_scatter<char_type> mon_thousands_sep{};
	basic_scatter<std::size_t> mon_grouping{};
	basic_scatter<char_type> positive_sign{};
	basic_scatter<char_type> negative_sign{};
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

using lc_monetary = basic_lc_monetary<char>;
using wlc_monetary = basic_lc_monetary<wchar_t>;
using u8lc_monetary = basic_lc_monetary<char8_t>;
using u16lc_monetary = basic_lc_monetary<char16_t>;
using u32lc_monetary = basic_lc_monetary<char32_t>;

template <typename char_type>
struct basic_lc_numeric
{
	basic_scatter<char_type> decimal_point{};
	basic_scatter<char_type> thousands_sep{};
	basic_scatter<std::size_t> grouping{};
};

using lc_numeric = basic_lc_numeric<char>;
using wlc_numeric = basic_lc_numeric<wchar_t>;
using u8lc_numeric = basic_lc_numeric<char8_t>;
using u16lc_numeric = basic_lc_numeric<char16_t>;
using u32lc_numeric = basic_lc_numeric<char32_t>;

template <typename char_type>
struct basic_lc_time_era
{
	bool direction{}; //+ is true, - is false
	std::int_least64_t offset{};
	std::int_least64_t start_date_year{};
	std::uint_least8_t start_date_month{};
	std::uint_least8_t start_date_day{};
	std::int_least8_t end_date_special{}; //-1 is -*, 0 means end_date exist, 1 is +*
	std::int_least64_t end_date_year{};
	std::uint_least8_t end_date_month{};
	std::uint_least8_t end_date_day{};
	basic_scatter<char_type> era_name;
	basic_scatter<char_type> era_format;
};

template <typename char_type>
struct basic_lc_time
{
	basic_scatter<char_type> abday[7]{};
	basic_scatter<char_type> day[7]{};
	basic_scatter<char_type> abmon[12]{};
	basic_scatter<char_type> ab_alt_mon[12]{};
	basic_scatter<char_type> mon[12]{};
	basic_scatter<char_type> d_t_fmt{};
	basic_scatter<char_type> d_fmt{};
	basic_scatter<char_type> t_fmt{};
	basic_scatter<char_type> t_fmt_ampm{};
	basic_scatter<char_type> date_fmt{};
	basic_scatter<char_type> am_pm[2]{};
	basic_scatter<basic_lc_time_era<char_type>> era{};
	basic_scatter<char_type> era_d_fmt{};
	basic_scatter<char_type> era_d_t_fmt{};
	basic_scatter<char_type> era_t_fmt{};
	basic_scatter<basic_scatter<char_type>> alt_digits{};
	struct
	{
		std::size_t ndays{7};
		std::int_least64_t first_day{19971201};
		std::size_t first_week{4};
	} week{};
	std::size_t first_weekday{};
	std::size_t first_workday{};
	std::size_t cal_direction{};
	basic_scatter<basic_scatter<char_type>> timezone{};
};

using lc_time = basic_lc_time<char>;
using wlc_time = basic_lc_time<wchar_t>;
using u8lc_time = basic_lc_time<char8_t>;
using u16lc_time = basic_lc_time<char16_t>;
using u32lc_time = basic_lc_time<char32_t>;

template <typename char_type>
struct basic_lc_messages
{
	basic_scatter<char_type> yesexpr{};
	basic_scatter<char_type> noexpr{};
	basic_scatter<char_type> yesstr{};
	basic_scatter<char_type> nostr{};
};

using lc_messages = basic_lc_messages<char>;
using wlc_messages = basic_lc_messages<wchar_t>;
using u8lc_messages = basic_lc_messages<char8_t>;
using u16lc_messages = basic_lc_messages<char16_t>;
using u32lc_messages = basic_lc_messages<char32_t>;

template <typename char_type>
struct basic_lc_paper
{
	std::uint_least64_t width{};
	std::uint_least64_t height{};
};

using lc_paper = basic_lc_paper<char>;
using wlc_paper = basic_lc_paper<wchar_t>;
using u8lc_paper = basic_lc_paper<char8_t>;
using u16lc_paper = basic_lc_paper<char16_t>;
using u32lc_paper = basic_lc_paper<char32_t>;

template <typename char_type>
struct basic_lc_telephone
{
	basic_scatter<char_type> tel_int_fmt{};
	basic_scatter<char_type> tel_dom_fmt{};
	basic_scatter<char_type> int_select{};
	basic_scatter<char_type> int_prefix{};
};

using lc_telephone = basic_lc_telephone<char>;
using wlc_telephone = basic_lc_telephone<wchar_t>;
using u8lc_telephone = basic_lc_telephone<char8_t>;
using u16lc_telephone = basic_lc_telephone<char16_t>;
using u32lc_telephone = basic_lc_telephone<char32_t>;

template <typename char_type>
struct basic_lc_name
{
	basic_scatter<char_type> name_fmt{};
	basic_scatter<char_type> name_gen{};
	basic_scatter<char_type> name_miss{};
	basic_scatter<char_type> name_mr{};
	basic_scatter<char_type> name_mrs{};
	basic_scatter<char_type> name_ms{};
};

using lc_name = basic_lc_name<char>;
using wlc_name = basic_lc_name<wchar_t>;
using u8lc_name = basic_lc_name<char8_t>;
using u16lc_name = basic_lc_name<char16_t>;
using u32lc_name = basic_lc_name<char32_t>;

template <typename char_type>
struct basic_lc_address
{
	basic_scatter<char_type> postal_fmt{};
	basic_scatter<char_type> country_name{};
	basic_scatter<char_type> country_post{};
	basic_scatter<char_type> country_ab2{};
	basic_scatter<char_type> country_ab3{};
	basic_scatter<char_type> country_num{};
	basic_scatter<char_type> country_car{};
	basic_scatter<char_type> country_isbn{};
	basic_scatter<char_type> lang_name{};
	basic_scatter<char_type> lang_ab{};
	basic_scatter<char_type> lang_term{};
	basic_scatter<char_type> lang_lib{};
};

using lc_address = basic_lc_address<char>;
using wlc_address = basic_lc_address<wchar_t>;
using u8lc_address = basic_lc_address<char8_t>;
using u16lc_address = basic_lc_address<char16_t>;
using u32lc_address = basic_lc_address<char32_t>;

template <typename char_type>
struct basic_lc_measurement
{
	std::uint_least64_t measurement{};
};

using lc_measurement = basic_lc_measurement<char>;
using wlc_measurement = basic_lc_measurement<wchar_t>;
using u8lc_measurement = basic_lc_measurement<char8_t>;
using u16lc_measurement = basic_lc_measurement<char16_t>;
using u32lc_measurement = basic_lc_measurement<char32_t>;

template <typename char_type>
struct basic_lc_keyboard
{
	basic_scatter<basic_scatter<char_type>> keyboards{};
};

using lc_keyboard = basic_lc_keyboard<char>;
using wlc_keyboard = basic_lc_keyboard<wchar_t>;
using u8lc_keyboard = basic_lc_keyboard<char8_t>;
using u16lc_keyboard = basic_lc_keyboard<char16_t>;
using u32lc_keyboard = basic_lc_keyboard<char32_t>;

template <typename char_type>
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

using lc_all = basic_lc_all<char>;
using wlc_all = basic_lc_all<wchar_t>;
using u8lc_all = basic_lc_all<char8_t>;
using u16lc_all = basic_lc_all<char16_t>;
using u32lc_all = basic_lc_all<char32_t>;

template <typename char_type>
struct basic_lc_data_storage
{
	std::vector<char_type> chars{};
	std::vector<std::size_t> integers{};
	std::vector<basic_lc_time_era<char_type>> eras{};
	std::vector<basic_scatter<char_type>> strings{};
	template <typename T, typename Self>
	[[nodiscard]] inline constexpr decltype(auto) get_storage(this Self &&self) noexcept
	{
		if constexpr (std::same_as<T, char_type>)
		{
			return (self.chars);
		}
		else if constexpr (std::same_as<T, std::size_t>)
		{
			return (self.integers);
		}
		else if constexpr (std::same_as<T, basic_lc_time_era<char_type>>)
		{
			return (self.eras);
		}
		else if constexpr (std::same_as<T, basic_scatter<char_type>>)
		{
			return (self.strings);
		}
		else
		{
			return;
		}
	}
};

using lc_data_storage = basic_lc_data_storage<char>;
using wlc_data_storage = basic_lc_data_storage<wchar_t>;
using u8lc_data_storage = basic_lc_data_storage<char8_t>;
using u16lc_data_storage = basic_lc_data_storage<char16_t>;
using u32lc_data_storage = basic_lc_data_storage<char32_t>;

template <typename char_type>
struct basic_lc_object
{
	basic_lc_data_storage<char_type> data_storage{};
	basic_lc_all<char_type> all{};
};

using lc_object = basic_lc_object<char>;
using wlc_object = basic_lc_object<wchar_t>;
using u8lc_object = basic_lc_object<char8_t>;
using u16lc_object = basic_lc_object<char16_t>;
using u32lc_object = basic_lc_object<char32_t>;

struct lc_locale
{
	lc_object const *lc;
	wlc_object const *wlc;
	u8lc_object const *u8lc;
	u16lc_object const *u16lc;
	u32lc_object const *u32lc;
};

template <std::integral char_type>
	requires(std::same_as<char_type, char> || std::same_as<char_type, wchar_t> ||
			 std::same_as<char_type, char8_t> || std::same_as<char_type, char16_t> || std::same_as<char_type, char32_t>)
inline constexpr basic_lc_object<char_type> const *get_lc(lc_locale const &loc) noexcept
{
	if constexpr (std::same_as<char_type, char>)
	{
		return loc.lc;
	}
	else if constexpr (std::same_as<char_type, wchar_t>)
	{
		return loc.wlc;
	}
	else if constexpr (std::same_as<char_type, char8_t>)
	{
		return loc.u8lc;
	}
	else if constexpr (std::same_as<char_type, char16_t>)
	{
		return loc.u16lc;
	}
	else if constexpr (std::same_as<char_type, char32_t>)
	{
		return loc.u32lc;
	}
	else
	{
		return {};
	}
}

} // namespace fast_io
