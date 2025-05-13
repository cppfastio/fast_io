export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::vector;
using ::fast_io::containers::operator==;
using ::fast_io::containers::operator<=>;
using ::fast_io::containers::swap;
using ::fast_io::containers::erase;
using ::fast_io::containers::erase_if;

namespace freestanding
{
using ::fast_io::freestanding::is_trivially_copyable_or_relocatable;
using ::fast_io::freestanding::is_zero_default_constructible;
} // namespace freestanding

} // namespace containers

#ifndef FAST_IO_FREESTANDING
namespace tlc
{
using ::fast_io::tlc::vector;
}

using ::fast_io::vector;
#endif
} // namespace fast_io
