﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("fy_DE"),.title=tsc("Sater Frisian and North Frisian Locale for Germany"),.source=tsc("information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc("kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(""),.language=tsc("Western Frisian"),.territory=tsc("Germany"),.revision=tsc("0.1"),.date=tsc("2003-11-30")},.monetary={.int_curr_symbol=tsc("EUR "),.currency_symbol=tsc("€"),.mon_decimal_point=tsc(","),.mon_thousands_sep=tsc("."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(","),.thousands_sep=tsc("."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc("Sdg"),tsc("Mdg"),tsc("Dsg"),tsc("Mwk"),tsc("Ddg"),tsc("Fdg"),tsc("Swd")},.day={tsc("Sinndag"),tsc("Mondag"),tsc("Dingsdag"),tsc("Meddwäakj"),tsc("Donnadag"),tsc("Friedag"),tsc("Sinnowend")},.abmon={tsc("Jan"),tsc("Feb"),tsc("Moz"),tsc("Apr"),tsc("Mai"),tsc("Jun"),tsc("Jul"),tsc("Aug"),tsc("Sep"),tsc("Okt"),tsc("Now"),tsc("Dez")},.mon={tsc("Jaunuwoa"),tsc("Februwoa"),tsc("Moaz"),tsc("Aprell"),tsc("Mai"),tsc("Juni"),tsc("Juli"),tsc("August"),tsc("Septamba"),tsc("Oktoba"),tsc("Nowamba"),tsc("Dezamba")},.d_t_fmt=tsc("%a %d. %b %Y %T"),.d_fmt=tsc("%d.%m.%Y"),.t_fmt=tsc("%T"),.t_fmt_ampm=tsc(""),.date_fmt=tsc("%a %d. %b %Y %T %Z"),.am_pm={tsc(""),tsc("")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc("^[+1jJyY]"),.noexpr=tsc("^[-0nN]"),.yesstr=tsc("ja"),.nostr=tsc("nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c %a %l"),.tel_dom_fmt=tsc("%A %l"),.int_select=tsc("00"),.int_prefix=tsc("49")},.name={.name_fmt=tsc("%d%t%g%t%m%t%f"),.name_miss=tsc("Fräulein"),.name_mr=tsc("Herr"),.name_mrs=tsc("Frau"),.name_ms=tsc("Frau")},.address={.postal_fmt=tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc("Dútslân"),.country_post=tsc("D"),.country_ab2=tsc("DE"),.country_ab3=tsc("DEU"),.country_num=276,.country_car=tsc("D"),.country_isbn=tsc("3"),.lang_name=tsc("Frysk"),.lang_ab=tsc("fy"),.lang_term=tsc("fry"),.lang_lib=tsc("fry")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"fy_DE"),.title=tsc(L"Sater Frisian and North Frisian Locale for Germany"),.source=tsc(L"information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc(L"kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(L""),.language=tsc(L"Western Frisian"),.territory=tsc(L"Germany"),.revision=tsc(L"0.1"),.date=tsc(L"2003-11-30")},.monetary={.int_curr_symbol=tsc(L"EUR "),.currency_symbol=tsc(L"€"),.mon_decimal_point=tsc(L","),.mon_thousands_sep=tsc(L"."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(L","),.thousands_sep=tsc(L"."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(L"Sdg"),tsc(L"Mdg"),tsc(L"Dsg"),tsc(L"Mwk"),tsc(L"Ddg"),tsc(L"Fdg"),tsc(L"Swd")},.day={tsc(L"Sinndag"),tsc(L"Mondag"),tsc(L"Dingsdag"),tsc(L"Meddwäakj"),tsc(L"Donnadag"),tsc(L"Friedag"),tsc(L"Sinnowend")},.abmon={tsc(L"Jan"),tsc(L"Feb"),tsc(L"Moz"),tsc(L"Apr"),tsc(L"Mai"),tsc(L"Jun"),tsc(L"Jul"),tsc(L"Aug"),tsc(L"Sep"),tsc(L"Okt"),tsc(L"Now"),tsc(L"Dez")},.mon={tsc(L"Jaunuwoa"),tsc(L"Februwoa"),tsc(L"Moaz"),tsc(L"Aprell"),tsc(L"Mai"),tsc(L"Juni"),tsc(L"Juli"),tsc(L"August"),tsc(L"Septamba"),tsc(L"Oktoba"),tsc(L"Nowamba"),tsc(L"Dezamba")},.d_t_fmt=tsc(L"%a %d. %b %Y %T"),.d_fmt=tsc(L"%d.%m.%Y"),.t_fmt=tsc(L"%T"),.t_fmt_ampm=tsc(L""),.date_fmt=tsc(L"%a %d. %b %Y %T %Z"),.am_pm={tsc(L""),tsc(L"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(L"^[+1jJyY]"),.noexpr=tsc(L"^[-0nN]"),.yesstr=tsc(L"ja"),.nostr=tsc(L"nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c %a %l"),.tel_dom_fmt=tsc(L"%A %l"),.int_select=tsc(L"00"),.int_prefix=tsc(L"49")},.name={.name_fmt=tsc(L"%d%t%g%t%m%t%f"),.name_miss=tsc(L"Fräulein"),.name_mr=tsc(L"Herr"),.name_mrs=tsc(L"Frau"),.name_ms=tsc(L"Frau")},.address={.postal_fmt=tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(L"Dútslân"),.country_post=tsc(L"D"),.country_ab2=tsc(L"DE"),.country_ab3=tsc(L"DEU"),.country_num=276,.country_car=tsc(L"D"),.country_isbn=tsc(L"3"),.lang_name=tsc(L"Frysk"),.lang_ab=tsc(L"fy"),.lang_term=tsc(L"fry"),.lang_lib=tsc(L"fry")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"fy_DE"),.title=tsc(u8"Sater Frisian and North Frisian Locale for Germany"),.source=tsc(u8"information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc(u8"kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(u8""),.language=tsc(u8"Western Frisian"),.territory=tsc(u8"Germany"),.revision=tsc(u8"0.1"),.date=tsc(u8"2003-11-30")},.monetary={.int_curr_symbol=tsc(u8"EUR "),.currency_symbol=tsc(u8"€"),.mon_decimal_point=tsc(u8","),.mon_thousands_sep=tsc(u8"."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u8","),.thousands_sep=tsc(u8"."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u8"Sdg"),tsc(u8"Mdg"),tsc(u8"Dsg"),tsc(u8"Mwk"),tsc(u8"Ddg"),tsc(u8"Fdg"),tsc(u8"Swd")},.day={tsc(u8"Sinndag"),tsc(u8"Mondag"),tsc(u8"Dingsdag"),tsc(u8"Meddwäakj"),tsc(u8"Donnadag"),tsc(u8"Friedag"),tsc(u8"Sinnowend")},.abmon={tsc(u8"Jan"),tsc(u8"Feb"),tsc(u8"Moz"),tsc(u8"Apr"),tsc(u8"Mai"),tsc(u8"Jun"),tsc(u8"Jul"),tsc(u8"Aug"),tsc(u8"Sep"),tsc(u8"Okt"),tsc(u8"Now"),tsc(u8"Dez")},.mon={tsc(u8"Jaunuwoa"),tsc(u8"Februwoa"),tsc(u8"Moaz"),tsc(u8"Aprell"),tsc(u8"Mai"),tsc(u8"Juni"),tsc(u8"Juli"),tsc(u8"August"),tsc(u8"Septamba"),tsc(u8"Oktoba"),tsc(u8"Nowamba"),tsc(u8"Dezamba")},.d_t_fmt=tsc(u8"%a %d. %b %Y %T"),.d_fmt=tsc(u8"%d.%m.%Y"),.t_fmt=tsc(u8"%T"),.t_fmt_ampm=tsc(u8""),.date_fmt=tsc(u8"%a %d. %b %Y %T %Z"),.am_pm={tsc(u8""),tsc(u8"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(u8"^[+1jJyY]"),.noexpr=tsc(u8"^[-0nN]"),.yesstr=tsc(u8"ja"),.nostr=tsc(u8"nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c %a %l"),.tel_dom_fmt=tsc(u8"%A %l"),.int_select=tsc(u8"00"),.int_prefix=tsc(u8"49")},.name={.name_fmt=tsc(u8"%d%t%g%t%m%t%f"),.name_miss=tsc(u8"Fräulein"),.name_mr=tsc(u8"Herr"),.name_mrs=tsc(u8"Frau"),.name_ms=tsc(u8"Frau")},.address={.postal_fmt=tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u8"Dútslân"),.country_post=tsc(u8"D"),.country_ab2=tsc(u8"DE"),.country_ab3=tsc(u8"DEU"),.country_num=276,.country_car=tsc(u8"D"),.country_isbn=tsc(u8"3"),.lang_name=tsc(u8"Frysk"),.lang_ab=tsc(u8"fy"),.lang_term=tsc(u8"fry"),.lang_lib=tsc(u8"fry")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"fy_DE"),.title=tsc(u"Sater Frisian and North Frisian Locale for Germany"),.source=tsc(u"information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc(u"kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(u""),.language=tsc(u"Western Frisian"),.territory=tsc(u"Germany"),.revision=tsc(u"0.1"),.date=tsc(u"2003-11-30")},.monetary={.int_curr_symbol=tsc(u"EUR "),.currency_symbol=tsc(u"€"),.mon_decimal_point=tsc(u","),.mon_thousands_sep=tsc(u"."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u","),.thousands_sep=tsc(u"."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u"Sdg"),tsc(u"Mdg"),tsc(u"Dsg"),tsc(u"Mwk"),tsc(u"Ddg"),tsc(u"Fdg"),tsc(u"Swd")},.day={tsc(u"Sinndag"),tsc(u"Mondag"),tsc(u"Dingsdag"),tsc(u"Meddwäakj"),tsc(u"Donnadag"),tsc(u"Friedag"),tsc(u"Sinnowend")},.abmon={tsc(u"Jan"),tsc(u"Feb"),tsc(u"Moz"),tsc(u"Apr"),tsc(u"Mai"),tsc(u"Jun"),tsc(u"Jul"),tsc(u"Aug"),tsc(u"Sep"),tsc(u"Okt"),tsc(u"Now"),tsc(u"Dez")},.mon={tsc(u"Jaunuwoa"),tsc(u"Februwoa"),tsc(u"Moaz"),tsc(u"Aprell"),tsc(u"Mai"),tsc(u"Juni"),tsc(u"Juli"),tsc(u"August"),tsc(u"Septamba"),tsc(u"Oktoba"),tsc(u"Nowamba"),tsc(u"Dezamba")},.d_t_fmt=tsc(u"%a %d. %b %Y %T"),.d_fmt=tsc(u"%d.%m.%Y"),.t_fmt=tsc(u"%T"),.t_fmt_ampm=tsc(u""),.date_fmt=tsc(u"%a %d. %b %Y %T %Z"),.am_pm={tsc(u""),tsc(u"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(u"^[+1jJyY]"),.noexpr=tsc(u"^[-0nN]"),.yesstr=tsc(u"ja"),.nostr=tsc(u"nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c %a %l"),.tel_dom_fmt=tsc(u"%A %l"),.int_select=tsc(u"00"),.int_prefix=tsc(u"49")},.name={.name_fmt=tsc(u"%d%t%g%t%m%t%f"),.name_miss=tsc(u"Fräulein"),.name_mr=tsc(u"Herr"),.name_mrs=tsc(u"Frau"),.name_ms=tsc(u"Frau")},.address={.postal_fmt=tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u"Dútslân"),.country_post=tsc(u"D"),.country_ab2=tsc(u"DE"),.country_ab3=tsc(u"DEU"),.country_num=276,.country_car=tsc(u"D"),.country_isbn=tsc(u"3"),.lang_name=tsc(u"Frysk"),.lang_ab=tsc(u"fy"),.lang_term=tsc(u"fry"),.lang_lib=tsc(u"fry")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"fy_DE"),.title=tsc(U"Sater Frisian and North Frisian Locale for Germany"),.source=tsc(U"information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc(U"kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(U""),.language=tsc(U"Western Frisian"),.territory=tsc(U"Germany"),.revision=tsc(U"0.1"),.date=tsc(U"2003-11-30")},.monetary={.int_curr_symbol=tsc(U"EUR "),.currency_symbol=tsc(U"€"),.mon_decimal_point=tsc(U","),.mon_thousands_sep=tsc(U"."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(U","),.thousands_sep=tsc(U"."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(U"Sdg"),tsc(U"Mdg"),tsc(U"Dsg"),tsc(U"Mwk"),tsc(U"Ddg"),tsc(U"Fdg"),tsc(U"Swd")},.day={tsc(U"Sinndag"),tsc(U"Mondag"),tsc(U"Dingsdag"),tsc(U"Meddwäakj"),tsc(U"Donnadag"),tsc(U"Friedag"),tsc(U"Sinnowend")},.abmon={tsc(U"Jan"),tsc(U"Feb"),tsc(U"Moz"),tsc(U"Apr"),tsc(U"Mai"),tsc(U"Jun"),tsc(U"Jul"),tsc(U"Aug"),tsc(U"Sep"),tsc(U"Okt"),tsc(U"Now"),tsc(U"Dez")},.mon={tsc(U"Jaunuwoa"),tsc(U"Februwoa"),tsc(U"Moaz"),tsc(U"Aprell"),tsc(U"Mai"),tsc(U"Juni"),tsc(U"Juli"),tsc(U"August"),tsc(U"Septamba"),tsc(U"Oktoba"),tsc(U"Nowamba"),tsc(U"Dezamba")},.d_t_fmt=tsc(U"%a %d. %b %Y %T"),.d_fmt=tsc(U"%d.%m.%Y"),.t_fmt=tsc(U"%T"),.t_fmt_ampm=tsc(U""),.date_fmt=tsc(U"%a %d. %b %Y %T %Z"),.am_pm={tsc(U""),tsc(U"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(U"^[+1jJyY]"),.noexpr=tsc(U"^[-0nN]"),.yesstr=tsc(U"ja"),.nostr=tsc(U"nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c %a %l"),.tel_dom_fmt=tsc(U"%A %l"),.int_select=tsc(U"00"),.int_prefix=tsc(U"49")},.name={.name_fmt=tsc(U"%d%t%g%t%m%t%f"),.name_miss=tsc(U"Fräulein"),.name_mr=tsc(U"Herr"),.name_mrs=tsc(U"Frau"),.name_ms=tsc(U"Frau")},.address={.postal_fmt=tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(U"Dútslân"),.country_post=tsc(U"D"),.country_ab2=tsc(U"DE"),.country_ab3=tsc(U"DEU"),.country_num=276,.country_car=tsc(U"D"),.country_isbn=tsc(U"3"),.lang_name=tsc(U"Frysk"),.lang_ab=tsc(U"fy"),.lang_term=tsc(U"fry"),.lang_lib=tsc(U"fry")},.measurement={.measurement=1}};


}
}

#include"../main.h"