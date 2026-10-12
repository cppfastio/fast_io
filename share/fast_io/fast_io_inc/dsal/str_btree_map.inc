export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::basic_str_btree_map;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::basic_str_btree_map;
using ::fast_io::str_btree_map;
using ::fast_io::wstr_btree_map;
using ::fast_io::u8str_btree_map;
using ::fast_io::u16str_btree_map;
using ::fast_io::u32str_btree_map;
using ::fast_io::zu_str_btree_map;

namespace tlc
{
using ::fast_io::tlc::basic_str_btree_map;
using ::fast_io::tlc::str_btree_map;
using ::fast_io::tlc::wstr_btree_map;
using ::fast_io::tlc::u8str_btree_map;
using ::fast_io::tlc::u16str_btree_map;
using ::fast_io::tlc::u32str_btree_map;
using ::fast_io::tlc::zu_str_btree_map;
} // namespace tlc
#endif
} // namespace fast_io
