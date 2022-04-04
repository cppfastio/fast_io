﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("fi_FI"),.title=tsc("Finnish locale for Finland"),.source=tsc("RAP\t\t;\t\tfast_io"),.address=tsc("Sankt Jørgens Alle 8, DK-1615 København V, Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("fast_io"),.email=tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(""),.fax=tsc(""),.language=tsc("Finnish"),.territory=tsc("Finland"),.revision=tsc("1.0"),.date=tsc("2000-06-29")},.monetary={.int_curr_symbol=tsc("EUR "),.currency_symbol=tsc("€"),.mon_decimal_point=tsc(","),.mon_thousands_sep=tsc(" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(","),.thousands_sep=tsc(" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc("su"),tsc("ma"),tsc("ti"),tsc("ke"),tsc("to"),tsc("pe"),tsc("la")},.day={tsc("sunnuntai"),tsc("maanantai"),tsc("tiistai"),tsc("keskiviikko"),tsc("torstai"),tsc("perjantai"),tsc("lauantai")},.abmon={tsc("tammi"),tsc("helmi"),tsc("maalis"),tsc("huhti"),tsc("touko"),tsc("kesä"),tsc("heinä"),tsc("elo"),tsc("syys"),tsc("loka"),tsc("marras"),tsc("joulu")},.mon={tsc("tammikuu"),tsc("helmikuu"),tsc("maaliskuu"),tsc("huhtikuu"),tsc("toukokuu"),tsc("kesäkuu"),tsc("heinäkuu"),tsc("elokuu"),tsc("syyskuu"),tsc("lokakuu"),tsc("marraskuu"),tsc("joulukuu")},.d_t_fmt=tsc("%a %e. %Bta %Y %H.%M.%S"),.d_fmt=tsc("%d.%m.%Y"),.t_fmt=tsc("%H.%M.%S"),.t_fmt_ampm=tsc(""),.date_fmt=tsc("%a %-d.%-m.%Y %H.%M.%S %z"),.am_pm={tsc(""),tsc("")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc("^[+1KkYy]"),.noexpr=tsc("^[-0EeNn]"),.yesstr=tsc("kyllä"),.nostr=tsc("ei")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c %a %l"),.tel_dom_fmt=tsc("(%A) %l"),.int_select=tsc("00"),.int_prefix=tsc("358")},.name={.name_fmt=tsc("%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc("%f%N%d%N%b%N%a%N%s %h%t%e%t%r%N%z %T%N%c%N"),.country_name=tsc("Suomi"),.country_post=tsc("FI"),.country_ab2=tsc("FI"),.country_ab3=tsc("FIN"),.country_num=246,.country_car=tsc("FIN"),.country_isbn=tsc("952"),.lang_name=tsc("suomi"),.lang_ab=tsc("fi"),.lang_term=tsc("fin"),.lang_lib=tsc("fin")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"fi_FI"),.title=tsc(L"Finnish locale for Finland"),.source=tsc(L"RAP\t\t;\t\tfast_io"),.address=tsc(L"Sankt Jørgens Alle 8, DK-1615 København V, Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"fast_io"),.email=tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(L""),.fax=tsc(L""),.language=tsc(L"Finnish"),.territory=tsc(L"Finland"),.revision=tsc(L"1.0"),.date=tsc(L"2000-06-29")},.monetary={.int_curr_symbol=tsc(L"EUR "),.currency_symbol=tsc(L"€"),.mon_decimal_point=tsc(L","),.mon_thousands_sep=tsc(L" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(L","),.thousands_sep=tsc(L" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(L"su"),tsc(L"ma"),tsc(L"ti"),tsc(L"ke"),tsc(L"to"),tsc(L"pe"),tsc(L"la")},.day={tsc(L"sunnuntai"),tsc(L"maanantai"),tsc(L"tiistai"),tsc(L"keskiviikko"),tsc(L"torstai"),tsc(L"perjantai"),tsc(L"lauantai")},.abmon={tsc(L"tammi"),tsc(L"helmi"),tsc(L"maalis"),tsc(L"huhti"),tsc(L"touko"),tsc(L"kesä"),tsc(L"heinä"),tsc(L"elo"),tsc(L"syys"),tsc(L"loka"),tsc(L"marras"),tsc(L"joulu")},.mon={tsc(L"tammikuu"),tsc(L"helmikuu"),tsc(L"maaliskuu"),tsc(L"huhtikuu"),tsc(L"toukokuu"),tsc(L"kesäkuu"),tsc(L"heinäkuu"),tsc(L"elokuu"),tsc(L"syyskuu"),tsc(L"lokakuu"),tsc(L"marraskuu"),tsc(L"joulukuu")},.d_t_fmt=tsc(L"%a %e. %Bta %Y %H.%M.%S"),.d_fmt=tsc(L"%d.%m.%Y"),.t_fmt=tsc(L"%H.%M.%S"),.t_fmt_ampm=tsc(L""),.date_fmt=tsc(L"%a %-d.%-m.%Y %H.%M.%S %z"),.am_pm={tsc(L""),tsc(L"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(L"^[+1KkYy]"),.noexpr=tsc(L"^[-0EeNn]"),.yesstr=tsc(L"kyllä"),.nostr=tsc(L"ei")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c %a %l"),.tel_dom_fmt=tsc(L"(%A) %l"),.int_select=tsc(L"00"),.int_prefix=tsc(L"358")},.name={.name_fmt=tsc(L"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(L"%f%N%d%N%b%N%a%N%s %h%t%e%t%r%N%z %T%N%c%N"),.country_name=tsc(L"Suomi"),.country_post=tsc(L"FI"),.country_ab2=tsc(L"FI"),.country_ab3=tsc(L"FIN"),.country_num=246,.country_car=tsc(L"FIN"),.country_isbn=tsc(L"952"),.lang_name=tsc(L"suomi"),.lang_ab=tsc(L"fi"),.lang_term=tsc(L"fin"),.lang_lib=tsc(L"fin")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"fi_FI"),.title=tsc(u8"Finnish locale for Finland"),.source=tsc(u8"RAP\t\t;\t\tfast_io"),.address=tsc(u8"Sankt Jørgens Alle 8, DK-1615 København V, Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"fast_io"),.email=tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u8""),.fax=tsc(u8""),.language=tsc(u8"Finnish"),.territory=tsc(u8"Finland"),.revision=tsc(u8"1.0"),.date=tsc(u8"2000-06-29")},.monetary={.int_curr_symbol=tsc(u8"EUR "),.currency_symbol=tsc(u8"€"),.mon_decimal_point=tsc(u8","),.mon_thousands_sep=tsc(u8" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u8","),.thousands_sep=tsc(u8" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u8"su"),tsc(u8"ma"),tsc(u8"ti"),tsc(u8"ke"),tsc(u8"to"),tsc(u8"pe"),tsc(u8"la")},.day={tsc(u8"sunnuntai"),tsc(u8"maanantai"),tsc(u8"tiistai"),tsc(u8"keskiviikko"),tsc(u8"torstai"),tsc(u8"perjantai"),tsc(u8"lauantai")},.abmon={tsc(u8"tammi"),tsc(u8"helmi"),tsc(u8"maalis"),tsc(u8"huhti"),tsc(u8"touko"),tsc(u8"kesä"),tsc(u8"heinä"),tsc(u8"elo"),tsc(u8"syys"),tsc(u8"loka"),tsc(u8"marras"),tsc(u8"joulu")},.mon={tsc(u8"tammikuu"),tsc(u8"helmikuu"),tsc(u8"maaliskuu"),tsc(u8"huhtikuu"),tsc(u8"toukokuu"),tsc(u8"kesäkuu"),tsc(u8"heinäkuu"),tsc(u8"elokuu"),tsc(u8"syyskuu"),tsc(u8"lokakuu"),tsc(u8"marraskuu"),tsc(u8"joulukuu")},.d_t_fmt=tsc(u8"%a %e. %Bta %Y %H.%M.%S"),.d_fmt=tsc(u8"%d.%m.%Y"),.t_fmt=tsc(u8"%H.%M.%S"),.t_fmt_ampm=tsc(u8""),.date_fmt=tsc(u8"%a %-d.%-m.%Y %H.%M.%S %z"),.am_pm={tsc(u8""),tsc(u8"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(u8"^[+1KkYy]"),.noexpr=tsc(u8"^[-0EeNn]"),.yesstr=tsc(u8"kyllä"),.nostr=tsc(u8"ei")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c %a %l"),.tel_dom_fmt=tsc(u8"(%A) %l"),.int_select=tsc(u8"00"),.int_prefix=tsc(u8"358")},.name={.name_fmt=tsc(u8"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(u8"%f%N%d%N%b%N%a%N%s %h%t%e%t%r%N%z %T%N%c%N"),.country_name=tsc(u8"Suomi"),.country_post=tsc(u8"FI"),.country_ab2=tsc(u8"FI"),.country_ab3=tsc(u8"FIN"),.country_num=246,.country_car=tsc(u8"FIN"),.country_isbn=tsc(u8"952"),.lang_name=tsc(u8"suomi"),.lang_ab=tsc(u8"fi"),.lang_term=tsc(u8"fin"),.lang_lib=tsc(u8"fin")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"fi_FI"),.title=tsc(u"Finnish locale for Finland"),.source=tsc(u"RAP\t\t;\t\tfast_io"),.address=tsc(u"Sankt Jørgens Alle 8, DK-1615 København V, Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"fast_io"),.email=tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u""),.fax=tsc(u""),.language=tsc(u"Finnish"),.territory=tsc(u"Finland"),.revision=tsc(u"1.0"),.date=tsc(u"2000-06-29")},.monetary={.int_curr_symbol=tsc(u"EUR "),.currency_symbol=tsc(u"€"),.mon_decimal_point=tsc(u","),.mon_thousands_sep=tsc(u" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u","),.thousands_sep=tsc(u" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u"su"),tsc(u"ma"),tsc(u"ti"),tsc(u"ke"),tsc(u"to"),tsc(u"pe"),tsc(u"la")},.day={tsc(u"sunnuntai"),tsc(u"maanantai"),tsc(u"tiistai"),tsc(u"keskiviikko"),tsc(u"torstai"),tsc(u"perjantai"),tsc(u"lauantai")},.abmon={tsc(u"tammi"),tsc(u"helmi"),tsc(u"maalis"),tsc(u"huhti"),tsc(u"touko"),tsc(u"kesä"),tsc(u"heinä"),tsc(u"elo"),tsc(u"syys"),tsc(u"loka"),tsc(u"marras"),tsc(u"joulu")},.mon={tsc(u"tammikuu"),tsc(u"helmikuu"),tsc(u"maaliskuu"),tsc(u"huhtikuu"),tsc(u"toukokuu"),tsc(u"kesäkuu"),tsc(u"heinäkuu"),tsc(u"elokuu"),tsc(u"syyskuu"),tsc(u"lokakuu"),tsc(u"marraskuu"),tsc(u"joulukuu")},.d_t_fmt=tsc(u"%a %e. %Bta %Y %H.%M.%S"),.d_fmt=tsc(u"%d.%m.%Y"),.t_fmt=tsc(u"%H.%M.%S"),.t_fmt_ampm=tsc(u""),.date_fmt=tsc(u"%a %-d.%-m.%Y %H.%M.%S %z"),.am_pm={tsc(u""),tsc(u"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(u"^[+1KkYy]"),.noexpr=tsc(u"^[-0EeNn]"),.yesstr=tsc(u"kyllä"),.nostr=tsc(u"ei")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c %a %l"),.tel_dom_fmt=tsc(u"(%A) %l"),.int_select=tsc(u"00"),.int_prefix=tsc(u"358")},.name={.name_fmt=tsc(u"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(u"%f%N%d%N%b%N%a%N%s %h%t%e%t%r%N%z %T%N%c%N"),.country_name=tsc(u"Suomi"),.country_post=tsc(u"FI"),.country_ab2=tsc(u"FI"),.country_ab3=tsc(u"FIN"),.country_num=246,.country_car=tsc(u"FIN"),.country_isbn=tsc(u"952"),.lang_name=tsc(u"suomi"),.lang_ab=tsc(u"fi"),.lang_term=tsc(u"fin"),.lang_lib=tsc(u"fin")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"fi_FI"),.title=tsc(U"Finnish locale for Finland"),.source=tsc(U"RAP\t\t;\t\tfast_io"),.address=tsc(U"Sankt Jørgens Alle 8, DK-1615 København V, Danmark\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"fast_io"),.email=tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(U""),.fax=tsc(U""),.language=tsc(U"Finnish"),.territory=tsc(U"Finland"),.revision=tsc(U"1.0"),.date=tsc(U"2000-06-29")},.monetary={.int_curr_symbol=tsc(U"EUR "),.currency_symbol=tsc(U"€"),.mon_decimal_point=tsc(U","),.mon_thousands_sep=tsc(U" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(U","),.thousands_sep=tsc(U" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(U"su"),tsc(U"ma"),tsc(U"ti"),tsc(U"ke"),tsc(U"to"),tsc(U"pe"),tsc(U"la")},.day={tsc(U"sunnuntai"),tsc(U"maanantai"),tsc(U"tiistai"),tsc(U"keskiviikko"),tsc(U"torstai"),tsc(U"perjantai"),tsc(U"lauantai")},.abmon={tsc(U"tammi"),tsc(U"helmi"),tsc(U"maalis"),tsc(U"huhti"),tsc(U"touko"),tsc(U"kesä"),tsc(U"heinä"),tsc(U"elo"),tsc(U"syys"),tsc(U"loka"),tsc(U"marras"),tsc(U"joulu")},.mon={tsc(U"tammikuu"),tsc(U"helmikuu"),tsc(U"maaliskuu"),tsc(U"huhtikuu"),tsc(U"toukokuu"),tsc(U"kesäkuu"),tsc(U"heinäkuu"),tsc(U"elokuu"),tsc(U"syyskuu"),tsc(U"lokakuu"),tsc(U"marraskuu"),tsc(U"joulukuu")},.d_t_fmt=tsc(U"%a %e. %Bta %Y %H.%M.%S"),.d_fmt=tsc(U"%d.%m.%Y"),.t_fmt=tsc(U"%H.%M.%S"),.t_fmt_ampm=tsc(U""),.date_fmt=tsc(U"%a %-d.%-m.%Y %H.%M.%S %z"),.am_pm={tsc(U""),tsc(U"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(U"^[+1KkYy]"),.noexpr=tsc(U"^[-0EeNn]"),.yesstr=tsc(U"kyllä"),.nostr=tsc(U"ei")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c %a %l"),.tel_dom_fmt=tsc(U"(%A) %l"),.int_select=tsc(U"00"),.int_prefix=tsc(U"358")},.name={.name_fmt=tsc(U"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(U"%f%N%d%N%b%N%a%N%s %h%t%e%t%r%N%z %T%N%c%N"),.country_name=tsc(U"Suomi"),.country_post=tsc(U"FI"),.country_ab2=tsc(U"FI"),.country_ab3=tsc(U"FIN"),.country_num=246,.country_car=tsc(U"FIN"),.country_isbn=tsc(U"952"),.lang_name=tsc(U"suomi"),.lang_ab=tsc(U"fi"),.lang_term=tsc(U"fin"),.lang_lib=tsc(U"fin")},.measurement={.measurement=1}};


}
}

#include"../main.h"