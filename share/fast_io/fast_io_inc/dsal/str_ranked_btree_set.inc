export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::basic_str_ranked_btree_set;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::basic_str_ranked_btree_set;
using ::fast_io::str_ranked_btree_set;
using ::fast_io::wstr_ranked_btree_set;
using ::fast_io::u8str_ranked_btree_set;
using ::fast_io::u16str_ranked_btree_set;
using ::fast_io::u32str_ranked_btree_set;

namespace tlc
{
using ::fast_io::tlc::basic_str_ranked_btree_set;
using ::fast_io::tlc::str_ranked_btree_set;
using ::fast_io::tlc::wstr_ranked_btree_set;
using ::fast_io::tlc::u8str_ranked_btree_set;
using ::fast_io::tlc::u16str_ranked_btree_set;
using ::fast_io::tlc::u32str_ranked_btree_set;
} // namespace tlc
#endif
} // namespace fast_io
