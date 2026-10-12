export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::basic_str_swiss_set;
using ::fast_io::containers::swap;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::basic_str_swiss_set;
using ::fast_io::str_swiss_set;
using ::fast_io::wstr_swiss_set;
using ::fast_io::u8str_swiss_set;
using ::fast_io::u16str_swiss_set;
using ::fast_io::u32str_swiss_set;

namespace tlc
{
using ::fast_io::tlc::basic_str_swiss_set;
using ::fast_io::tlc::str_swiss_set;
using ::fast_io::tlc::wstr_swiss_set;
using ::fast_io::tlc::u8str_swiss_set;
using ::fast_io::tlc::u16str_swiss_set;
using ::fast_io::tlc::u32str_swiss_set;
} // namespace tlc
#endif
} // namespace fast_io
