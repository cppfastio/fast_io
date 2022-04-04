﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3,2};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("te_IN"),.title=tsc("Telugu language locale for India"),.source=tsc("IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc("1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("fast_io"),.email=tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(""),.fax=tsc(""),.language=tsc("Telugu"),.territory=tsc("India"),.revision=tsc("0.95"),.date=tsc("2004-10-05")},.monetary={.int_curr_symbol=tsc("INR "),.currency_symbol=tsc("₹"),.mon_decimal_point=tsc("."),.mon_thousands_sep=tsc(","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc("."),.thousands_sep=tsc(","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc("ఆది"),tsc("సోమ"),tsc("మంగళ"),tsc("బుధ"),tsc("గురు"),tsc("శుక్ర"),tsc("శని")},.day={tsc("ఆదివారం"),tsc("సోమవారం"),tsc("మంగళవారం"),tsc("బుధవారం"),tsc("గురువారం"),tsc("శుక్రవారం"),tsc("శనివారం")},.abmon={tsc("జన"),tsc("ఫిబ్ర"),tsc("మార్చి"),tsc("ఏప్రి"),tsc("మే"),tsc("జూన్"),tsc("జులై"),tsc("ఆగ"),tsc("సెప్టెం"),tsc("అక్టో"),tsc("నవం"),tsc("డిసెం")},.mon={tsc("జనవరి"),tsc("ఫిబ్రవరి"),tsc("మార్చి"),tsc("ఏప్రిల్"),tsc("మే"),tsc("జూన్"),tsc("జులై"),tsc("ఆగస్టు"),tsc("సెప్టెంబర్"),tsc("అక్టోబర్"),tsc("నవంబర్"),tsc("డిసెంబర్")},.d_t_fmt=tsc("%B %d %A %Y %p%I.%M.%S"),.d_fmt=tsc("%d-%m-%y"),.t_fmt=tsc("%p%I.%M.%S %Z"),.t_fmt_ampm=tsc("%p%I.%M.%S %Z"),.date_fmt=tsc("%B %d %A %Y %p%I.%M.%S %Z"),.am_pm={tsc("ఉ."),tsc("సా.")},.week={7,19971130,1}},.messages={.yesexpr=tsc("^[+1yYఅ]"),.noexpr=tsc("^[-0nNవ]"),.yesstr=tsc("అవును"),.nostr=tsc("వద్దు")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c ;%a ;%l"),.int_select=tsc("00"),.int_prefix=tsc("91")},.name={.name_fmt=tsc("%p%t%f%t%g%t%m"),.name_gen=tsc(""),.name_miss=tsc("కుమారి"),.name_mr=tsc("శ్రీ"),.name_mrs=tsc("శ్రీమతి"),.name_ms=tsc("")},.address={.postal_fmt=tsc("%a%N%d%N%f%N%r%t%e%N%h%t%b%N%s%t%N%T%t%z%N%S%N%c"),.country_name=tsc("భారతదేశం"),.country_ab2=tsc("IN"),.country_ab3=tsc("IND"),.country_num=356,.country_car=tsc("IND"),.lang_name=tsc("తెలుగు"),.lang_ab=tsc("te"),.lang_term=tsc("tel"),.lang_lib=tsc("tel")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"te_IN"),.title=tsc(L"Telugu language locale for India"),.source=tsc(L"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(L"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"fast_io"),.email=tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(L""),.fax=tsc(L""),.language=tsc(L"Telugu"),.territory=tsc(L"India"),.revision=tsc(L"0.95"),.date=tsc(L"2004-10-05")},.monetary={.int_curr_symbol=tsc(L"INR "),.currency_symbol=tsc(L"₹"),.mon_decimal_point=tsc(L"."),.mon_thousands_sep=tsc(L","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(L"."),.thousands_sep=tsc(L","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc(L"ఆది"),tsc(L"సోమ"),tsc(L"మంగళ"),tsc(L"బుధ"),tsc(L"గురు"),tsc(L"శుక్ర"),tsc(L"శని")},.day={tsc(L"ఆదివారం"),tsc(L"సోమవారం"),tsc(L"మంగళవారం"),tsc(L"బుధవారం"),tsc(L"గురువారం"),tsc(L"శుక్రవారం"),tsc(L"శనివారం")},.abmon={tsc(L"జన"),tsc(L"ఫిబ్ర"),tsc(L"మార్చి"),tsc(L"ఏప్రి"),tsc(L"మే"),tsc(L"జూన్"),tsc(L"జులై"),tsc(L"ఆగ"),tsc(L"సెప్టెం"),tsc(L"అక్టో"),tsc(L"నవం"),tsc(L"డిసెం")},.mon={tsc(L"జనవరి"),tsc(L"ఫిబ్రవరి"),tsc(L"మార్చి"),tsc(L"ఏప్రిల్"),tsc(L"మే"),tsc(L"జూన్"),tsc(L"జులై"),tsc(L"ఆగస్టు"),tsc(L"సెప్టెంబర్"),tsc(L"అక్టోబర్"),tsc(L"నవంబర్"),tsc(L"డిసెంబర్")},.d_t_fmt=tsc(L"%B %d %A %Y %p%I.%M.%S"),.d_fmt=tsc(L"%d-%m-%y"),.t_fmt=tsc(L"%p%I.%M.%S %Z"),.t_fmt_ampm=tsc(L"%p%I.%M.%S %Z"),.date_fmt=tsc(L"%B %d %A %Y %p%I.%M.%S %Z"),.am_pm={tsc(L"ఉ."),tsc(L"సా.")},.week={7,19971130,1}},.messages={.yesexpr=tsc(L"^[+1yYఅ]"),.noexpr=tsc(L"^[-0nNవ]"),.yesstr=tsc(L"అవును"),.nostr=tsc(L"వద్దు")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c ;%a ;%l"),.int_select=tsc(L"00"),.int_prefix=tsc(L"91")},.name={.name_fmt=tsc(L"%p%t%f%t%g%t%m"),.name_gen=tsc(L""),.name_miss=tsc(L"కుమారి"),.name_mr=tsc(L"శ్రీ"),.name_mrs=tsc(L"శ్రీమతి"),.name_ms=tsc(L"")},.address={.postal_fmt=tsc(L"%a%N%d%N%f%N%r%t%e%N%h%t%b%N%s%t%N%T%t%z%N%S%N%c"),.country_name=tsc(L"భారతదేశం"),.country_ab2=tsc(L"IN"),.country_ab3=tsc(L"IND"),.country_num=356,.country_car=tsc(L"IND"),.lang_name=tsc(L"తెలుగు"),.lang_ab=tsc(L"te"),.lang_term=tsc(L"tel"),.lang_lib=tsc(L"tel")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"te_IN"),.title=tsc(u8"Telugu language locale for India"),.source=tsc(u8"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(u8"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"fast_io"),.email=tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u8""),.fax=tsc(u8""),.language=tsc(u8"Telugu"),.territory=tsc(u8"India"),.revision=tsc(u8"0.95"),.date=tsc(u8"2004-10-05")},.monetary={.int_curr_symbol=tsc(u8"INR "),.currency_symbol=tsc(u8"₹"),.mon_decimal_point=tsc(u8"."),.mon_thousands_sep=tsc(u8","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u8"."),.thousands_sep=tsc(u8","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc(u8"ఆది"),tsc(u8"సోమ"),tsc(u8"మంగళ"),tsc(u8"బుధ"),tsc(u8"గురు"),tsc(u8"శుక్ర"),tsc(u8"శని")},.day={tsc(u8"ఆదివారం"),tsc(u8"సోమవారం"),tsc(u8"మంగళవారం"),tsc(u8"బుధవారం"),tsc(u8"గురువారం"),tsc(u8"శుక్రవారం"),tsc(u8"శనివారం")},.abmon={tsc(u8"జన"),tsc(u8"ఫిబ్ర"),tsc(u8"మార్చి"),tsc(u8"ఏప్రి"),tsc(u8"మే"),tsc(u8"జూన్"),tsc(u8"జులై"),tsc(u8"ఆగ"),tsc(u8"సెప్టెం"),tsc(u8"అక్టో"),tsc(u8"నవం"),tsc(u8"డిసెం")},.mon={tsc(u8"జనవరి"),tsc(u8"ఫిబ్రవరి"),tsc(u8"మార్చి"),tsc(u8"ఏప్రిల్"),tsc(u8"మే"),tsc(u8"జూన్"),tsc(u8"జులై"),tsc(u8"ఆగస్టు"),tsc(u8"సెప్టెంబర్"),tsc(u8"అక్టోబర్"),tsc(u8"నవంబర్"),tsc(u8"డిసెంబర్")},.d_t_fmt=tsc(u8"%B %d %A %Y %p%I.%M.%S"),.d_fmt=tsc(u8"%d-%m-%y"),.t_fmt=tsc(u8"%p%I.%M.%S %Z"),.t_fmt_ampm=tsc(u8"%p%I.%M.%S %Z"),.date_fmt=tsc(u8"%B %d %A %Y %p%I.%M.%S %Z"),.am_pm={tsc(u8"ఉ."),tsc(u8"సా.")},.week={7,19971130,1}},.messages={.yesexpr=tsc(u8"^[+1yYఅ]"),.noexpr=tsc(u8"^[-0nNవ]"),.yesstr=tsc(u8"అవును"),.nostr=tsc(u8"వద్దు")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c ;%a ;%l"),.int_select=tsc(u8"00"),.int_prefix=tsc(u8"91")},.name={.name_fmt=tsc(u8"%p%t%f%t%g%t%m"),.name_gen=tsc(u8""),.name_miss=tsc(u8"కుమారి"),.name_mr=tsc(u8"శ్రీ"),.name_mrs=tsc(u8"శ్రీమతి"),.name_ms=tsc(u8"")},.address={.postal_fmt=tsc(u8"%a%N%d%N%f%N%r%t%e%N%h%t%b%N%s%t%N%T%t%z%N%S%N%c"),.country_name=tsc(u8"భారతదేశం"),.country_ab2=tsc(u8"IN"),.country_ab3=tsc(u8"IND"),.country_num=356,.country_car=tsc(u8"IND"),.lang_name=tsc(u8"తెలుగు"),.lang_ab=tsc(u8"te"),.lang_term=tsc(u8"tel"),.lang_lib=tsc(u8"tel")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"te_IN"),.title=tsc(u"Telugu language locale for India"),.source=tsc(u"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(u"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"fast_io"),.email=tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u""),.fax=tsc(u""),.language=tsc(u"Telugu"),.territory=tsc(u"India"),.revision=tsc(u"0.95"),.date=tsc(u"2004-10-05")},.monetary={.int_curr_symbol=tsc(u"INR "),.currency_symbol=tsc(u"₹"),.mon_decimal_point=tsc(u"."),.mon_thousands_sep=tsc(u","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u"."),.thousands_sep=tsc(u","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc(u"ఆది"),tsc(u"సోమ"),tsc(u"మంగళ"),tsc(u"బుధ"),tsc(u"గురు"),tsc(u"శుక్ర"),tsc(u"శని")},.day={tsc(u"ఆదివారం"),tsc(u"సోమవారం"),tsc(u"మంగళవారం"),tsc(u"బుధవారం"),tsc(u"గురువారం"),tsc(u"శుక్రవారం"),tsc(u"శనివారం")},.abmon={tsc(u"జన"),tsc(u"ఫిబ్ర"),tsc(u"మార్చి"),tsc(u"ఏప్రి"),tsc(u"మే"),tsc(u"జూన్"),tsc(u"జులై"),tsc(u"ఆగ"),tsc(u"సెప్టెం"),tsc(u"అక్టో"),tsc(u"నవం"),tsc(u"డిసెం")},.mon={tsc(u"జనవరి"),tsc(u"ఫిబ్రవరి"),tsc(u"మార్చి"),tsc(u"ఏప్రిల్"),tsc(u"మే"),tsc(u"జూన్"),tsc(u"జులై"),tsc(u"ఆగస్టు"),tsc(u"సెప్టెంబర్"),tsc(u"అక్టోబర్"),tsc(u"నవంబర్"),tsc(u"డిసెంబర్")},.d_t_fmt=tsc(u"%B %d %A %Y %p%I.%M.%S"),.d_fmt=tsc(u"%d-%m-%y"),.t_fmt=tsc(u"%p%I.%M.%S %Z"),.t_fmt_ampm=tsc(u"%p%I.%M.%S %Z"),.date_fmt=tsc(u"%B %d %A %Y %p%I.%M.%S %Z"),.am_pm={tsc(u"ఉ."),tsc(u"సా.")},.week={7,19971130,1}},.messages={.yesexpr=tsc(u"^[+1yYఅ]"),.noexpr=tsc(u"^[-0nNవ]"),.yesstr=tsc(u"అవును"),.nostr=tsc(u"వద్దు")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c ;%a ;%l"),.int_select=tsc(u"00"),.int_prefix=tsc(u"91")},.name={.name_fmt=tsc(u"%p%t%f%t%g%t%m"),.name_gen=tsc(u""),.name_miss=tsc(u"కుమారి"),.name_mr=tsc(u"శ్రీ"),.name_mrs=tsc(u"శ్రీమతి"),.name_ms=tsc(u"")},.address={.postal_fmt=tsc(u"%a%N%d%N%f%N%r%t%e%N%h%t%b%N%s%t%N%T%t%z%N%S%N%c"),.country_name=tsc(u"భారతదేశం"),.country_ab2=tsc(u"IN"),.country_ab3=tsc(u"IND"),.country_num=356,.country_car=tsc(u"IND"),.lang_name=tsc(u"తెలుగు"),.lang_ab=tsc(u"te"),.lang_term=tsc(u"tel"),.lang_lib=tsc(u"tel")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"te_IN"),.title=tsc(U"Telugu language locale for India"),.source=tsc(U"IBM Globalization Center of Competency, Yamato Software Laboratory\t\t;\t\tfast_io"),.address=tsc(U"1623-14, Shimotsuruma, Yamato-shi, Kanagawa-ken, 242-8502, Japan\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"fast_io"),.email=tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(U""),.fax=tsc(U""),.language=tsc(U"Telugu"),.territory=tsc(U"India"),.revision=tsc(U"0.95"),.date=tsc(U"2004-10-05")},.monetary={.int_curr_symbol=tsc(U"INR "),.currency_symbol=tsc(U"₹"),.mon_decimal_point=tsc(U"."),.mon_thousands_sep=tsc(U","),.mon_grouping={numeric_grouping_storage,2},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(U"."),.thousands_sep=tsc(U","),.grouping={numeric_grouping_storage,2}},.time={.abday={tsc(U"ఆది"),tsc(U"సోమ"),tsc(U"మంగళ"),tsc(U"బుధ"),tsc(U"గురు"),tsc(U"శుక్ర"),tsc(U"శని")},.day={tsc(U"ఆదివారం"),tsc(U"సోమవారం"),tsc(U"మంగళవారం"),tsc(U"బుధవారం"),tsc(U"గురువారం"),tsc(U"శుక్రవారం"),tsc(U"శనివారం")},.abmon={tsc(U"జన"),tsc(U"ఫిబ్ర"),tsc(U"మార్చి"),tsc(U"ఏప్రి"),tsc(U"మే"),tsc(U"జూన్"),tsc(U"జులై"),tsc(U"ఆగ"),tsc(U"సెప్టెం"),tsc(U"అక్టో"),tsc(U"నవం"),tsc(U"డిసెం")},.mon={tsc(U"జనవరి"),tsc(U"ఫిబ్రవరి"),tsc(U"మార్చి"),tsc(U"ఏప్రిల్"),tsc(U"మే"),tsc(U"జూన్"),tsc(U"జులై"),tsc(U"ఆగస్టు"),tsc(U"సెప్టెంబర్"),tsc(U"అక్టోబర్"),tsc(U"నవంబర్"),tsc(U"డిసెంబర్")},.d_t_fmt=tsc(U"%B %d %A %Y %p%I.%M.%S"),.d_fmt=tsc(U"%d-%m-%y"),.t_fmt=tsc(U"%p%I.%M.%S %Z"),.t_fmt_ampm=tsc(U"%p%I.%M.%S %Z"),.date_fmt=tsc(U"%B %d %A %Y %p%I.%M.%S %Z"),.am_pm={tsc(U"ఉ."),tsc(U"సా.")},.week={7,19971130,1}},.messages={.yesexpr=tsc(U"^[+1yYఅ]"),.noexpr=tsc(U"^[-0nNవ]"),.yesstr=tsc(U"అవును"),.nostr=tsc(U"వద్దు")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c ;%a ;%l"),.int_select=tsc(U"00"),.int_prefix=tsc(U"91")},.name={.name_fmt=tsc(U"%p%t%f%t%g%t%m"),.name_gen=tsc(U""),.name_miss=tsc(U"కుమారి"),.name_mr=tsc(U"శ్రీ"),.name_mrs=tsc(U"శ్రీమతి"),.name_ms=tsc(U"")},.address={.postal_fmt=tsc(U"%a%N%d%N%f%N%r%t%e%N%h%t%b%N%s%t%N%T%t%z%N%S%N%c"),.country_name=tsc(U"భారతదేశం"),.country_ab2=tsc(U"IN"),.country_ab3=tsc(U"IND"),.country_num=356,.country_car=tsc(U"IND"),.lang_name=tsc(U"తెలుగు"),.lang_ab=tsc(U"te"),.lang_term=tsc(U"tel"),.lang_lib=tsc(U"tel")},.measurement={.measurement=1}};


}
}

#include"../main.h"