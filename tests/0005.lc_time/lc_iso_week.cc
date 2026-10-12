// ISO week boundaries — lc_iso_week math on known dates
#include <fast_io.h>
#include <fast_io_i18n.h>

using namespace fast_io::io;

int main()
{
	struct tc
	{
		::std::int_least32_t y, mo, dy, wday, week, iso_year;
	};
	constexpr tc cases[]{
		{2021, 1, 1, 5, 53, 2020}, // ISO 2020-W53
		{2020, 12, 31, 4, 53, 2020},
		{2025, 3, 15, 6, 11, 2025},
		{2019, 12, 30, 1, 1, 2020}, // ISO 2020-W01
		{2018, 12, 31, 1, 1, 2019},
		{2023, 1, 1, 7, 52, 2022},
	};
	for (auto const &c : cases)
	{
		::fast_io::details::lc_tm tm{::fast_io::details::lc_tm_from(
			::fast_io::iso8601_timestamp{c.y,
										 static_cast<::std::uint_least8_t>(c.mo),
										 static_cast<::std::uint_least8_t>(c.dy)})};
		::fast_io::details::lc_iso_week(tm);
		::fast_io::println(
			c.y, ::fast_io::mnp::chvw(u8'-'), c.mo,
			::fast_io::mnp::chvw(u8'-'), c.dy, " wday=", c.wday,
			" V=", tm.iso_week, " G=", tm.iso_year,
			::fast_io::mnp::os_c_str(
				(tm.iso_week == c.week && tm.iso_year == c.iso_year)
					? " OK" : " FAIL"));
	}
}
