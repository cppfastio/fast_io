export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::basic_sized_string;
using ::fast_io::containers::operator==;
using ::fast_io::containers::operator<=>;
using ::fast_io::containers::swap;
using ::fast_io::containers::erase;
using ::fast_io::containers::erase_if;
using ::fast_io::containers::print_alias_define;
using ::fast_io::containers::strlike_construct_define;
using ::fast_io::containers::strlike_construct_single_character_define;
using ::fast_io::containers::strlike_begin;
using ::fast_io::containers::strlike_curr;
using ::fast_io::containers::strlike_end;
using ::fast_io::containers::strlike_set_curr;
using ::fast_io::containers::strlike_reserve;
using ::fast_io::containers::io_strlike_ref;
using ::fast_io::containers::basic_sized_ostring_ref_fast_io;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::basic_sized_string;
using ::fast_io::basic_sized_string_zu32;
using ::fast_io::sized_string_zu32;
using ::fast_io::wsized_string_zu32;
using ::fast_io::u8sized_string_zu32;
using ::fast_io::u16sized_string_zu32;
using ::fast_io::u32sized_string_zu32;

using ::fast_io::basic_concat_fast_io_sized;
using ::fast_io::basic_concat_fast_io_sized_zu32;
using ::fast_io::concat_fast_io_sized_zu32;
using ::fast_io::wconcat_fast_io_sized_zu32;
using ::fast_io::u8concat_fast_io_sized_zu32;
using ::fast_io::u16concat_fast_io_sized_zu32;
using ::fast_io::u32concat_fast_io_sized_zu32;
using ::fast_io::concatln_fast_io_sized_zu32;
using ::fast_io::wconcatln_fast_io_sized_zu32;
using ::fast_io::u8concatln_fast_io_sized_zu32;
using ::fast_io::u16concatln_fast_io_sized_zu32;
using ::fast_io::u32concatln_fast_io_sized_zu32;

namespace tlc
{
using ::fast_io::tlc::basic_sized_string;
using ::fast_io::tlc::basic_sized_string_zu32;
using ::fast_io::tlc::sized_string_zu32;
using ::fast_io::tlc::wsized_string_zu32;
using ::fast_io::tlc::u8sized_string_zu32;
using ::fast_io::tlc::u16sized_string_zu32;
using ::fast_io::tlc::u32sized_string_zu32;

using ::fast_io::tlc::basic_concat_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::concat_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::wconcat_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::u8concat_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::u16concat_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::u32concat_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::concatln_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::wconcatln_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::u8concatln_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::u16concatln_fast_io_sized_zu32_tlc;
using ::fast_io::tlc::u32concatln_fast_io_sized_zu32_tlc;
} // namespace tlc
#endif
} // namespace fast_io
