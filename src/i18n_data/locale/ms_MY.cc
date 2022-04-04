﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("ms_MY"),.title=tsc("Malay language locale for Malaysia"),.source=tsc("IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc("1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("fast_io"),.email=tsc("bug-glibc-locales@gnu.org, sebol@ikhlas.com;euloanty@live.com"),.tel=tsc(""),.fax=tsc(""),.language=tsc("Malay"),.territory=tsc("Malaysia"),.revision=tsc("0.92b"),.date=tsc("2001, December, 10")},.monetary={.int_curr_symbol=tsc("MYR "),.currency_symbol=tsc("RM"),.mon_decimal_point=tsc("."),.mon_thousands_sep=tsc(","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc("."),.thousands_sep=tsc(","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc("Ahd"),tsc("Isn"),tsc("Sel"),tsc("Rab"),tsc("Kha"),tsc("Jum"),tsc("Sab")},.day={tsc("Ahad"),tsc("Isnin"),tsc("Selasa"),tsc("Rabu"),tsc("Khamis"),tsc("Jumaat"),tsc("Sabtu")},.abmon={tsc("Jan"),tsc("Feb"),tsc("Mac"),tsc("Apr"),tsc("Mei"),tsc("Jun"),tsc("Jul"),tsc("Ogos"),tsc("Sep"),tsc("Okt"),tsc("Nov"),tsc("Dis")},.mon={tsc("Januari"),tsc("Februari"),tsc("Mac"),tsc("April"),tsc("Mei"),tsc("Jun"),tsc("Julai"),tsc("Ogos"),tsc("September"),tsc("Oktober"),tsc("November"),tsc("Disember")},.d_t_fmt=tsc("%A %d %b %Y %I:%M:%S %p"),.d_fmt=tsc("%A %d %b %Y"),.t_fmt=tsc("%I:%M:%S %p %Z"),.t_fmt_ampm=tsc("%I:%M:%S %p %Z"),.date_fmt=tsc("%A %d %b %Y %I:%M:%S %p %Z"),.am_pm={tsc(""),tsc("")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc("^[+1yY]"),.noexpr=tsc("^[-0tTnN]"),.yesstr=tsc("ya"),.nostr=tsc("tidak")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c ;%a ;%l"),.int_select=tsc("00"),.int_prefix=tsc("60")},.name={.name_fmt=tsc("%p%t%f%t%g"),.name_gen=tsc(""),.name_miss=tsc("cik"),.name_mr=tsc("Eecik"),.name_mrs=tsc("Puan"),.name_ms=tsc("cik//Puan")},.address={.postal_fmt=tsc("%z%c%T%s%b%e%r"),.country_name=tsc("Malaysia"),.country_ab2=tsc("MY"),.country_ab3=tsc("MYS"),.country_num=458,.country_car=tsc("MAL"),.lang_name=tsc("Melayu"),.lang_ab=tsc("ms"),.lang_term=tsc("msa"),.lang_lib=tsc("may")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"ms_MY"),.title=tsc(L"Malay language locale for Malaysia"),.source=tsc(L"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(L"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"fast_io"),.email=tsc(L"bug-glibc-locales@gnu.org, sebol@ikhlas.com;euloanty@live.com"),.tel=tsc(L""),.fax=tsc(L""),.language=tsc(L"Malay"),.territory=tsc(L"Malaysia"),.revision=tsc(L"0.92b"),.date=tsc(L"2001, December, 10")},.monetary={.int_curr_symbol=tsc(L"MYR "),.currency_symbol=tsc(L"RM"),.mon_decimal_point=tsc(L"."),.mon_thousands_sep=tsc(L","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc(L"."),.thousands_sep=tsc(L","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(L"Ahd"),tsc(L"Isn"),tsc(L"Sel"),tsc(L"Rab"),tsc(L"Kha"),tsc(L"Jum"),tsc(L"Sab")},.day={tsc(L"Ahad"),tsc(L"Isnin"),tsc(L"Selasa"),tsc(L"Rabu"),tsc(L"Khamis"),tsc(L"Jumaat"),tsc(L"Sabtu")},.abmon={tsc(L"Jan"),tsc(L"Feb"),tsc(L"Mac"),tsc(L"Apr"),tsc(L"Mei"),tsc(L"Jun"),tsc(L"Jul"),tsc(L"Ogos"),tsc(L"Sep"),tsc(L"Okt"),tsc(L"Nov"),tsc(L"Dis")},.mon={tsc(L"Januari"),tsc(L"Februari"),tsc(L"Mac"),tsc(L"April"),tsc(L"Mei"),tsc(L"Jun"),tsc(L"Julai"),tsc(L"Ogos"),tsc(L"September"),tsc(L"Oktober"),tsc(L"November"),tsc(L"Disember")},.d_t_fmt=tsc(L"%A %d %b %Y %I:%M:%S %p"),.d_fmt=tsc(L"%A %d %b %Y"),.t_fmt=tsc(L"%I:%M:%S %p %Z"),.t_fmt_ampm=tsc(L"%I:%M:%S %p %Z"),.date_fmt=tsc(L"%A %d %b %Y %I:%M:%S %p %Z"),.am_pm={tsc(L""),tsc(L"")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc(L"^[+1yY]"),.noexpr=tsc(L"^[-0tTnN]"),.yesstr=tsc(L"ya"),.nostr=tsc(L"tidak")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c ;%a ;%l"),.int_select=tsc(L"00"),.int_prefix=tsc(L"60")},.name={.name_fmt=tsc(L"%p%t%f%t%g"),.name_gen=tsc(L""),.name_miss=tsc(L"cik"),.name_mr=tsc(L"Eecik"),.name_mrs=tsc(L"Puan"),.name_ms=tsc(L"cik//Puan")},.address={.postal_fmt=tsc(L"%z%c%T%s%b%e%r"),.country_name=tsc(L"Malaysia"),.country_ab2=tsc(L"MY"),.country_ab3=tsc(L"MYS"),.country_num=458,.country_car=tsc(L"MAL"),.lang_name=tsc(L"Melayu"),.lang_ab=tsc(L"ms"),.lang_term=tsc(L"msa"),.lang_lib=tsc(L"may")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"ms_MY"),.title=tsc(u8"Malay language locale for Malaysia"),.source=tsc(u8"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(u8"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"fast_io"),.email=tsc(u8"bug-glibc-locales@gnu.org, sebol@ikhlas.com;euloanty@live.com"),.tel=tsc(u8""),.fax=tsc(u8""),.language=tsc(u8"Malay"),.territory=tsc(u8"Malaysia"),.revision=tsc(u8"0.92b"),.date=tsc(u8"2001, December, 10")},.monetary={.int_curr_symbol=tsc(u8"MYR "),.currency_symbol=tsc(u8"RM"),.mon_decimal_point=tsc(u8"."),.mon_thousands_sep=tsc(u8","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc(u8"."),.thousands_sep=tsc(u8","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u8"Ahd"),tsc(u8"Isn"),tsc(u8"Sel"),tsc(u8"Rab"),tsc(u8"Kha"),tsc(u8"Jum"),tsc(u8"Sab")},.day={tsc(u8"Ahad"),tsc(u8"Isnin"),tsc(u8"Selasa"),tsc(u8"Rabu"),tsc(u8"Khamis"),tsc(u8"Jumaat"),tsc(u8"Sabtu")},.abmon={tsc(u8"Jan"),tsc(u8"Feb"),tsc(u8"Mac"),tsc(u8"Apr"),tsc(u8"Mei"),tsc(u8"Jun"),tsc(u8"Jul"),tsc(u8"Ogos"),tsc(u8"Sep"),tsc(u8"Okt"),tsc(u8"Nov"),tsc(u8"Dis")},.mon={tsc(u8"Januari"),tsc(u8"Februari"),tsc(u8"Mac"),tsc(u8"April"),tsc(u8"Mei"),tsc(u8"Jun"),tsc(u8"Julai"),tsc(u8"Ogos"),tsc(u8"September"),tsc(u8"Oktober"),tsc(u8"November"),tsc(u8"Disember")},.d_t_fmt=tsc(u8"%A %d %b %Y %I:%M:%S %p"),.d_fmt=tsc(u8"%A %d %b %Y"),.t_fmt=tsc(u8"%I:%M:%S %p %Z"),.t_fmt_ampm=tsc(u8"%I:%M:%S %p %Z"),.date_fmt=tsc(u8"%A %d %b %Y %I:%M:%S %p %Z"),.am_pm={tsc(u8""),tsc(u8"")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc(u8"^[+1yY]"),.noexpr=tsc(u8"^[-0tTnN]"),.yesstr=tsc(u8"ya"),.nostr=tsc(u8"tidak")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c ;%a ;%l"),.int_select=tsc(u8"00"),.int_prefix=tsc(u8"60")},.name={.name_fmt=tsc(u8"%p%t%f%t%g"),.name_gen=tsc(u8""),.name_miss=tsc(u8"cik"),.name_mr=tsc(u8"Eecik"),.name_mrs=tsc(u8"Puan"),.name_ms=tsc(u8"cik//Puan")},.address={.postal_fmt=tsc(u8"%z%c%T%s%b%e%r"),.country_name=tsc(u8"Malaysia"),.country_ab2=tsc(u8"MY"),.country_ab3=tsc(u8"MYS"),.country_num=458,.country_car=tsc(u8"MAL"),.lang_name=tsc(u8"Melayu"),.lang_ab=tsc(u8"ms"),.lang_term=tsc(u8"msa"),.lang_lib=tsc(u8"may")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"ms_MY"),.title=tsc(u"Malay language locale for Malaysia"),.source=tsc(u"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(u"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"fast_io"),.email=tsc(u"bug-glibc-locales@gnu.org, sebol@ikhlas.com;euloanty@live.com"),.tel=tsc(u""),.fax=tsc(u""),.language=tsc(u"Malay"),.territory=tsc(u"Malaysia"),.revision=tsc(u"0.92b"),.date=tsc(u"2001, December, 10")},.monetary={.int_curr_symbol=tsc(u"MYR "),.currency_symbol=tsc(u"RM"),.mon_decimal_point=tsc(u"."),.mon_thousands_sep=tsc(u","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc(u"."),.thousands_sep=tsc(u","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u"Ahd"),tsc(u"Isn"),tsc(u"Sel"),tsc(u"Rab"),tsc(u"Kha"),tsc(u"Jum"),tsc(u"Sab")},.day={tsc(u"Ahad"),tsc(u"Isnin"),tsc(u"Selasa"),tsc(u"Rabu"),tsc(u"Khamis"),tsc(u"Jumaat"),tsc(u"Sabtu")},.abmon={tsc(u"Jan"),tsc(u"Feb"),tsc(u"Mac"),tsc(u"Apr"),tsc(u"Mei"),tsc(u"Jun"),tsc(u"Jul"),tsc(u"Ogos"),tsc(u"Sep"),tsc(u"Okt"),tsc(u"Nov"),tsc(u"Dis")},.mon={tsc(u"Januari"),tsc(u"Februari"),tsc(u"Mac"),tsc(u"April"),tsc(u"Mei"),tsc(u"Jun"),tsc(u"Julai"),tsc(u"Ogos"),tsc(u"September"),tsc(u"Oktober"),tsc(u"November"),tsc(u"Disember")},.d_t_fmt=tsc(u"%A %d %b %Y %I:%M:%S %p"),.d_fmt=tsc(u"%A %d %b %Y"),.t_fmt=tsc(u"%I:%M:%S %p %Z"),.t_fmt_ampm=tsc(u"%I:%M:%S %p %Z"),.date_fmt=tsc(u"%A %d %b %Y %I:%M:%S %p %Z"),.am_pm={tsc(u""),tsc(u"")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc(u"^[+1yY]"),.noexpr=tsc(u"^[-0tTnN]"),.yesstr=tsc(u"ya"),.nostr=tsc(u"tidak")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c ;%a ;%l"),.int_select=tsc(u"00"),.int_prefix=tsc(u"60")},.name={.name_fmt=tsc(u"%p%t%f%t%g"),.name_gen=tsc(u""),.name_miss=tsc(u"cik"),.name_mr=tsc(u"Eecik"),.name_mrs=tsc(u"Puan"),.name_ms=tsc(u"cik//Puan")},.address={.postal_fmt=tsc(u"%z%c%T%s%b%e%r"),.country_name=tsc(u"Malaysia"),.country_ab2=tsc(u"MY"),.country_ab3=tsc(u"MYS"),.country_num=458,.country_car=tsc(u"MAL"),.lang_name=tsc(u"Melayu"),.lang_ab=tsc(u"ms"),.lang_term=tsc(u"msa"),.lang_lib=tsc(u"may")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"ms_MY"),.title=tsc(U"Malay language locale for Malaysia"),.source=tsc(U"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(U"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"fast_io"),.email=tsc(U"bug-glibc-locales@gnu.org, sebol@ikhlas.com;euloanty@live.com"),.tel=tsc(U""),.fax=tsc(U""),.language=tsc(U"Malay"),.territory=tsc(U"Malaysia"),.revision=tsc(U"0.92b"),.date=tsc(U"2001, December, 10")},.monetary={.int_curr_symbol=tsc(U"MYR "),.currency_symbol=tsc(U"RM"),.mon_decimal_point=tsc(U"."),.mon_thousands_sep=tsc(U","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=0},.numeric={.decimal_point=tsc(U"."),.thousands_sep=tsc(U","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(U"Ahd"),tsc(U"Isn"),tsc(U"Sel"),tsc(U"Rab"),tsc(U"Kha"),tsc(U"Jum"),tsc(U"Sab")},.day={tsc(U"Ahad"),tsc(U"Isnin"),tsc(U"Selasa"),tsc(U"Rabu"),tsc(U"Khamis"),tsc(U"Jumaat"),tsc(U"Sabtu")},.abmon={tsc(U"Jan"),tsc(U"Feb"),tsc(U"Mac"),tsc(U"Apr"),tsc(U"Mei"),tsc(U"Jun"),tsc(U"Jul"),tsc(U"Ogos"),tsc(U"Sep"),tsc(U"Okt"),tsc(U"Nov"),tsc(U"Dis")},.mon={tsc(U"Januari"),tsc(U"Februari"),tsc(U"Mac"),tsc(U"April"),tsc(U"Mei"),tsc(U"Jun"),tsc(U"Julai"),tsc(U"Ogos"),tsc(U"September"),tsc(U"Oktober"),tsc(U"November"),tsc(U"Disember")},.d_t_fmt=tsc(U"%A %d %b %Y %I:%M:%S %p"),.d_fmt=tsc(U"%A %d %b %Y"),.t_fmt=tsc(U"%I:%M:%S %p %Z"),.t_fmt_ampm=tsc(U"%I:%M:%S %p %Z"),.date_fmt=tsc(U"%A %d %b %Y %I:%M:%S %p %Z"),.am_pm={tsc(U""),tsc(U"")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc(U"^[+1yY]"),.noexpr=tsc(U"^[-0tTnN]"),.yesstr=tsc(U"ya"),.nostr=tsc(U"tidak")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c ;%a ;%l"),.int_select=tsc(U"00"),.int_prefix=tsc(U"60")},.name={.name_fmt=tsc(U"%p%t%f%t%g"),.name_gen=tsc(U""),.name_miss=tsc(U"cik"),.name_mr=tsc(U"Eecik"),.name_mrs=tsc(U"Puan"),.name_ms=tsc(U"cik//Puan")},.address={.postal_fmt=tsc(U"%z%c%T%s%b%e%r"),.country_name=tsc(U"Malaysia"),.country_ab2=tsc(U"MY"),.country_ab3=tsc(U"MYS"),.country_num=458,.country_car=tsc(U"MAL"),.lang_name=tsc(U"Melayu"),.lang_ab=tsc(U"ms"),.lang_term=tsc(U"msa"),.lang_lib=tsc(U"may")},.measurement={.measurement=1}};


}
}

#include"../main.h"