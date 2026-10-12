export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::bitvec;
using ::fast_io::containers::operator==;
using ::fast_io::containers::operator<=>;
using ::fast_io::containers::swap;
using ::fast_io::containers::operator&;
using ::fast_io::containers::operator|;
using ::fast_io::containers::operator^;
using ::fast_io::containers::operator<<;
using ::fast_io::containers::operator>>;
using ::fast_io::containers::bitvec_rotl;
using ::fast_io::containers::bitvec_rotr;
using ::fast_io::containers::bitvec_bit_floor;
using ::fast_io::containers::bitvec_bit_ceil;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::bitvec;

namespace tlc
{
using ::fast_io::tlc::bitvec;
} // namespace tlc
#endif
} // namespace fast_io
