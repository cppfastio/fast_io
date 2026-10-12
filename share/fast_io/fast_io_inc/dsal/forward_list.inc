export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::forward_list;
using ::fast_io::containers::operator==;
using ::fast_io::containers::operator<=>;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::forward_list;
using ::fast_io::zu_forward_list;

namespace tlc
{
using ::fast_io::tlc::forward_list;
using ::fast_io::tlc::zu_forward_list;
} // namespace tlc
#endif
} // namespace fast_io
