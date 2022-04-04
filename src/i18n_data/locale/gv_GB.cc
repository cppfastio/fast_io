﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("gv_GB"),.title=tsc("Manx Gaelic locale for Britain"),.source=tsc("Alastair McKinstry\t\t;\t\tfast_io"),.address=tsc("Croí Lár, Ballinahalla, Maigh Cuilinn,, Co. Gaillimh, Ireland\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("fast_io"),.email=tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(""),.fax=tsc(""),.language=tsc("Manx"),.territory=tsc("United Kingdom"),.revision=tsc("1.0"),.date=tsc("2000-06-29")},.monetary={.int_curr_symbol=tsc("GBP "),.currency_symbol=tsc("£"),.mon_decimal_point=tsc("."),.mon_thousands_sep=tsc(","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc("."),.thousands_sep=tsc(","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc("Jed"),tsc("Jel"),tsc("Jem"),tsc("Jerc"),tsc("Jerd"),tsc("Jeh"),tsc("Jes")},.day={tsc("Jedoonee"),tsc("Jelhein"),tsc("Jemayrt"),tsc("Jercean"),tsc("Jerdein"),tsc("Jeheiney"),tsc("Jesarn")},.abmon={tsc("J-guer"),tsc("T-arree"),tsc("Mayrnt"),tsc("Avrril"),tsc("Boaldyn"),tsc("M-souree"),tsc("J-souree"),tsc("Luanistyn"),tsc("M-fouyir"),tsc("J-fouyir"),tsc("M.Houney"),tsc("M.Nollick")},.mon={tsc("Jerrey-geuree"),tsc("Toshiaght-arree"),tsc("Mayrnt"),tsc("Averil"),tsc("Boaldyn"),tsc("Mean-souree"),tsc("Jerrey-souree"),tsc("Luanistyn"),tsc("Mean-fouyir"),tsc("Jerrey-fouyir"),tsc("Mee Houney"),tsc("Mee ny Nollick")},.d_t_fmt=tsc("%a %d %b %Y %T"),.d_fmt=tsc("%d//%m//%y"),.t_fmt=tsc("%T"),.t_fmt_ampm=tsc(""),.date_fmt=tsc("%a %d %b %Y %T %Z"),.am_pm={tsc(""),tsc("")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc("^[+1yY]"),.noexpr=tsc("^[-0nN]")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c %a %l"),.int_select=tsc("00"),.int_prefix=tsc("44")},.name={.name_fmt=tsc("%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc("Rywvaneth Unys"),.country_ab2=tsc("GB"),.country_ab3=tsc("GBR"),.country_num=826,.country_car=tsc("GB"),.lang_name=tsc("Gaelg"),.lang_ab=tsc("gv"),.lang_term=tsc("glv"),.lang_lib=tsc("glv")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"gv_GB"),.title=tsc(L"Manx Gaelic locale for Britain"),.source=tsc(L"Alastair McKinstry\t\t;\t\tfast_io"),.address=tsc(L"Croí Lár, Ballinahalla, Maigh Cuilinn,, Co. Gaillimh, Ireland\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"fast_io"),.email=tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(L""),.fax=tsc(L""),.language=tsc(L"Manx"),.territory=tsc(L"United Kingdom"),.revision=tsc(L"1.0"),.date=tsc(L"2000-06-29")},.monetary={.int_curr_symbol=tsc(L"GBP "),.currency_symbol=tsc(L"£"),.mon_decimal_point=tsc(L"."),.mon_thousands_sep=tsc(L","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(L"."),.thousands_sep=tsc(L","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(L"Jed"),tsc(L"Jel"),tsc(L"Jem"),tsc(L"Jerc"),tsc(L"Jerd"),tsc(L"Jeh"),tsc(L"Jes")},.day={tsc(L"Jedoonee"),tsc(L"Jelhein"),tsc(L"Jemayrt"),tsc(L"Jercean"),tsc(L"Jerdein"),tsc(L"Jeheiney"),tsc(L"Jesarn")},.abmon={tsc(L"J-guer"),tsc(L"T-arree"),tsc(L"Mayrnt"),tsc(L"Avrril"),tsc(L"Boaldyn"),tsc(L"M-souree"),tsc(L"J-souree"),tsc(L"Luanistyn"),tsc(L"M-fouyir"),tsc(L"J-fouyir"),tsc(L"M.Houney"),tsc(L"M.Nollick")},.mon={tsc(L"Jerrey-geuree"),tsc(L"Toshiaght-arree"),tsc(L"Mayrnt"),tsc(L"Averil"),tsc(L"Boaldyn"),tsc(L"Mean-souree"),tsc(L"Jerrey-souree"),tsc(L"Luanistyn"),tsc(L"Mean-fouyir"),tsc(L"Jerrey-fouyir"),tsc(L"Mee Houney"),tsc(L"Mee ny Nollick")},.d_t_fmt=tsc(L"%a %d %b %Y %T"),.d_fmt=tsc(L"%d//%m//%y"),.t_fmt=tsc(L"%T"),.t_fmt_ampm=tsc(L""),.date_fmt=tsc(L"%a %d %b %Y %T %Z"),.am_pm={tsc(L""),tsc(L"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(L"^[+1yY]"),.noexpr=tsc(L"^[-0nN]")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c %a %l"),.int_select=tsc(L"00"),.int_prefix=tsc(L"44")},.name={.name_fmt=tsc(L"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(L"Rywvaneth Unys"),.country_ab2=tsc(L"GB"),.country_ab3=tsc(L"GBR"),.country_num=826,.country_car=tsc(L"GB"),.lang_name=tsc(L"Gaelg"),.lang_ab=tsc(L"gv"),.lang_term=tsc(L"glv"),.lang_lib=tsc(L"glv")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"gv_GB"),.title=tsc(u8"Manx Gaelic locale for Britain"),.source=tsc(u8"Alastair McKinstry\t\t;\t\tfast_io"),.address=tsc(u8"Croí Lár, Ballinahalla, Maigh Cuilinn,, Co. Gaillimh, Ireland\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"fast_io"),.email=tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u8""),.fax=tsc(u8""),.language=tsc(u8"Manx"),.territory=tsc(u8"United Kingdom"),.revision=tsc(u8"1.0"),.date=tsc(u8"2000-06-29")},.monetary={.int_curr_symbol=tsc(u8"GBP "),.currency_symbol=tsc(u8"£"),.mon_decimal_point=tsc(u8"."),.mon_thousands_sep=tsc(u8","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u8"."),.thousands_sep=tsc(u8","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u8"Jed"),tsc(u8"Jel"),tsc(u8"Jem"),tsc(u8"Jerc"),tsc(u8"Jerd"),tsc(u8"Jeh"),tsc(u8"Jes")},.day={tsc(u8"Jedoonee"),tsc(u8"Jelhein"),tsc(u8"Jemayrt"),tsc(u8"Jercean"),tsc(u8"Jerdein"),tsc(u8"Jeheiney"),tsc(u8"Jesarn")},.abmon={tsc(u8"J-guer"),tsc(u8"T-arree"),tsc(u8"Mayrnt"),tsc(u8"Avrril"),tsc(u8"Boaldyn"),tsc(u8"M-souree"),tsc(u8"J-souree"),tsc(u8"Luanistyn"),tsc(u8"M-fouyir"),tsc(u8"J-fouyir"),tsc(u8"M.Houney"),tsc(u8"M.Nollick")},.mon={tsc(u8"Jerrey-geuree"),tsc(u8"Toshiaght-arree"),tsc(u8"Mayrnt"),tsc(u8"Averil"),tsc(u8"Boaldyn"),tsc(u8"Mean-souree"),tsc(u8"Jerrey-souree"),tsc(u8"Luanistyn"),tsc(u8"Mean-fouyir"),tsc(u8"Jerrey-fouyir"),tsc(u8"Mee Houney"),tsc(u8"Mee ny Nollick")},.d_t_fmt=tsc(u8"%a %d %b %Y %T"),.d_fmt=tsc(u8"%d//%m//%y"),.t_fmt=tsc(u8"%T"),.t_fmt_ampm=tsc(u8""),.date_fmt=tsc(u8"%a %d %b %Y %T %Z"),.am_pm={tsc(u8""),tsc(u8"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(u8"^[+1yY]"),.noexpr=tsc(u8"^[-0nN]")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c %a %l"),.int_select=tsc(u8"00"),.int_prefix=tsc(u8"44")},.name={.name_fmt=tsc(u8"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u8"Rywvaneth Unys"),.country_ab2=tsc(u8"GB"),.country_ab3=tsc(u8"GBR"),.country_num=826,.country_car=tsc(u8"GB"),.lang_name=tsc(u8"Gaelg"),.lang_ab=tsc(u8"gv"),.lang_term=tsc(u8"glv"),.lang_lib=tsc(u8"glv")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"gv_GB"),.title=tsc(u"Manx Gaelic locale for Britain"),.source=tsc(u"Alastair McKinstry\t\t;\t\tfast_io"),.address=tsc(u"Croí Lár, Ballinahalla, Maigh Cuilinn,, Co. Gaillimh, Ireland\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"fast_io"),.email=tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u""),.fax=tsc(u""),.language=tsc(u"Manx"),.territory=tsc(u"United Kingdom"),.revision=tsc(u"1.0"),.date=tsc(u"2000-06-29")},.monetary={.int_curr_symbol=tsc(u"GBP "),.currency_symbol=tsc(u"£"),.mon_decimal_point=tsc(u"."),.mon_thousands_sep=tsc(u","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u"."),.thousands_sep=tsc(u","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u"Jed"),tsc(u"Jel"),tsc(u"Jem"),tsc(u"Jerc"),tsc(u"Jerd"),tsc(u"Jeh"),tsc(u"Jes")},.day={tsc(u"Jedoonee"),tsc(u"Jelhein"),tsc(u"Jemayrt"),tsc(u"Jercean"),tsc(u"Jerdein"),tsc(u"Jeheiney"),tsc(u"Jesarn")},.abmon={tsc(u"J-guer"),tsc(u"T-arree"),tsc(u"Mayrnt"),tsc(u"Avrril"),tsc(u"Boaldyn"),tsc(u"M-souree"),tsc(u"J-souree"),tsc(u"Luanistyn"),tsc(u"M-fouyir"),tsc(u"J-fouyir"),tsc(u"M.Houney"),tsc(u"M.Nollick")},.mon={tsc(u"Jerrey-geuree"),tsc(u"Toshiaght-arree"),tsc(u"Mayrnt"),tsc(u"Averil"),tsc(u"Boaldyn"),tsc(u"Mean-souree"),tsc(u"Jerrey-souree"),tsc(u"Luanistyn"),tsc(u"Mean-fouyir"),tsc(u"Jerrey-fouyir"),tsc(u"Mee Houney"),tsc(u"Mee ny Nollick")},.d_t_fmt=tsc(u"%a %d %b %Y %T"),.d_fmt=tsc(u"%d//%m//%y"),.t_fmt=tsc(u"%T"),.t_fmt_ampm=tsc(u""),.date_fmt=tsc(u"%a %d %b %Y %T %Z"),.am_pm={tsc(u""),tsc(u"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(u"^[+1yY]"),.noexpr=tsc(u"^[-0nN]")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c %a %l"),.int_select=tsc(u"00"),.int_prefix=tsc(u"44")},.name={.name_fmt=tsc(u"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u"Rywvaneth Unys"),.country_ab2=tsc(u"GB"),.country_ab3=tsc(u"GBR"),.country_num=826,.country_car=tsc(u"GB"),.lang_name=tsc(u"Gaelg"),.lang_ab=tsc(u"gv"),.lang_term=tsc(u"glv"),.lang_lib=tsc(u"glv")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"gv_GB"),.title=tsc(U"Manx Gaelic locale for Britain"),.source=tsc(U"Alastair McKinstry\t\t;\t\tfast_io"),.address=tsc(U"Croí Lár, Ballinahalla, Maigh Cuilinn,, Co. Gaillimh, Ireland\t\t;\t\thttps://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"fast_io"),.email=tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(U""),.fax=tsc(U""),.language=tsc(U"Manx"),.territory=tsc(U"United Kingdom"),.revision=tsc(U"1.0"),.date=tsc(U"2000-06-29")},.monetary={.int_curr_symbol=tsc(U"GBP "),.currency_symbol=tsc(U"£"),.mon_decimal_point=tsc(U"."),.mon_thousands_sep=tsc(U","),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=1,.p_sep_by_space=0,.n_cs_precedes=1,.n_sep_by_space=0,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(U"."),.thousands_sep=tsc(U","),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(U"Jed"),tsc(U"Jel"),tsc(U"Jem"),tsc(U"Jerc"),tsc(U"Jerd"),tsc(U"Jeh"),tsc(U"Jes")},.day={tsc(U"Jedoonee"),tsc(U"Jelhein"),tsc(U"Jemayrt"),tsc(U"Jercean"),tsc(U"Jerdein"),tsc(U"Jeheiney"),tsc(U"Jesarn")},.abmon={tsc(U"J-guer"),tsc(U"T-arree"),tsc(U"Mayrnt"),tsc(U"Avrril"),tsc(U"Boaldyn"),tsc(U"M-souree"),tsc(U"J-souree"),tsc(U"Luanistyn"),tsc(U"M-fouyir"),tsc(U"J-fouyir"),tsc(U"M.Houney"),tsc(U"M.Nollick")},.mon={tsc(U"Jerrey-geuree"),tsc(U"Toshiaght-arree"),tsc(U"Mayrnt"),tsc(U"Averil"),tsc(U"Boaldyn"),tsc(U"Mean-souree"),tsc(U"Jerrey-souree"),tsc(U"Luanistyn"),tsc(U"Mean-fouyir"),tsc(U"Jerrey-fouyir"),tsc(U"Mee Houney"),tsc(U"Mee ny Nollick")},.d_t_fmt=tsc(U"%a %d %b %Y %T"),.d_fmt=tsc(U"%d//%m//%y"),.t_fmt=tsc(U"%T"),.t_fmt_ampm=tsc(U""),.date_fmt=tsc(U"%a %d %b %Y %T %Z"),.am_pm={tsc(U""),tsc(U"")},.week={7,19971130,4},.first_weekday=2},.messages={.yesexpr=tsc(U"^[+1yY]"),.noexpr=tsc(U"^[-0nN]")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c %a %l"),.int_select=tsc(U"00"),.int_prefix=tsc(U"44")},.name={.name_fmt=tsc(U"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(U"Rywvaneth Unys"),.country_ab2=tsc(U"GB"),.country_ab3=tsc(U"GBR"),.country_num=826,.country_car=tsc(U"GB"),.lang_name=tsc(U"Gaelg"),.lang_ab=tsc(U"gv"),.lang_term=tsc(U"glv"),.lang_lib=tsc(U"glv")},.measurement={.measurement=1}};


}
}

#include"../main.h"