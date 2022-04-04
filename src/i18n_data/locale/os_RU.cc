﻿#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr std::size_t numeric_grouping_storage[]{3};

inline constexpr lc_all lc_all_global{.identification={.name=tsc("os_RU"),.title=tsc("Ossetian locale for Russia"),.source=tsc("fast_io"),.address=tsc("https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc("fast_io"),.email=tsc("bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(""),.fax=tsc(""),.language=tsc("Ossetic"),.territory=tsc("Russia"),.revision=tsc("1.0"),.date=tsc("2000-06-29")},.monetary={.int_curr_symbol=tsc("RUB "),.currency_symbol=tsc("₽"),.mon_decimal_point=tsc(","),.mon_thousands_sep=tsc(" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(""),.negative_sign=tsc("-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(","),.thousands_sep=tsc(" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc("Хцб"),tsc("Крс"),tsc("Дцг"),tsc("Æрт"),tsc("Цпр"),tsc("Мрб"),tsc("Сбт")},.day={tsc("Хуыцаубон"),tsc("Къуырисæр"),tsc("Дыццæг"),tsc("Æртыццæг"),tsc("Цыппæрæм"),tsc("Майрæмбон"),tsc("Сабат")},.abmon={tsc("Янв"),tsc("Фев"),tsc("Мар"),tsc("Апр"),tsc("Май"),tsc("Июн"),tsc("Июл"),tsc("Авг"),tsc("Сен"),tsc("Окт"),tsc("Ноя"),tsc("Дек")},.mon={tsc("январы"),tsc("февралы"),tsc("мартъийы"),tsc("апрелы"),tsc("майы"),tsc("июны"),tsc("июлы"),tsc("августы"),tsc("сентябры"),tsc("октябры"),tsc("ноябры"),tsc("декабры")},.d_t_fmt=tsc("%a %d %b %Y %T"),.d_fmt=tsc("%d.%m.%Y"),.t_fmt=tsc("%T"),.t_fmt_ampm=tsc(""),.date_fmt=tsc("%a %d %b %Y %T %Z"),.am_pm={tsc(""),tsc("")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc("^[+1yYуУ]"),.noexpr=tsc("^[-0nNнН]"),.yesstr=tsc("уойы"),.nostr=tsc("нӕйы")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc("+%c %a %l"),.int_select=tsc("0~10"),.int_prefix=tsc("7")},.name={.name_fmt=tsc("%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc("%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc("Уӕрӕсе"),.country_ab2=tsc("RU"),.country_ab3=tsc("RUS"),.country_num=643,.country_car=tsc("RUS"),.lang_name=tsc("ирон"),.lang_ab=tsc("os"),.lang_term=tsc("oss"),.lang_lib=tsc("oss")},.measurement={.measurement=1}};

inline constexpr wlc_all wlc_all_global{.identification={.name=tsc(L"os_RU"),.title=tsc(L"Ossetian locale for Russia"),.source=tsc(L"fast_io"),.address=tsc(L"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(L"fast_io"),.email=tsc(L"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(L""),.fax=tsc(L""),.language=tsc(L"Ossetic"),.territory=tsc(L"Russia"),.revision=tsc(L"1.0"),.date=tsc(L"2000-06-29")},.monetary={.int_curr_symbol=tsc(L"RUB "),.currency_symbol=tsc(L"₽"),.mon_decimal_point=tsc(L","),.mon_thousands_sep=tsc(L" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(L""),.negative_sign=tsc(L"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(L","),.thousands_sep=tsc(L" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(L"Хцб"),tsc(L"Крс"),tsc(L"Дцг"),tsc(L"Æрт"),tsc(L"Цпр"),tsc(L"Мрб"),tsc(L"Сбт")},.day={tsc(L"Хуыцаубон"),tsc(L"Къуырисæр"),tsc(L"Дыццæг"),tsc(L"Æртыццæг"),tsc(L"Цыппæрæм"),tsc(L"Майрæмбон"),tsc(L"Сабат")},.abmon={tsc(L"Янв"),tsc(L"Фев"),tsc(L"Мар"),tsc(L"Апр"),tsc(L"Май"),tsc(L"Июн"),tsc(L"Июл"),tsc(L"Авг"),tsc(L"Сен"),tsc(L"Окт"),tsc(L"Ноя"),tsc(L"Дек")},.mon={tsc(L"январы"),tsc(L"февралы"),tsc(L"мартъийы"),tsc(L"апрелы"),tsc(L"майы"),tsc(L"июны"),tsc(L"июлы"),tsc(L"августы"),tsc(L"сентябры"),tsc(L"октябры"),tsc(L"ноябры"),tsc(L"декабры")},.d_t_fmt=tsc(L"%a %d %b %Y %T"),.d_fmt=tsc(L"%d.%m.%Y"),.t_fmt=tsc(L"%T"),.t_fmt_ampm=tsc(L""),.date_fmt=tsc(L"%a %d %b %Y %T %Z"),.am_pm={tsc(L""),tsc(L"")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc(L"^[+1yYуУ]"),.noexpr=tsc(L"^[-0nNнН]"),.yesstr=tsc(L"уойы"),.nostr=tsc(L"нӕйы")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(L"+%c %a %l"),.int_select=tsc(L"0~10"),.int_prefix=tsc(L"7")},.name={.name_fmt=tsc(L"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(L"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(L"Уӕрӕсе"),.country_ab2=tsc(L"RU"),.country_ab3=tsc(L"RUS"),.country_num=643,.country_car=tsc(L"RUS"),.lang_name=tsc(L"ирон"),.lang_ab=tsc(L"os"),.lang_term=tsc(L"oss"),.lang_lib=tsc(L"oss")},.measurement={.measurement=1}};

inline constexpr u8lc_all u8lc_all_global{.identification={.name=tsc(u8"os_RU"),.title=tsc(u8"Ossetian locale for Russia"),.source=tsc(u8"fast_io"),.address=tsc(u8"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u8"fast_io"),.email=tsc(u8"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u8""),.fax=tsc(u8""),.language=tsc(u8"Ossetic"),.territory=tsc(u8"Russia"),.revision=tsc(u8"1.0"),.date=tsc(u8"2000-06-29")},.monetary={.int_curr_symbol=tsc(u8"RUB "),.currency_symbol=tsc(u8"₽"),.mon_decimal_point=tsc(u8","),.mon_thousands_sep=tsc(u8" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u8""),.negative_sign=tsc(u8"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u8","),.thousands_sep=tsc(u8" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u8"Хцб"),tsc(u8"Крс"),tsc(u8"Дцг"),tsc(u8"Æрт"),tsc(u8"Цпр"),tsc(u8"Мрб"),tsc(u8"Сбт")},.day={tsc(u8"Хуыцаубон"),tsc(u8"Къуырисæр"),tsc(u8"Дыццæг"),tsc(u8"Æртыццæг"),tsc(u8"Цыппæрæм"),tsc(u8"Майрæмбон"),tsc(u8"Сабат")},.abmon={tsc(u8"Янв"),tsc(u8"Фев"),tsc(u8"Мар"),tsc(u8"Апр"),tsc(u8"Май"),tsc(u8"Июн"),tsc(u8"Июл"),tsc(u8"Авг"),tsc(u8"Сен"),tsc(u8"Окт"),tsc(u8"Ноя"),tsc(u8"Дек")},.mon={tsc(u8"январы"),tsc(u8"февралы"),tsc(u8"мартъийы"),tsc(u8"апрелы"),tsc(u8"майы"),tsc(u8"июны"),tsc(u8"июлы"),tsc(u8"августы"),tsc(u8"сентябры"),tsc(u8"октябры"),tsc(u8"ноябры"),tsc(u8"декабры")},.d_t_fmt=tsc(u8"%a %d %b %Y %T"),.d_fmt=tsc(u8"%d.%m.%Y"),.t_fmt=tsc(u8"%T"),.t_fmt_ampm=tsc(u8""),.date_fmt=tsc(u8"%a %d %b %Y %T %Z"),.am_pm={tsc(u8""),tsc(u8"")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc(u8"^[+1yYуУ]"),.noexpr=tsc(u8"^[-0nNнН]"),.yesstr=tsc(u8"уойы"),.nostr=tsc(u8"нӕйы")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u8"+%c %a %l"),.int_select=tsc(u8"0~10"),.int_prefix=tsc(u8"7")},.name={.name_fmt=tsc(u8"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(u8"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u8"Уӕрӕсе"),.country_ab2=tsc(u8"RU"),.country_ab3=tsc(u8"RUS"),.country_num=643,.country_car=tsc(u8"RUS"),.lang_name=tsc(u8"ирон"),.lang_ab=tsc(u8"os"),.lang_term=tsc(u8"oss"),.lang_lib=tsc(u8"oss")},.measurement={.measurement=1}};

inline constexpr u16lc_all u16lc_all_global{.identification={.name=tsc(u"os_RU"),.title=tsc(u"Ossetian locale for Russia"),.source=tsc(u"fast_io"),.address=tsc(u"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(u"fast_io"),.email=tsc(u"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(u""),.fax=tsc(u""),.language=tsc(u"Ossetic"),.territory=tsc(u"Russia"),.revision=tsc(u"1.0"),.date=tsc(u"2000-06-29")},.monetary={.int_curr_symbol=tsc(u"RUB "),.currency_symbol=tsc(u"₽"),.mon_decimal_point=tsc(u","),.mon_thousands_sep=tsc(u" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(u""),.negative_sign=tsc(u"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(u","),.thousands_sep=tsc(u" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(u"Хцб"),tsc(u"Крс"),tsc(u"Дцг"),tsc(u"Æрт"),tsc(u"Цпр"),tsc(u"Мрб"),tsc(u"Сбт")},.day={tsc(u"Хуыцаубон"),tsc(u"Къуырисæр"),tsc(u"Дыццæг"),tsc(u"Æртыццæг"),tsc(u"Цыппæрæм"),tsc(u"Майрæмбон"),tsc(u"Сабат")},.abmon={tsc(u"Янв"),tsc(u"Фев"),tsc(u"Мар"),tsc(u"Апр"),tsc(u"Май"),tsc(u"Июн"),tsc(u"Июл"),tsc(u"Авг"),tsc(u"Сен"),tsc(u"Окт"),tsc(u"Ноя"),tsc(u"Дек")},.mon={tsc(u"январы"),tsc(u"февралы"),tsc(u"мартъийы"),tsc(u"апрелы"),tsc(u"майы"),tsc(u"июны"),tsc(u"июлы"),tsc(u"августы"),tsc(u"сентябры"),tsc(u"октябры"),tsc(u"ноябры"),tsc(u"декабры")},.d_t_fmt=tsc(u"%a %d %b %Y %T"),.d_fmt=tsc(u"%d.%m.%Y"),.t_fmt=tsc(u"%T"),.t_fmt_ampm=tsc(u""),.date_fmt=tsc(u"%a %d %b %Y %T %Z"),.am_pm={tsc(u""),tsc(u"")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc(u"^[+1yYуУ]"),.noexpr=tsc(u"^[-0nNнН]"),.yesstr=tsc(u"уойы"),.nostr=tsc(u"нӕйы")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(u"+%c %a %l"),.int_select=tsc(u"0~10"),.int_prefix=tsc(u"7")},.name={.name_fmt=tsc(u"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(u"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(u"Уӕрӕсе"),.country_ab2=tsc(u"RU"),.country_ab3=tsc(u"RUS"),.country_num=643,.country_car=tsc(u"RUS"),.lang_name=tsc(u"ирон"),.lang_ab=tsc(u"os"),.lang_term=tsc(u"oss"),.lang_lib=tsc(u"oss")},.measurement={.measurement=1}};

inline constexpr u32lc_all u32lc_all_global{.identification={.name=tsc(U"os_RU"),.title=tsc(U"Ossetian locale for Russia"),.source=tsc(U"fast_io"),.address=tsc(U"https://gitee.com/qabeowjbtkwb/fast_io\t\t;\t\thttps://github.com/cppfastio/fast_io"),.contact=tsc(U"fast_io"),.email=tsc(U"bug-glibc-locales@gnu.org;euloanty@live.com"),.tel=tsc(U""),.fax=tsc(U""),.language=tsc(U"Ossetic"),.territory=tsc(U"Russia"),.revision=tsc(U"1.0"),.date=tsc(U"2000-06-29")},.monetary={.int_curr_symbol=tsc(U"RUB "),.currency_symbol=tsc(U"₽"),.mon_decimal_point=tsc(U","),.mon_thousands_sep=tsc(U" "),.mon_grouping={numeric_grouping_storage,1},.positive_sign=tsc(U""),.negative_sign=tsc(U"-"),.int_frac_digits=2,.frac_digits=2,.p_cs_precedes=0,.p_sep_by_space=1,.n_cs_precedes=0,.n_sep_by_space=1,.p_sign_posn=1,.n_sign_posn=1},.numeric={.decimal_point=tsc(U","),.thousands_sep=tsc(U" "),.grouping={numeric_grouping_storage,1}},.time={.abday={tsc(U"Хцб"),tsc(U"Крс"),tsc(U"Дцг"),tsc(U"Æрт"),tsc(U"Цпр"),tsc(U"Мрб"),tsc(U"Сбт")},.day={tsc(U"Хуыцаубон"),tsc(U"Къуырисæр"),tsc(U"Дыццæг"),tsc(U"Æртыццæг"),tsc(U"Цыппæрæм"),tsc(U"Майрæмбон"),tsc(U"Сабат")},.abmon={tsc(U"Янв"),tsc(U"Фев"),tsc(U"Мар"),tsc(U"Апр"),tsc(U"Май"),tsc(U"Июн"),tsc(U"Июл"),tsc(U"Авг"),tsc(U"Сен"),tsc(U"Окт"),tsc(U"Ноя"),tsc(U"Дек")},.mon={tsc(U"январы"),tsc(U"февралы"),tsc(U"мартъийы"),tsc(U"апрелы"),tsc(U"майы"),tsc(U"июны"),tsc(U"июлы"),tsc(U"августы"),tsc(U"сентябры"),tsc(U"октябры"),tsc(U"ноябры"),tsc(U"декабры")},.d_t_fmt=tsc(U"%a %d %b %Y %T"),.d_fmt=tsc(U"%d.%m.%Y"),.t_fmt=tsc(U"%T"),.t_fmt_ampm=tsc(U""),.date_fmt=tsc(U"%a %d %b %Y %T %Z"),.am_pm={tsc(U""),tsc(U"")},.week={7,19971130,1},.first_weekday=2},.messages={.yesexpr=tsc(U"^[+1yYуУ]"),.noexpr=tsc(U"^[-0nNнН]"),.yesstr=tsc(U"уойы"),.nostr=tsc(U"нӕйы")},.paper={.width=210,.height=297},.telephone={.tel_int_fmt=tsc(U"+%c %a %l"),.int_select=tsc(U"0~10"),.int_prefix=tsc(U"7")},.name={.name_fmt=tsc(U"%d%t%g%t%m%t%f")},.address={.postal_fmt=tsc(U"%f%N%a%N%d%N%b%N%s %h %e %r%N%z %T%N%c%N"),.country_name=tsc(U"Уӕрӕсе"),.country_ab2=tsc(U"RU"),.country_ab3=tsc(U"RUS"),.country_num=643,.country_car=tsc(U"RUS"),.lang_name=tsc(U"ирон"),.lang_ab=tsc(U"os"),.lang_term=tsc(U"oss"),.lang_lib=tsc(U"oss")},.measurement={.measurement=1}};


}
}

#include"../main.h"