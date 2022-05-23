#pragma once

namespace fast_io
{

namespace details
{

template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_identification(output bos,basic_lc_identification<char_type> const& identification)
{
	if constexpr(std::same_as<char_type,char>)
		print_freestanding(bos,"LC_IDENTIFICATION\n"
			"name\t\"",identification.name,"\"\n"
			"encoding\t\"",identification.encoding,"\"\n"
			"title\t\"",identification.title,"\"\n"
			"source\t\"",identification.source,"\"\n"
			"address\t\"",identification.address,"\"\n"
			"contact\t\"",identification.contact,"\"\n"
			"email\t\"",identification.email,"\"\n"
			"tel\t\"",identification.tel,"\"\n"
			"fax\t\"",identification.fax,"\"\n"
			"language\t\"",identification.language,"\"\n"
			"territory\t\"",identification.territory,"\"\n"
			"audience\t\"",identification.audience,"\"\n"
			"application\t\"",identification.application,"\"\n"
			"abbreviation\t\"",identification.abbreviation,"\"\n"
			"revision\t\"",identification.revision,"\"\n"
			"date\t\"",identification.date,"\"\n"
			"END LC_IDENTIFICATION");
	else if constexpr(std::same_as<char_type,wchar_t>)
		print_freestanding(bos,L"LC_IDENTIFICATION\n"
			L"name\t\"",identification.name,L"\"\n"
			L"encoding\t\"",identification.encoding,L"\"\n"
			L"title\t\"",identification.title,L"\"\n"
			L"source\t\"",identification.source,L"\"\n"
			L"address\t\"",identification.address,L"\"\n"
			L"contact\t\"",identification.contact,L"\"\n"
			L"email\t\"",identification.email,L"\"\n"
			L"tel\t\"",identification.tel,L"\"\n"
			L"fax\t\"",identification.fax,L"\"\n"
			L"language\t\"",identification.language,L"\"\n"
			L"territory\t\"",identification.territory,L"\"\n"
			L"audience\t\"",identification.audience,L"\"\n"
			L"application\t\"",identification.application,L"\"\n"
			L"abbreviation\t\"",identification.abbreviation,L"\"\n"
			L"revision\t\"",identification.revision,L"\"\n"
			L"date\t\"",identification.date,L"\"\n"
			L"END LC_IDENTIFICATION");
	else if constexpr(std::same_as<char_type,char16_t>)
		print_freestanding(bos,u"LC_IDENTIFICATION\n"
			u"name\t\"",identification.name,u"\"\n"
			u"encoding\t\"",identification.encoding,u"\"\n"
			u"title\t\"",identification.title,u"\"\n"
			u"source\t\"",identification.source,u"\"\n"
			u"address\t\"",identification.address,u"\"\n"
			u"contact\t\"",identification.contact,u"\"\n"
			u"email\t\"",identification.email,u"\"\n"
			u"tel\t\"",identification.tel,u"\"\n"
			u"fax\t\"",identification.fax,u"\"\n"
			u"language\t\"",identification.language,u"\"\n"
			u"territory\t\"",identification.territory,u"\"\n"
			u"audience\t\"",identification.audience,u"\"\n"
			u"application\t\"",identification.application,u"\"\n"
			u"abbreviation\t\"",identification.abbreviation,u"\"\n"
			u"revision\t\"",identification.revision,u"\"\n"
			u"date\t\"",identification.date,u"\"\n"
			u"END LC_IDENTIFICATION");
	else if constexpr(std::same_as<char_type,char32_t>)
		print_freestanding(bos,U"LC_IDENTIFICATION\n"
			U"name\t\"",identification.name,U"\"\n"
			U"encoding\t\"",identification.encoding,U"\"\n"
			U"title\t\"",identification.title,U"\"\n"
			U"source\t\"",identification.source,U"\"\n"
			U"address\t\"",identification.address,U"\"\n"
			U"contact\t\"",identification.contact,U"\"\n"
			U"email\t\"",identification.email,U"\"\n"
			U"tel\t\"",identification.tel,U"\"\n"
			U"fax\t\"",identification.fax,U"\"\n"
			U"language\t\"",identification.language,U"\"\n"
			U"territory\t\"",identification.territory,U"\"\n"
			U"audience\t\"",identification.audience,U"\"\n"
			U"application\t\"",identification.application,U"\"\n"
			U"abbreviation\t\"",identification.abbreviation,U"\"\n"
			U"revision\t\"",identification.revision,U"\"\n"
			U"date\t\"",identification.date,U"\"\n"
			U"END LC_IDENTIFICATION");
	else if constexpr(std::same_as<char_type,char8_t>)
		print_freestanding(bos,u8"LC_IDENTIFICATION\n"
			u8"name\t\"",identification.name,u8"\"\n"
			u8"encoding\t\"",identification.encoding,u8"\"\n"
			u8"title\t\"",identification.title,u8"\"\n"
			u8"source\t\"",identification.source,u8"\"\n"
			u8"address\t\"",identification.address,u8"\"\n"
			u8"contact\t\"",identification.contact,u8"\"\n"
			u8"email\t\"",identification.email,u8"\"\n"
			u8"tel\t\"",identification.tel,u8"\"\n"
			u8"fax\t\"",identification.fax,u8"\"\n"
			u8"language\t\"",identification.language,u8"\"\n"
			u8"territory\t\"",identification.territory,u8"\"\n"
			u8"audience\t\"",identification.audience,u8"\"\n"
			u8"application\t\"",identification.application,u8"\"\n"
			u8"abbreviation\t\"",identification.abbreviation,u8"\"\n"
			u8"revision\t\"",identification.revision,u8"\"\n"
			u8"date\t\"",identification.date,u8"\"\n"
			u8"END LC_IDENTIFICATION");
}


}

namespace details
{
template<typename output>
constexpr void print_grouping(output bos,basic_io_scatter_t<std::size_t> grouping)
{
	using char_type = typename output::char_type;
	if(grouping.len==0)
	{
		if constexpr(std::same_as<char,char_type>)
			print_freestanding(bos,"-1");
		else if constexpr(std::same_as<wchar_t,char_type>)
			print_freestanding(bos,L"-1");
		else if constexpr(std::same_as<char16_t,char_type>)
			print_freestanding(bos,u"-1");
		else if constexpr(std::same_as<char32_t,char_type>)
			print_freestanding(bos,U"-1");
		else if constexpr(std::same_as<char8_t,char_type>)
			print_freestanding(bos,u8"-1");
		return;
	}
	for(std::size_t i{};i!=grouping.len;++i)
	{
		if(i)
			put(bos,char_literal_v<u8';',char_type>);
		if(grouping.base[i]==std::numeric_limits<std::size_t>::max())
		{
			if constexpr(std::same_as<char,char_type>)
				print_freestanding(bos,"-1");
			else if constexpr(std::same_as<wchar_t,char_type>)
				print_freestanding(bos,L"-1");
			else if constexpr(std::same_as<char16_t,char_type>)
				print_freestanding(bos,u"-1");
			else if constexpr(std::same_as<char32_t,char_type>)
				print_freestanding(bos,U"-1");
			else if constexpr(std::same_as<char8_t,char_type>)
				print_freestanding(bos,u8"-1");
		}
		else
			print_freestanding(bos,grouping.base[i]);
	}
}

template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_monetary(output bos,basic_lc_monetary<char_type> const& monetary)
{
	if constexpr(std::same_as<char_type,char>)
	{
		print_freestanding(bos,"LC_MONETARY\n"
			"int_curr_symbol\t\"",monetary.int_curr_symbol,"\"\n"
			"currency_symbol\t\"",monetary.currency_symbol,"\"\n"
			"mon_decimal_point\t\"",monetary.mon_decimal_point,"\"\n"
			"mon_thousands_sep\t\"",monetary.mon_thousands_sep,"\"\n"
			"mon_grouping\t");
		::fast_io::details::print_grouping(bos,monetary.mon_grouping);
		print_freestanding(bos,"\n"
			"positive_sign\t\"",monetary.positive_sign,"\"\n"
			"negative_sign\t\"",monetary.negative_sign,"\"\n"
			"int_frac_digits\t",monetary.int_frac_digits,"\n"
			"frac_digits\t",monetary.frac_digits,"\n"
			"p_cs_precedes\t",monetary.p_cs_precedes,"\n"
			"p_sep_by_space\t",monetary.p_sep_by_space,"\n"
			"n_cs_precedes\t",monetary.n_cs_precedes,"\n"
			"n_sep_by_space\t",monetary.n_sep_by_space,"\n"
			"int_p_cs_precedes\t",monetary.int_p_cs_precedes,"\n"
			"int_p_sep_by_space\t",monetary.int_p_sep_by_space,"\n"
			"int_n_cs_precedes\t",monetary.int_n_cs_precedes,"\n"
			"int_n_sep_by_space\t",monetary.int_n_sep_by_space,"\n"
			"p_sign_posn\t",monetary.p_sign_posn,"\n"
			"n_sign_posn\t",monetary.n_sign_posn,"\n"
			"int_p_sign_posn\t",monetary.int_p_sign_posn,"\n"
			"int_n_sign_posn\t",monetary.int_n_sign_posn,"\n"
			"END LC_MONETARY");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print_freestanding(bos,L"LC_MONETARY\n"
			L"int_curr_symbol\t\"",monetary.int_curr_symbol,L"\"\n"
			L"currency_symbol\t\"",monetary.currency_symbol,L"\"\n"
			L"mon_decimal_point\t\"",monetary.mon_decimal_point,L"\"\n"
			L"mon_thousands_sep\t\"",monetary.mon_thousands_sep,L"\"\n"
			L"mon_grouping\t");
		::fast_io::details::print_grouping(bos,monetary.mon_grouping);
		print_freestanding(bos,L"\n"
			L"positive_sign\t\"",monetary.positive_sign,L"\"\n"
			L"negative_sign\t\"",monetary.negative_sign,L"\"\n"
			L"int_frac_digits\t",monetary.int_frac_digits,L"\n"
			L"frac_digits\t",monetary.frac_digits,L"\n"
			L"p_cs_precedes\t",monetary.p_cs_precedes,L"\n"
			L"p_sep_by_space\t",monetary.p_sep_by_space,L"\n"
			L"n_cs_precedes\t",monetary.n_cs_precedes,L"\n"
			L"n_sep_by_space\t",monetary.n_sep_by_space,L"\n"
			L"int_p_cs_precedes\t",monetary.int_p_cs_precedes,L"\n"
			L"int_p_sep_by_space\t",monetary.int_p_sep_by_space,L"\n"
			L"int_n_cs_precedes\t",monetary.int_n_cs_precedes,L"\n"
			L"int_n_sep_by_space\t",monetary.int_n_sep_by_space,L"\n"
			L"p_sign_posn\t",monetary.p_sign_posn,L"\n"
			L"n_sign_posn\t",monetary.n_sign_posn,L"\n"
			L"int_p_sign_posn\t",monetary.int_p_sign_posn,L"\n"
			L"int_n_sign_posn\t",monetary.int_n_sign_posn,L"\n"
			L"END LC_MONETARY");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print_freestanding(bos,u"LC_MONETARY\n"
			u"int_curr_symbol\t\"",monetary.int_curr_symbol,u"\"\n"
			u"currency_symbol\t\"",monetary.currency_symbol,u"\"\n"
			u"mon_decimal_point\t\"",monetary.mon_decimal_point,u"\"\n"
			u"mon_thousands_sep\t\"",monetary.mon_thousands_sep,u"\"\n"
			u"mon_grouping\t");
		::fast_io::details::print_grouping(bos,monetary.mon_grouping);
		print_freestanding(bos,u"\n"
			u"positive_sign\t\"",monetary.positive_sign,u"\"\n"
			u"negative_sign\t\"",monetary.negative_sign,u"\"\n"
			u"int_frac_digits\t",monetary.int_frac_digits,u"\n"
			u"frac_digits\t",monetary.frac_digits,u"\n"
			u"p_cs_precedes\t",monetary.p_cs_precedes,u"\n"
			u"p_sep_by_space\t",monetary.p_sep_by_space,u"\n"
			u"n_cs_precedes\t",monetary.n_cs_precedes,u"\n"
			u"n_sep_by_space\t",monetary.n_sep_by_space,u"\n"
			u"int_p_cs_precedes\t",monetary.int_p_cs_precedes,u"\n"
			u"int_p_sep_by_space\t",monetary.int_p_sep_by_space,u"\n"
			u"int_n_cs_precedes\t",monetary.int_n_cs_precedes,u"\n"
			u"int_n_sep_by_space\t",monetary.int_n_sep_by_space,u"\n"
			u"p_sign_posn\t",monetary.p_sign_posn,u"\n"
			u"n_sign_posn\t",monetary.n_sign_posn,u"\n"
			u"int_p_sign_posn\t",monetary.int_p_sign_posn,u"\n"
			u"int_n_sign_posn\t",monetary.int_n_sign_posn,u"\n"
			u"END LC_MONETARY");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print_freestanding(bos,U"LC_MONETARY\n"
			U"int_curr_symbol\t\"",monetary.int_curr_symbol,U"\"\n"
			U"currency_symbol\t\"",monetary.currency_symbol,U"\"\n"
			U"mon_decimal_point\t\"",monetary.mon_decimal_point,U"\"\n"
			U"mon_thousands_sep\t\"",monetary.mon_thousands_sep,U"\"\n"
			U"mon_grouping\t");
		::fast_io::details::print_grouping(bos,monetary.mon_grouping);
		print_freestanding(bos,U"\n"
			U"positive_sign\t\"",monetary.positive_sign,U"\"\n"
			U"negative_sign\t\"",monetary.negative_sign,U"\"\n"
			U"int_frac_digits\t",monetary.int_frac_digits,U"\n"
			U"frac_digits\t",monetary.frac_digits,U"\n"
			U"p_cs_precedes\t",monetary.p_cs_precedes,U"\n"
			U"p_sep_by_space\t",monetary.p_sep_by_space,U"\n"
			U"n_cs_precedes\t",monetary.n_cs_precedes,U"\n"
			U"n_sep_by_space\t",monetary.n_sep_by_space,U"\n"
			U"int_p_cs_precedes\t",monetary.int_p_cs_precedes,U"\n"
			U"int_p_sep_by_space\t",monetary.int_p_sep_by_space,U"\n"
			U"int_n_cs_precedes\t",monetary.int_n_cs_precedes,U"\n"
			U"int_n_sep_by_space\t",monetary.int_n_sep_by_space,U"\n"
			U"p_sign_posn\t",monetary.p_sign_posn,U"\n"
			U"n_sign_posn\t",monetary.n_sign_posn,U"\n"
			U"int_p_sign_posn\t",monetary.int_p_sign_posn,U"\n"
			U"int_n_sign_posn\t",monetary.int_n_sign_posn,U"\n"
			U"END LC_MONETARY");
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		print_freestanding(bos,u8"LC_MONETARY\n"
			u8"int_curr_symbol\t\"",monetary.int_curr_symbol,u8"\"\n"
			u8"currency_symbol\t\"",monetary.currency_symbol,u8"\"\n"
			u8"mon_decimal_point\t\"",monetary.mon_decimal_point,u8"\"\n"
			u8"mon_thousands_sep\t\"",monetary.mon_thousands_sep,u8"\"\n"
			u8"mon_grouping\t");
		::fast_io::details::print_grouping(bos,monetary.mon_grouping);
		print_freestanding(bos,u8"\n"
			u8"positive_sign\t\"",monetary.positive_sign,u8"\"\n"
			u8"negative_sign\t\"",monetary.negative_sign,u8"\"\n"
			u8"int_frac_digits\t",monetary.int_frac_digits,u8"\n"
			u8"frac_digits\t",monetary.frac_digits,u8"\n"
			u8"p_cs_precedes\t",monetary.p_cs_precedes,u8"\n"
			u8"p_sep_by_space\t",monetary.p_sep_by_space,u8"\n"
			u8"n_cs_precedes\t",monetary.n_cs_precedes,u8"\n"
			u8"n_sep_by_space\t",monetary.n_sep_by_space,u8"\n"
			u8"int_p_cs_precedes\t",monetary.int_p_cs_precedes,u8"\n"
			u8"int_p_sep_by_space\t",monetary.int_p_sep_by_space,u8"\n"
			u8"int_n_cs_precedes\t",monetary.int_n_cs_precedes,u8"\n"
			u8"int_n_sep_by_space\t",monetary.int_n_sep_by_space,u8"\n"
			u8"p_sign_posn\t",monetary.p_sign_posn,u8"\n"
			u8"n_sign_posn\t",monetary.n_sign_posn,u8"\n"
			u8"int_p_sign_posn\t",monetary.int_p_sign_posn,u8"\n"
			u8"int_n_sign_posn\t",monetary.int_n_sign_posn,u8"\n"
			u8"END LC_MONETARY");
	}
}
template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_numeric(output bos,basic_lc_numeric<char_type> const& numeric)
{
	if constexpr(std::same_as<char_type,char>)
	{
		print_freestanding(bos,"LC_NUMERIC\n"
			"decimal_point\t\"",numeric.decimal_point,"\"\n"
			"thousands_sep\t\"",numeric.thousands_sep,"\"\n"
			"grouping\t");
		::fast_io::details::print_grouping(bos,numeric.grouping);
		print_freestanding(bos,"\n"
			"END LC_NUMERIC");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print_freestanding(bos,L"LC_NUMERIC\n"
			L"decimal_point\t\"",numeric.decimal_point,L"\"\n"
			L"thousands_sep\t\"",numeric.thousands_sep,L"\"\n"
			L"grouping\t");
		::fast_io::details::print_grouping(bos,numeric.grouping);
		print_freestanding(bos,L"\n"
			L"END LC_NUMERIC");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print_freestanding(bos,u"LC_NUMERIC\n"
			u"decimal_point\t\"",numeric.decimal_point,u"\"\n"
			u"thousands_sep\t\"",numeric.thousands_sep,u"\"\n"
			u"grouping\t");
		::fast_io::details::print_grouping(bos,numeric.grouping);
		print_freestanding(bos,u"\n"
			u"END LC_NUMERIC");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print_freestanding(bos,U"LC_NUMERIC\n"
			U"decimal_point\t\"",numeric.decimal_point,U"\"\n"
			U"thousands_sep\t\"",numeric.thousands_sep,U"\"\n"
			U"grouping\t");
		::fast_io::details::print_grouping(bos,numeric.grouping);
		print_freestanding(bos,U"\n"
			U"END LC_NUMERIC");
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		print_freestanding(bos,u8"LC_NUMERIC\n"
			u8"decimal_point\t\"",numeric.decimal_point,u8"\"\n"
			u8"thousands_sep\t\"",numeric.thousands_sep,u8"\"\n"
			u8"grouping\t");
		::fast_io::details::print_grouping(bos,numeric.grouping);
		print_freestanding(bos,u8"\n"
			u8"END LC_NUMERIC");
	}
}
}

namespace details
{
template<buffer_output_stream output>
constexpr void print_loc_days_real_impl(output bos,typename output::char_type const* category_name,std::size_t category_name_len,basic_io_scatter_t<typename output::char_type> const* day_strings,std::size_t day_strings_len)
{
	using char_type = typename output::char_type;
	if(day_strings_len==0||day_strings_len==0)
		return;
	print_freestanding(bos,basic_io_scatter_t<char_type>{category_name,category_name_len});
	put(bos,char_literal_v<u8'\t',char_type>);
	for(std::size_t i{};i!=day_strings_len;++i)
	{
		if(i)
			put(bos,char_literal_v<u8';',char_type>);
		put(bos,char_literal_v<u8'\"',char_type>);
		print_freestanding(bos,day_strings[i]);
		put(bos,char_literal_v<u8'\"',char_type>);
	}
	put(bos,char_literal_v<u8'\n',char_type>);
}

template<std::size_t n,buffer_output_stream output>
constexpr void print_loc_days_impl(output bos,typename output::char_type const (&category_name)[n],basic_io_scatter_t<basic_io_scatter_t<typename output::char_type>> day_strings)
{
	print_loc_days_real_impl(bos,category_name,n-1,day_strings.base,day_strings.len);
}

template<buffer_output_stream output>
constexpr void print_loc_era_impl(output bos,basic_lc_time_era<typename output::char_type> const* eras_ptr,std::size_t n)
{
	using char_type = typename output::char_type;
	if(n==0)
		return;
	if constexpr(std::same_as<char_type,char>)
	{
		print(bos,"era\t");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print(bos,L"era\t");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print(bos,u"era\t");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print(bos,U"era\t");
	}
	else
	{
		print(bos,u8"era\t");
	}
	for(std::size_t i{};i!=n;++i)
	{
		if(i)
			put(bos,char_literal_v<u8';',char_type>);
		put(bos,char_literal_v<u8'\"',char_type>);
		print_freestanding(bos,eras_ptr[i].era);
		put(bos,char_literal_v<u8'\"',char_type>);
	}
	put(bos,char_literal_v<u8'\n',char_type>);
}

template<buffer_output_stream output,std::integral ch_type1,std::size_t n1,std::integral ch_type2,std::size_t n2>
constexpr void print_loc_days_impl_const(output bos,ch_type1 const (&category_name)[n1],basic_io_scatter_t<ch_type2> const (&day_strings)[n2])
{
	print_loc_days_real_impl(bos,category_name,n1-1,day_strings,n2);
}

template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_time(output bos,basic_lc_time<char_type> const& time)
{
	if constexpr(std::same_as<char,char_type>)
	{
		print_freestanding(bos,"LC_TIME\n");
		::fast_io::details::print_loc_days_impl_const(bos,"abday",time.abday);
		::fast_io::details::print_loc_days_impl_const(bos,"day",time.day);
		::fast_io::details::print_loc_days_impl_const(bos,"abmon",time.abmon);
		::fast_io::details::print_loc_days_impl_const(bos,"ab_alt_mon",time.ab_alt_mon);
		::fast_io::details::print_loc_days_impl_const(bos,"mon",time.mon);
		print_freestanding(bos,
		"d_t_fmt\t\"",time.d_t_fmt,"\"\n",
		"d_fmt\t\"",time.d_fmt,"\"\n",
		"t_fmt\t\"",time.t_fmt,"\"\n",
		"t_fmt_ampm\t\"",time.t_fmt_ampm,"\"\n",
		"date_fmt\t\"",time.date_fmt,"\"\n",
		"am_pm\t\"",time.am_pm[0],"\";\"",time.am_pm[1],"\"\n");
		::fast_io::details::print_loc_era_impl(bos,time.era.base,time.era.len);
		print_freestanding(bos,"era_d_fmt\t\"",time.era_d_fmt,"\"\n",
		"era_d_t_fmt\t\"",time.era_d_t_fmt,"\"\n",
		"era_t_fmt\t\"",time.era_t_fmt,"\"\n");
		::fast_io::details::print_loc_days_impl(bos,"alt_digits",time.alt_digits);
		println_freestanding(bos,"week\t",time.week.ndays,";",time.week.first_day,";",time.week.first_week,"\n",
		"first_weekday\t",time.first_weekday,"\n"
		"first_workday\t",time.first_workday,"\n"
		"cal_direction\t",time.cal_direction);
		::fast_io::details::print_loc_days_impl(bos,"timezone",time.timezone);
		print_freestanding(bos,"END LC_TIME");
	}
	else if constexpr(std::same_as<wchar_t,char_type>)
	{
		print_freestanding(bos,L"LC_TIME\n");
		::fast_io::details::print_loc_days_impl_const(bos,L"abday",time.abday);
		::fast_io::details::print_loc_days_impl_const(bos,L"day",time.day);
		::fast_io::details::print_loc_days_impl_const(bos,L"abmon",time.abmon);
		::fast_io::details::print_loc_days_impl_const(bos,L"ab_alt_mon",time.ab_alt_mon);
		::fast_io::details::print_loc_days_impl_const(bos,L"mon",time.mon);
		print_freestanding(bos,
		L"d_t_fmt\t\"",time.d_t_fmt,L"\"\n",
		L"d_fmt\t\"",time.d_fmt,L"\"\n",
		L"t_fmt\t\"",time.t_fmt,L"\"\n",
		L"t_fmt_ampm\t\"",time.t_fmt_ampm,L"\"\n",
		L"date_fmt\t\"",time.date_fmt,L"\"\n",
		L"am_pm\t\"",time.am_pm[0],L"\";\"",time.am_pm[1],L"\"\n");
		::fast_io::details::print_loc_era_impl(bos,time.era.base,time.era.len);
		print_freestanding(bos,L"era_d_fmt\t\"",time.era_d_fmt,L"\"\n",
		L"era_d_t_fmt\t\"",time.era_d_t_fmt,L"\"\n",
		L"era_t_fmt\t\"",time.era_t_fmt,L"\"\n");
		::fast_io::details::print_loc_days_impl(bos,L"alt_digits",time.alt_digits);
		println_freestanding(bos,L"week\t",time.week.ndays,L";",time.week.first_day,L";",time.week.first_week,L"\n",
		L"first_weekday\t",time.first_weekday,L"\n"
		L"first_workday\t",time.first_workday,L"\n"
		L"cal_direction\t",time.cal_direction);
		::fast_io::details::print_loc_days_impl(bos,L"timezone",time.timezone);
		print_freestanding(bos,L"END LC_TIME");
	}
	else if constexpr(std::same_as<char16_t,char_type>)
	{
		print_freestanding(bos,u"LC_TIME\n");
		::fast_io::details::print_loc_days_impl_const(bos,u"abday",time.abday);
		::fast_io::details::print_loc_days_impl_const(bos,u"day",time.day);
		::fast_io::details::print_loc_days_impl_const(bos,u"abmon",time.abmon);
		::fast_io::details::print_loc_days_impl_const(bos,u"ab_alt_mon",time.ab_alt_mon);
		::fast_io::details::print_loc_days_impl_const(bos,u"mon",time.mon);
		print_freestanding(bos,
		u"d_t_fmt\t\"",time.d_t_fmt,u"\"\n",
		u"d_fmt\t\"",time.d_fmt,u"\"\n",
		u"t_fmt\t\"",time.t_fmt,u"\"\n",
		u"t_fmt_ampm\t\"",time.t_fmt_ampm,u"\"\n",
		u"date_fmt\t\"",time.date_fmt,u"\"\n",
		u"am_pm\t\"",time.am_pm[0],u"\";\"",time.am_pm[1],u"\"\n");
		::fast_io::details::print_loc_era_impl(bos,time.era.base,time.era.len);
		print_freestanding(bos,u"era_d_fmt\t\"",time.era_d_fmt,u"\"\n",
		u"era_d_t_fmt\t\"",time.era_d_t_fmt,u"\"\n",
		u"era_t_fmt\t\"",time.era_t_fmt,u"\"\n");
		::fast_io::details::print_loc_days_impl(bos,u"alt_digits",time.alt_digits);
		println_freestanding(bos,u"week\t",time.week.ndays,u";",time.week.first_day,u";",time.week.first_week,u"\n",
		u"first_weekday\t",time.first_weekday,u"\n"
		u"first_workday\t",time.first_workday,u"\n"
		u"cal_direction\t",time.cal_direction);
		::fast_io::details::print_loc_days_impl(bos,u"timezone",time.timezone);
		print_freestanding(bos,u"END LC_TIME");
	}
	else if constexpr(std::same_as<char32_t,char_type>)
	{
		print_freestanding(bos,U"LC_TIME\n");
		::fast_io::details::print_loc_days_impl_const(bos,U"abday",time.abday);
		::fast_io::details::print_loc_days_impl_const(bos,U"day",time.day);
		::fast_io::details::print_loc_days_impl_const(bos,U"abmon",time.abmon);
		::fast_io::details::print_loc_days_impl_const(bos,U"ab_alt_mon",time.ab_alt_mon);
		::fast_io::details::print_loc_days_impl_const(bos,U"mon",time.mon);
		print_freestanding(bos,
		U"d_t_fmt\t\"",time.d_t_fmt,U"\"\n",
		U"d_fmt\t\"",time.d_fmt,U"\"\n",
		U"t_fmt\t\"",time.t_fmt,U"\"\n",
		U"t_fmt_ampm\t\"",time.t_fmt_ampm,U"\"\n",
		U"date_fmt\t\"",time.date_fmt,U"\"\n",
		U"am_pm\t\"",time.am_pm[0],U"\";\"",time.am_pm[1],U"\"\n");
		::fast_io::details::print_loc_era_impl(bos,time.era.base,time.era.len);
		print_freestanding(bos,U"era_d_fmt\t\"",time.era_d_fmt,U"\"\n",
		U"era_d_t_fmt\t\"",time.era_d_t_fmt,U"\"\n",
		U"era_t_fmt\t\"",time.era_t_fmt,U"\"\n");
		::fast_io::details::print_loc_days_impl(bos,U"alt_digits",time.alt_digits);
		println_freestanding(bos,U"week\t",time.week.ndays,U";",time.week.first_day,U";",time.week.first_week,U"\n",
		U"first_weekday\t",time.first_weekday,U"\n"
		U"first_workday\t",time.first_workday,U"\n"
		U"cal_direction\t",time.cal_direction);
		::fast_io::details::print_loc_days_impl(bos,U"timezone",time.timezone);
		print_freestanding(bos,U"END LC_TIME");
	}
	else if constexpr(std::same_as<char8_t,char_type>)
	{
		print_freestanding(bos,u8"LC_TIME\n");
		::fast_io::details::print_loc_days_impl_const(bos,u8"abday",time.abday);
		::fast_io::details::print_loc_days_impl_const(bos,u8"day",time.day);
		::fast_io::details::print_loc_days_impl_const(bos,u8"abmon",time.abmon);
		::fast_io::details::print_loc_days_impl_const(bos,u8"ab_alt_mon",time.ab_alt_mon);
		::fast_io::details::print_loc_days_impl_const(bos,u8"mon",time.mon);
		print_freestanding(bos,
		u8"d_t_fmt\t\"",time.d_t_fmt,u8"\"\n",
		u8"d_fmt\t\"",time.d_fmt,u8"\"\n",
		u8"t_fmt\t\"",time.t_fmt,u8"\"\n",
		u8"t_fmt_ampm\t\"",time.t_fmt_ampm,u8"\"\n",
		u8"date_fmt\t\"",time.date_fmt,u8"\"\n",
		u8"am_pm\t\"",time.am_pm[0],u8"\";\"",time.am_pm[1],u8"\"\n");
		::fast_io::details::print_loc_era_impl(bos,time.era.base,time.era.len);
		print_freestanding(bos,u8"era_d_fmt\t\"",time.era_d_fmt,u8"\"\n",
		u8"era_d_t_fmt\t\"",time.era_d_t_fmt,u8"\"\n",
		u8"era_t_fmt\t\"",time.era_t_fmt,u8"\"\n");
		::fast_io::details::print_loc_days_impl(bos,u8"alt_digits",time.alt_digits);
		println_freestanding(bos,u8"week\t",time.week.ndays,u8";",time.week.first_day,u8";",time.week.first_week,u8"\n",
		u8"first_weekday\t",time.first_weekday,u8"\n"
		u8"first_workday\t",time.first_workday,u8"\n"
		u8"cal_direction\t",time.cal_direction);
		::fast_io::details::print_loc_days_impl(bos,u8"timezone",time.timezone);
		print_freestanding(bos,u8"END LC_TIME");
	}
}

template<buffer_output_stream output>
constexpr void print_loc_keyboards_impl(output bos,basic_io_scatter_t<typename output::char_type> const* keyboards_strings,std::size_t keyboards_strings_len)
{
	for(std::size_t i{};i!=keyboards_strings_len;++i)
	{
		if(i)
			put(bos,char_literal_v<u8';',typename output::char_type>);
		put(bos,char_literal_v<u8'\"',typename output::char_type>);
		print(bos,keyboards_strings[i]);
		put(bos,char_literal_v<u8'\"',typename output::char_type>);
	}
}

template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_messages(output bos,basic_lc_messages<char_type> const& messages)
{
	if constexpr(std::same_as<char_type,char>)
	{
		print_freestanding(bos,"LC_MESSAGES\n"
			"yesexpr\t\"",messages.yesexpr,"\"\n"
			"noexpr\t\"",messages.noexpr,"\"\n"
			"yesstr\t\"",messages.yesstr,"\"\n"
			"nostr\t\"",messages.nostr,"\"\n"
			"END LC_MESSAGES");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print_freestanding(bos,L"LC_MESSAGES\n"
			L"yesexpr\t\"",messages.yesexpr,L"\"\n"
			L"noexpr\t\"",messages.noexpr,L"\"\n"
			L"yesstr\t\"",messages.yesstr,L"\"\n"
			L"nostr\t\"",messages.nostr,L"\"\n"
			L"END LC_MESSAGES");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print_freestanding(bos,u"LC_MESSAGES\n"
			u"yesexpr\t\"",messages.yesexpr,u"\"\n"
			u"noexpr\t\"",messages.noexpr,u"\"\n"
			u"yesstr\t\"",messages.yesstr,u"\"\n"
			u"nostr\t\"",messages.nostr,u"\"\n"
			u"END LC_MESSAGES");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print_freestanding(bos,U"LC_MESSAGES\n"
			U"yesexpr\t\"",messages.yesexpr,U"\"\n"
			U"noexpr\t\"",messages.noexpr,U"\"\n"
			U"yesstr\t\"",messages.yesstr,U"\"\n"
			U"nostr\t\"",messages.nostr,U"\"\n"
			U"END LC_MESSAGES");
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		print_freestanding(bos,u8"LC_MESSAGES\n"
			u8"yesexpr\t\"",messages.yesexpr,u8"\"\n"
			u8"noexpr\t\"",messages.noexpr,u8"\"\n"
			u8"yesstr\t\"",messages.yesstr,u8"\"\n"
			u8"nostr\t\"",messages.nostr,u8"\"\n"
			u8"END LC_MESSAGES");
	}
}
template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_paper(output bos,basic_lc_paper<char_type> const& paper)
{
	if constexpr(std::same_as<char_type,char>)
	{
		print_freestanding(bos,"LC_PAPER\n"
			"width\t",paper.width,"\n"
			"height\t",paper.height,"\n"
			"END LC_PAPER");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print_freestanding(bos,L"LC_PAPER\n"
			L"width\t",paper.width,L"\n"
			L"height\t",paper.height,L"\n"
			L"END LC_PAPER");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print_freestanding(bos,u"LC_PAPER\n"
			u"width\t",paper.width,u"\n"
			u"height\t",paper.height,u"\n"
			u"END LC_PAPER");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print_freestanding(bos,U"LC_PAPER\n"
			U"width\t",paper.width,U"\n"
			U"height\t",paper.height,U"\n"
			U"END LC_PAPER");
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		print_freestanding(bos,u8"LC_PAPER\n"
			u8"width\t",paper.width,u8"\n"
			u8"height\t",paper.height,u8"\n"
			u8"END LC_PAPER");
	}
}

template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_telephone(output bos,basic_lc_telephone<char_type> const& telephone)
{
	if constexpr(std::same_as<char_type,char>)
	{
		print_freestanding(bos,"LC_TELEPHONE\n"
			"tel_int_fmt\t\"",telephone.tel_int_fmt,"\"\n"
			"tel_dom_fmt\t\"",telephone.tel_dom_fmt,"\"\n"
			"int_select\t\"",telephone.int_select,"\"\n"
			"int_prefix\t\"",telephone.int_prefix,"\"\n"
			"END LC_TELEPHONE");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print_freestanding(bos,L"LC_TELEPHONE\n"
			L"tel_int_fmt\t\"",telephone.tel_int_fmt,L"\"\n"
			L"tel_dom_fmt\t\"",telephone.tel_dom_fmt,L"\"\n"
			L"int_select\t\"",telephone.int_select,L"\"\n"
			L"int_prefix\t\"",telephone.int_prefix,L"\"\n"
			L"END LC_TELEPHONE");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print_freestanding(bos,u"LC_TELEPHONE\n"
			u"tel_int_fmt\t\"",telephone.tel_int_fmt,u"\"\n"
			u"tel_dom_fmt\t\"",telephone.tel_dom_fmt,u"\"\n"
			u"int_select\t\"",telephone.int_select,u"\"\n"
			u"int_prefix\t\"",telephone.int_prefix,u"\"\n"
			u"END LC_TELEPHONE");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print_freestanding(bos,U"LC_TELEPHONE\n"
			U"tel_int_fmt\t\"",telephone.tel_int_fmt,U"\"\n"
			U"tel_dom_fmt\t\"",telephone.tel_dom_fmt,U"\"\n"
			U"int_select\t\"",telephone.int_select,U"\"\n"
			U"int_prefix\t\"",telephone.int_prefix,U"\"\n"
			U"END LC_TELEPHONE");
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		print_freestanding(bos,u8"LC_TELEPHONE\n"
			u8"tel_int_fmt\t\"",telephone.tel_int_fmt,u8"\"\n"
			u8"tel_dom_fmt\t\"",telephone.tel_dom_fmt,u8"\"\n"
			u8"int_select\t\"",telephone.int_select,u8"\"\n"
			u8"int_prefix\t\"",telephone.int_prefix,u8"\"\n"
			u8"END LC_TELEPHONE");
	}
}
template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_name(output bos,basic_lc_name<char_type> const& name)
{
	if constexpr(std::same_as<char_type,char>)
	{
		print_freestanding(bos,"LC_NAME\n"
			"name_fmt\t\"",name.name_fmt,"\"\n"
			"name_gen\t\"",name.name_gen,"\"\n"
			"name_miss\t\"",name.name_miss,"\"\n"
			"name_mr\t\"",name.name_mr,"\"\n"
			"name_mrs\t\"",name.name_mrs,"\"\n"
			"name_ms\t\"",name.name_ms,"\"\n"
			"END LC_NAME");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print_freestanding(bos,L"LC_NAME\n"
			L"name_fmt\t\"",name.name_fmt,L"\"\n"
			L"name_gen\t\"",name.name_gen,L"\"\n"
			L"name_miss\t\"",name.name_miss,L"\"\n"
			L"name_mr\t\"",name.name_mr,L"\"\n"
			L"name_mrs\t\"",name.name_mrs,L"\"\n"
			L"name_ms\t\"",name.name_ms,L"\"\n"
			L"END LC_NAME");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print_freestanding(bos,u"LC_NAME\n"
			u"name_fmt\t\"",name.name_fmt,u"\"\n"
			u"name_gen\t\"",name.name_gen,u"\"\n"
			u"name_miss\t\"",name.name_miss,u"\"\n"
			u"name_mr\t\"",name.name_mr,u"\"\n"
			u"name_mrs\t\"",name.name_mrs,u"\"\n"
			u"name_ms\t\"",name.name_ms,u"\"\n"
			u"END LC_NAME");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print_freestanding(bos,U"LC_NAME\n"
			U"name_fmt\t\"",name.name_fmt,U"\"\n"
			U"name_gen\t\"",name.name_gen,U"\"\n"
			U"name_miss\t\"",name.name_miss,U"\"\n"
			U"name_mr\t\"",name.name_mr,U"\"\n"
			U"name_mrs\t\"",name.name_mrs,U"\"\n"
			U"name_ms\t\"",name.name_ms,U"\"\n"
			U"END LC_NAME");
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		print_freestanding(bos,u8"LC_NAME\n"
			u8"name_fmt\t\"",name.name_fmt,u8"\"\n"
			u8"name_gen\t\"",name.name_gen,u8"\"\n"
			u8"name_miss\t\"",name.name_miss,u8"\"\n"
			u8"name_mr\t\"",name.name_mr,u8"\"\n"
			u8"name_mrs\t\"",name.name_mrs,u8"\"\n"
			u8"name_ms\t\"",name.name_ms,u8"\"\n"
			u8"END LC_NAME");
	}
}
template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_measurement(output bos,basic_lc_measurement<char_type> measurement)
{
	if constexpr(std::same_as<char_type,char>)
	{
		print_freestanding(bos,"LC_MEASUREMENT\n"
			"measurement\t",measurement.measurement,"\n"
			"END LC_MEASUREMENT");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print_freestanding(bos,L"LC_MEASUREMENT\n"
			L"measurement\t",measurement.measurement,L"\n"
			L"END LC_MEASUREMENT");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print_freestanding(bos,u"LC_MEASUREMENT\n"
			u"measurement\t",measurement.measurement,u"\n"
			u"END LC_MEASUREMENT");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print_freestanding(bos,U"LC_MEASUREMENT\n"
			U"measurement\t",measurement.measurement,U"\n"
			U"END LC_MEASUREMENT");
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		print_freestanding(bos,u8"LC_MEASUREMENT\n"
			u8"measurement\t",measurement.measurement,u8"\n"
			u8"END LC_MEASUREMENT");
	}
}
template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_keyboard(output bos,basic_lc_keyboard<char_type> const& keyboard)
{
	if constexpr(std::same_as<char_type,char>)
	{
		print_freestanding(bos,"LC_KEYBOARD\nkeyboards\t");
		::fast_io::details::print_loc_keyboards_impl(bos,keyboard.keyboards.base,keyboard.keyboards.len);
		print_freestanding(bos,"\nEND LC_KEYBOARD");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		print_freestanding(bos,L"LC_KEYBOARD\nkeyboards\t");
		::fast_io::details::print_loc_keyboards_impl(bos,keyboard.keyboards.base,keyboard.keyboards.len);
		print_freestanding(bos,L"\nEND LC_KEYBOARD");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		print_freestanding(bos,u"LC_KEYBOARD\nkeyboards\t");
		::fast_io::details::print_loc_keyboards_impl(bos,keyboard.keyboards.base,keyboard.keyboards.len);
		print_freestanding(bos,u"\nEND LC_KEYBOARD");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		print_freestanding(bos,U"LC_KEYBOARD\nkeyboards\t");
		::fast_io::details::print_loc_keyboards_impl(bos,keyboard.keyboards.base,keyboard.keyboards.len);
		print_freestanding(bos,U"\nEND LC_KEYBOARD");
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		print_freestanding(bos,u8"LC_KEYBOARD\nkeyboards\t");
		::fast_io::details::print_loc_keyboards_impl(bos,keyboard.keyboards.base,keyboard.keyboards.len);
		print_freestanding(bos,u8"\nEND LC_KEYBOARD");
	}
}

template<std::integral char_type>
inline constexpr char_type const* lc_all_create_nn() noexcept
{
	if constexpr(std::same_as<char_type,char>)
		return "\n\n";
	else if constexpr(std::same_as<char_type,wchar_t>)
		return L"\n\n";
	else if constexpr(std::same_as<char_type,char16_t>)
		return u"\n\n";
	else if constexpr(std::same_as<char_type,char32_t>)
		return U"\n\n";
	else
		return u8"\n\n";
}

template<typename output,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
constexpr void print_define_impl_lc_all(output bos,basic_lc_all<char_type> const& all)
{
	constexpr char_type const* first{lc_all_create_nn<char_type>()};
	constexpr char_type const* last{first+2};
	print_define_impl_lc_identification(bos,all.identification);
	write(bos,first,last);
	print_define_impl_lc_monetary(bos,all.monetary);
	write(bos,first,last);
	print_define_impl_lc_time(bos,all.time);
	write(bos,first,last);
	print_define_impl_lc_messages(bos,all.messages);
	write(bos,first,last);
	print_define_impl_lc_paper(bos,all.paper);
	write(bos,first,last);
	print_define_impl_lc_telephone(bos,all.telephone);
	write(bos,first,last);
	print_define_impl_lc_name(bos,all.name);
	write(bos,first,last);
	print_define_impl_lc_measurement(bos,all.measurement);
	write(bos,first,last);
	print_define_impl_lc_keyboard(bos,all.keyboard);
}

}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_identification<char_type>>,output bos,basic_lc_identification<char_type> const& identification)
{
	::fast_io::details::print_define_impl_lc_identification(bos,identification);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_monetary<char_type>>,output bos,basic_lc_monetary<char_type> const& monetary)
{
	::fast_io::details::print_define_impl_lc_monetary(bos,monetary);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_numeric<char_type>>,output bos,basic_lc_numeric<char_type> const& numeric)
{
	::fast_io::details::print_define_impl_lc_numeric(bos,numeric);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_time<char_type>>,output bos,basic_lc_time<char_type> const& time)
{
	::fast_io::details::print_define_impl_lc_time(bos,time);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_messages<char_type>>,output bos,basic_lc_messages<char_type> const& messages)
{
	::fast_io::details::print_define_impl_lc_messages(bos,messages);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_paper<char_type>>,output bos,basic_lc_paper<char_type> const& paper)
{
	::fast_io::details::print_define_impl_lc_paper(bos,paper);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_telephone<char_type>>,output bos,basic_lc_telephone<char_type> const& telephone)
{
	::fast_io::details::print_define_impl_lc_telephone(bos,telephone);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_name<char_type>>,output bos,basic_lc_name<char_type> const& name)
{
	::fast_io::details::print_define_impl_lc_name(bos,name);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_measurement<char_type>>,output bos,basic_lc_measurement<char_type> measurement)
{
	::fast_io::details::print_define_impl_lc_measurement(bos,measurement);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_keyboard<char_type>>,output bos,basic_lc_keyboard<char_type> keyboard)
{
	::fast_io::details::print_define_impl_lc_keyboard(bos,keyboard);
}

template<buffer_output_stream output,std::integral char_type>
requires (std::same_as<typename output::char_type,char_type>&&::std::is_trivially_copyable_v<output>)
constexpr void print_define(io_reserve_type_t<char_type,basic_lc_all<char_type>>,output bos,basic_lc_all<char_type> const& all)
{
	::fast_io::details::print_define_impl_lc_all(bos,all);
}

template<std::integral char_type>
constexpr ::fast_io::parameter<basic_lc_all<char_type> const&> status_io_print_forward(io_alias_type_t<char_type>,lc_locale const& ln) noexcept
{
	return {*get_all<char_type>(ln)};
}

}
