export namespace fast_io
{
namespace freestanding
{

using ::fast_io::freestanding::is_zero_default_constructible_v;
using ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v;

using ::fast_io::freestanding::is_trivially_copyable_or_relocatable;
using ::fast_io::freestanding::is_zero_default_constructible;

using ::fast_io::freestanding::lexicographical_compare_three_way;
using ::fast_io::freestanding::find_if;
using ::fast_io::freestanding::copy_n;
using ::fast_io::freestanding::copy;
using ::fast_io::freestanding::fill;
using ::fast_io::freestanding::fill_n;
using ::fast_io::freestanding::copy_backward;
using ::fast_io::freestanding::move_backward;
using ::fast_io::freestanding::my_mismatch;
using ::fast_io::freestanding::remove;
using ::fast_io::freestanding::my_memcpy;
using ::fast_io::freestanding::my_memmove;
using ::fast_io::freestanding::my_memset;
using ::fast_io::freestanding::my_memcmp;
using ::fast_io::freestanding::non_overlapped_copy_n;
using ::fast_io::freestanding::non_overlapped_copy;
using ::fast_io::freestanding::my_copy_n;
using ::fast_io::freestanding::my_copy;
using ::fast_io::freestanding::my_copy_backward;
using ::fast_io::freestanding::my_compare_iter_n;
using ::fast_io::freestanding::lexicographical_compare;
using ::fast_io::freestanding::equal;
using ::fast_io::freestanding::lower_bound;
using ::fast_io::freestanding::uninitialized_copy;
using ::fast_io::freestanding::uninitialized_copy_n;
using ::fast_io::freestanding::uninitialized_move_n;
using ::fast_io::freestanding::uninitialized_default_construct;
using ::fast_io::freestanding::uninitialized_default_construct_n;
using ::fast_io::freestanding::uninitialized_fill;
using ::fast_io::freestanding::uninitialized_fill_n;
using ::fast_io::freestanding::rotate;
using ::fast_io::freestanding::find_not;
using ::fast_io::freestanding::find_last;
using ::fast_io::freestanding::find_last_not;
using ::fast_io::freestanding::find_first_not_of;
using ::fast_io::freestanding::find_last_of;
using ::fast_io::freestanding::find_last_not_of;

using ::fast_io::freestanding::errc;
} // namespace freestanding
} // namespace fast_io