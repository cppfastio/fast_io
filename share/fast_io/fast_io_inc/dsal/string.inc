export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::basic_string;
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
using ::fast_io::containers::basic_ostring_ref_fast_io;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
namespace tlc
{
using ::fast_io::tlc::string;
using ::fast_io::tlc::wstring;
using ::fast_io::tlc::u8string;
using ::fast_io::tlc::u16string;
using ::fast_io::tlc::u32string;

using ::fast_io::tlc::concat_fast_io_tlc;
using ::fast_io::tlc::wconcat_fast_io_tlc;
using ::fast_io::tlc::u8concat_fast_io_tlc;
using ::fast_io::tlc::u16concat_fast_io_tlc;
using ::fast_io::tlc::u32concat_fast_io_tlc;

using ::fast_io::tlc::ostring_ref_fast_io_tlc;
using ::fast_io::tlc::wostring_ref_fast_io_tlc;
using ::fast_io::tlc::u8ostring_ref_fast_io_tlc;
using ::fast_io::tlc::u16ostring_ref_fast_io_tlc;
using ::fast_io::tlc::u32ostring_ref_fast_io_tlc;
} // namespace tlc

using ::fast_io::string;
using ::fast_io::wstring;
using ::fast_io::u8string;
using ::fast_io::u16string;
using ::fast_io::u32string;

using ::fast_io::concat_fast_io;
using ::fast_io::wconcat_fast_io;
using ::fast_io::u8concat_fast_io;
using ::fast_io::u16concat_fast_io;
using ::fast_io::u32concat_fast_io;

using ::fast_io::ostring_ref_fast_io;
using ::fast_io::wostring_ref_fast_io;
using ::fast_io::u8ostring_ref_fast_io;
using ::fast_io::u16ostring_ref_fast_io;
using ::fast_io::u32ostring_ref_fast_io;
#endif
} // namespace fast_io