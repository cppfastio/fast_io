#pragma once

namespace fast_io
{

template <::std::integral char_type>
using basic_c_io_observer_unlocked = basic_c_family_io_observer<c_family::native_unlocked, char_type>;
template <::std::integral char_type>
using basic_c_io_observer = basic_c_family_io_observer<c_family::native, char_type>;

template <::std::integral char_type>
using basic_c_file_unlocked = basic_c_family_file<c_family::native_unlocked, char_type>;
template <::std::integral char_type>
using basic_c_file = basic_c_family_file<c_family::native, char_type>;

using c_io_observer_unlocked = basic_c_io_observer_unlocked<char>;
using c_io_observer = basic_c_io_observer<char>;
using c_file = basic_c_file<char>;
using c_file_unlocked = basic_c_file_unlocked<char>;
using wc_io_observer_unlocked = basic_c_io_observer_unlocked<wchar_t>;
using wc_io_observer = basic_c_io_observer<wchar_t>;
using wc_file = basic_c_file<wchar_t>;
using wc_file_unlocked = basic_c_file_unlocked<wchar_t>;
using u8c_io_observer_unlocked = basic_c_io_observer_unlocked<char8_t>;
using u8c_io_observer = basic_c_io_observer<char8_t>;
using u8c_file = basic_c_file<char8_t>;
using u8c_file_unlocked = basic_c_file_unlocked<char8_t>;
using u16c_io_observer_unlocked = basic_c_io_observer_unlocked<char16_t>;
using u16c_io_observer = basic_c_io_observer<char16_t>;
using u16c_file = basic_c_file<char16_t>;
using u16c_file_unlocked = basic_c_file_unlocked<char16_t>;
using u32c_io_observer_unlocked = basic_c_io_observer_unlocked<char32_t>;
using u32c_io_observer = basic_c_io_observer<char32_t>;
using u32c_file = basic_c_file<char32_t>;
using u32c_file_unlocked = basic_c_file_unlocked<char32_t>;

using c_file_factory = c_family_file_factory<c_family::native>;
using c_file_factory_unlocked = c_family_file_factory<c_family::native_unlocked>;
} // namespace fast_io
