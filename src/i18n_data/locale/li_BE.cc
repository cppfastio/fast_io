﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("li_BE"),.title=tsc("Limburgish Language Locale for Belgium"),.source=tsc("information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc("kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(""),.language=tsc("Limburgish"),.territory=tsc("Belgium"),.revision=tsc("0.1"),.date=tsc("2003-11-30")},.monetary={.int_curr_symbol=tsc("EUR "),.currency_symbol=tsc("€"),.mon_decimal_point=tsc(","),.mon_thousands_sep=tsc("."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=1,.n_cs_precedes=1,.n_sep_by_space=2,.p_sign_posn=1,.n_sign_posn=4},.numeric={.decimal_point=tsc(","),.thousands_sep=tsc("."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc("zón"),tsc("mao"),tsc("dae"),tsc("goo"),tsc("dón"),tsc("vri"),tsc("zao")},.day={tsc("zóndig"),tsc("maondig"),tsc("daensdig"),tsc("goonsdig"),tsc("dónderdig"),tsc("vriedig"),tsc("zaoterdig")},.abmon={tsc("jan"),tsc("fib"),tsc("mie"),tsc("epr"),tsc("mei"),tsc("jun"),tsc("jul"),tsc("aug"),tsc("sep"),tsc("okt"),tsc("nov"),tsc("des")},.mon={tsc("jannewarie"),tsc("fibberwarie"),tsc("miert"),tsc("eprèl"),tsc("meij"),tsc("junie"),tsc("julie"),tsc("augustus"),tsc("september"),tsc("oktober"),tsc("november"),tsc("desember")},.d_t_fmt=tsc("%a %d. %b %Y %T"),.d_fmt=tsc("%d.%m.%Y"),.t_fmt=tsc("%T"),.t_fmt_ampm=tsc(""),.date_fmt=tsc("%a %d. %b %Y %T %Z"),.am_pm={tsc(""),tsc("")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc("^[+1jJyY]"),.noexpr=tsc("^[-0nN]"),.yesstr=tsc("ja"),.nostr=tsc("nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c %a %l"),.int_select=tsc("00"),.int_prefix=tsc("32")},.name={.name_fmt=tsc("%d%t%g%t%m%t%f"),.name_gen=tsc("heer of mevrouw"),.name_miss=tsc("mevrouw"),.name_mr=tsc("heer"),.name_mrs=tsc("mevrouw"),.name_ms=tsc("mevrouw")},.address={.postal_fmt=tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc("Bèlsj"),.country_post=tsc("B"),.country_ab2=tsc("BE"),.country_ab3=tsc("BEL"),.country_num=56,.country_car=tsc("B"),.lang_name=tsc("Lèmbörgs"),.lang_ab=tsc("li"),.lang_term=tsc("lim"),.lang_lib=tsc("lim")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"li_BE"),.title=tsc(L"Limburgish Language Locale for Belgium"),.source=tsc(L"information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc(L"kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(L""),.language=tsc(L"Limburgish"),.territory=tsc(L"Belgium"),.revision=tsc(L"0.1"),.date=tsc(L"2003-11-30")},.monetary={.int_curr_symbol=tsc(L"EUR "),.currency_symbol=tsc(L"€"),.mon_decimal_point=tsc(L","),.mon_thousands_sep=tsc(L"."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=1,.n_cs_precedes=1,.n_sep_by_space=2,.p_sign_posn=1,.n_sign_posn=4},.numeric={.decimal_point=tsc(L","),.thousands_sep=tsc(L"."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(L"zón"),tsc(L"mao"),tsc(L"dae"),tsc(L"goo"),tsc(L"dón"),tsc(L"vri"),tsc(L"zao")},.day={tsc(L"zóndig"),tsc(L"maondig"),tsc(L"daensdig"),tsc(L"goonsdig"),tsc(L"dónderdig"),tsc(L"vriedig"),tsc(L"zaoterdig")},.abmon={tsc(L"jan"),tsc(L"fib"),tsc(L"mie"),tsc(L"epr"),tsc(L"mei"),tsc(L"jun"),tsc(L"jul"),tsc(L"aug"),tsc(L"sep"),tsc(L"okt"),tsc(L"nov"),tsc(L"des")},.mon={tsc(L"jannewarie"),tsc(L"fibberwarie"),tsc(L"miert"),tsc(L"eprèl"),tsc(L"meij"),tsc(L"junie"),tsc(L"julie"),tsc(L"augustus"),tsc(L"september"),tsc(L"oktober"),tsc(L"november"),tsc(L"desember")},.d_t_fmt=tsc(L"%a %d. %b %Y %T"),.d_fmt=tsc(L"%d.%m.%Y"),.t_fmt=tsc(L"%T"),.t_fmt_ampm=tsc(L""),.date_fmt=tsc(L"%a %d. %b %Y %T %Z"),.am_pm={tsc(L""),tsc(L"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(L"^[+1jJyY]"),.noexpr=tsc(L"^[-0nN]"),.yesstr=tsc(L"ja"),.nostr=tsc(L"nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c %a %l"),.int_select=tsc(L"00"),.int_prefix=tsc(L"32")},.name={.name_fmt=tsc(L"%d%t%g%t%m%t%f"),.name_gen=tsc(L"heer of mevrouw"),.name_miss=tsc(L"mevrouw"),.name_mr=tsc(L"heer"),.name_mrs=tsc(L"mevrouw"),.name_ms=tsc(L"mevrouw")},.address={.postal_fmt=tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(L"Bèlsj"),.country_post=tsc(L"B"),.country_ab2=tsc(L"BE"),.country_ab3=tsc(L"BEL"),.country_num=56,.country_car=tsc(L"B"),.lang_name=tsc(L"Lèmbörgs"),.lang_ab=tsc(L"li"),.lang_term=tsc(L"lim"),.lang_lib=tsc(L"lim")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"li_BE"),.title=tsc(u8"Limburgish Language Locale for Belgium"),.source=tsc(u8"information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc(u8"kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(u8""),.language=tsc(u8"Limburgish"),.territory=tsc(u8"Belgium"),.revision=tsc(u8"0.1"),.date=tsc(u8"2003-11-30")},.monetary={.int_curr_symbol=tsc(u8"EUR "),.currency_symbol=tsc(u8"€"),.mon_decimal_point=tsc(u8","),.mon_thousands_sep=tsc(u8"."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=1,.n_cs_precedes=1,.n_sep_by_space=2,.p_sign_posn=1,.n_sign_posn=4},.numeric={.decimal_point=tsc(u8","),.thousands_sep=tsc(u8"."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u8"zón"),tsc(u8"mao"),tsc(u8"dae"),tsc(u8"goo"),tsc(u8"dón"),tsc(u8"vri"),tsc(u8"zao")},.day={tsc(u8"zóndig"),tsc(u8"maondig"),tsc(u8"daensdig"),tsc(u8"goonsdig"),tsc(u8"dónderdig"),tsc(u8"vriedig"),tsc(u8"zaoterdig")},.abmon={tsc(u8"jan"),tsc(u8"fib"),tsc(u8"mie"),tsc(u8"epr"),tsc(u8"mei"),tsc(u8"jun"),tsc(u8"jul"),tsc(u8"aug"),tsc(u8"sep"),tsc(u8"okt"),tsc(u8"nov"),tsc(u8"des")},.mon={tsc(u8"jannewarie"),tsc(u8"fibberwarie"),tsc(u8"miert"),tsc(u8"eprèl"),tsc(u8"meij"),tsc(u8"junie"),tsc(u8"julie"),tsc(u8"augustus"),tsc(u8"september"),tsc(u8"oktober"),tsc(u8"november"),tsc(u8"desember")},.d_t_fmt=tsc(u8"%a %d. %b %Y %T"),.d_fmt=tsc(u8"%d.%m.%Y"),.t_fmt=tsc(u8"%T"),.t_fmt_ampm=tsc(u8""),.date_fmt=tsc(u8"%a %d. %b %Y %T %Z"),.am_pm={tsc(u8""),tsc(u8"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(u8"^[+1jJyY]"),.noexpr=tsc(u8"^[-0nN]"),.yesstr=tsc(u8"ja"),.nostr=tsc(u8"nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c %a %l"),.int_select=tsc(u8"00"),.int_prefix=tsc(u8"32")},.name={.name_fmt=tsc(u8"%d%t%g%t%m%t%f"),.name_gen=tsc(u8"heer of mevrouw"),.name_miss=tsc(u8"mevrouw"),.name_mr=tsc(u8"heer"),.name_mrs=tsc(u8"mevrouw"),.name_ms=tsc(u8"mevrouw")},.address={.postal_fmt=tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u8"Bèlsj"),.country_post=tsc(u8"B"),.country_ab2=tsc(u8"BE"),.country_ab3=tsc(u8"BEL"),.country_num=56,.country_car=tsc(u8"B"),.lang_name=tsc(u8"Lèmbörgs"),.lang_ab=tsc(u8"li"),.lang_term=tsc(u8"lim"),.lang_lib=tsc(u8"lim")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"li_BE"),.title=tsc(u"Limburgish Language Locale for Belgium"),.source=tsc(u"information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc(u"kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(u""),.language=tsc(u"Limburgish"),.territory=tsc(u"Belgium"),.revision=tsc(u"0.1"),.date=tsc(u"2003-11-30")},.monetary={.int_curr_symbol=tsc(u"EUR "),.currency_symbol=tsc(u"€"),.mon_decimal_point=tsc(u","),.mon_thousands_sep=tsc(u"."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=1,.n_cs_precedes=1,.n_sep_by_space=2,.p_sign_posn=1,.n_sign_posn=4},.numeric={.decimal_point=tsc(u","),.thousands_sep=tsc(u"."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u"zón"),tsc(u"mao"),tsc(u"dae"),tsc(u"goo"),tsc(u"dón"),tsc(u"vri"),tsc(u"zao")},.day={tsc(u"zóndig"),tsc(u"maondig"),tsc(u"daensdig"),tsc(u"goonsdig"),tsc(u"dónderdig"),tsc(u"vriedig"),tsc(u"zaoterdig")},.abmon={tsc(u"jan"),tsc(u"fib"),tsc(u"mie"),tsc(u"epr"),tsc(u"mei"),tsc(u"jun"),tsc(u"jul"),tsc(u"aug"),tsc(u"sep"),tsc(u"okt"),tsc(u"nov"),tsc(u"des")},.mon={tsc(u"jannewarie"),tsc(u"fibberwarie"),tsc(u"miert"),tsc(u"eprèl"),tsc(u"meij"),tsc(u"junie"),tsc(u"julie"),tsc(u"augustus"),tsc(u"september"),tsc(u"oktober"),tsc(u"november"),tsc(u"desember")},.d_t_fmt=tsc(u"%a %d. %b %Y %T"),.d_fmt=tsc(u"%d.%m.%Y"),.t_fmt=tsc(u"%T"),.t_fmt_ampm=tsc(u""),.date_fmt=tsc(u"%a %d. %b %Y %T %Z"),.am_pm={tsc(u""),tsc(u"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(u"^[+1jJyY]"),.noexpr=tsc(u"^[-0nN]"),.yesstr=tsc(u"ja"),.nostr=tsc(u"nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c %a %l"),.int_select=tsc(u"00"),.int_prefix=tsc(u"32")},.name={.name_fmt=tsc(u"%d%t%g%t%m%t%f"),.name_gen=tsc(u"heer of mevrouw"),.name_miss=tsc(u"mevrouw"),.name_mr=tsc(u"heer"),.name_mrs=tsc(u"mevrouw"),.name_ms=tsc(u"mevrouw")},.address={.postal_fmt=tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u"Bèlsj"),.country_post=tsc(u"B"),.country_ab2=tsc(u"BE"),.country_ab3=tsc(u"BEL"),.country_num=56,.country_car=tsc(u"B"),.lang_name=tsc(u"Lèmbörgs"),.lang_ab=tsc(u"li"),.lang_term=tsc(u"lim"),.lang_lib=tsc(u"lim")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"li_BE"),.title=tsc(U"Limburgish Language Locale for Belgium"),.source=tsc(U"information from Kenneth Christiansen\t\t;\t\tfast_io"),.address=tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"Kenneth Christiansen, Pablo Saratxaga\t\t;\t\tfast_io"),.email=tsc(U"kenneth@gnu.org, pablo@mandriva.com;euloanty@live.com"),.tel=tsc(U""),.language=tsc(U"Limburgish"),.territory=tsc(U"Belgium"),.revision=tsc(U"0.1"),.date=tsc(U"2003-11-30")},.monetary={.int_curr_symbol=tsc(U"EUR "),.currency_symbol=tsc(U"€"),.mon_decimal_point=tsc(U","),.mon_thousands_sep=tsc(U"."),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=1,.n_cs_precedes=1,.n_sep_by_space=2,.p_sign_posn=1,.n_sign_posn=4},.numeric={.decimal_point=tsc(U","),.thousands_sep=tsc(U"."),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(U"zón"),tsc(U"mao"),tsc(U"dae"),tsc(U"goo"),tsc(U"dón"),tsc(U"vri"),tsc(U"zao")},.day={tsc(U"zóndig"),tsc(U"maondig"),tsc(U"daensdig"),tsc(U"goonsdig"),tsc(U"dónderdig"),tsc(U"vriedig"),tsc(U"zaoterdig")},.abmon={tsc(U"jan"),tsc(U"fib"),tsc(U"mie"),tsc(U"epr"),tsc(U"mei"),tsc(U"jun"),tsc(U"jul"),tsc(U"aug"),tsc(U"sep"),tsc(U"okt"),tsc(U"nov"),tsc(U"des")},.mon={tsc(U"jannewarie"),tsc(U"fibberwarie"),tsc(U"miert"),tsc(U"eprèl"),tsc(U"meij"),tsc(U"junie"),tsc(U"julie"),tsc(U"augustus"),tsc(U"september"),tsc(U"oktober"),tsc(U"november"),tsc(U"desember")},.d_t_fmt=tsc(U"%a %d. %b %Y %T"),.d_fmt=tsc(U"%d.%m.%Y"),.t_fmt=tsc(U"%T"),.t_fmt_ampm=tsc(U""),.date_fmt=tsc(U"%a %d. %b %Y %T %Z"),.am_pm={tsc(U""),tsc(U"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(U"^[+1jJyY]"),.noexpr=tsc(U"^[-0nN]"),.yesstr=tsc(U"ja"),.nostr=tsc(U"nee")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c %a %l"),.int_select=tsc(U"00"),.int_prefix=tsc(U"32")},.name={.name_fmt=tsc(U"%d%t%g%t%m%t%f"),.name_gen=tsc(U"heer of mevrouw"),.name_miss=tsc(U"mevrouw"),.name_mr=tsc(U"heer"),.name_mrs=tsc(U"mevrouw"),.name_ms=tsc(U"mevrouw")},.address={.postal_fmt=tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(U"Bèlsj"),.country_post=tsc(U"B"),.country_ab2=tsc(U"BE"),.country_ab3=tsc(U"BEL"),.country_num=56,.country_car=tsc(U"B"),.lang_name=tsc(U"Lèmbörgs"),.lang_ab=tsc(U"li"),.lang_term=tsc(U"lim"),.lang_lib=tsc(U"lim")},.measurement={.measurement=1}};


}
}

#include"../main.h"