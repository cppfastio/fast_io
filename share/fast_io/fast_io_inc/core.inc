export namespace fast_io
{

using ::fast_io::io_reserve_type_t;
using ::fast_io::io_reserve_type;
using ::fast_io::print_alias_define;
using ::fast_io::print_reserve_size;
using ::fast_io::print_reserve_define;
using ::fast_io::print_define_internal_shift;

namespace manipulators
{
using ::fast_io::manipulators::scalar_placement;
using ::fast_io::manipulators::floating_format;
using ::fast_io::manipulators::lc_time_flag;
using ::fast_io::manipulators::percentage_flag;

using ::fast_io::manipulators::ip_flags;
using ::fast_io::manipulators::ip_default_flags;
using ::fast_io::manipulators::ip_default_inaddr_flags;
using ::fast_io::manipulators::ip_manip_t;
using ::fast_io::manipulators::ip_scan_manip_t;
using ::fast_io::manipulators::ip_scan_default_flags;
using ::fast_io::manipulators::ip_scan_default_inaddr_flags;

using ::fast_io::manipulators::scalar_flags;
using ::fast_io::manipulators::integral_default_scalar_flags;
using ::fast_io::manipulators::floating_point_default_scalar_flags;
using ::fast_io::manipulators::address_default_scalar_flags;
using ::fast_io::manipulators::scalar_manip_t;

using ::fast_io::manipulators::member_function_pointer_holder_t;
using ::fast_io::manipulators::whole_get_t;

using ::fast_io::manipulators::pointervw;
using ::fast_io::manipulators::handlevw;
using ::fast_io::manipulators::methodvw;
using ::fast_io::manipulators::os_c_str;
using ::fast_io::manipulators::addrvw;
using ::fast_io::manipulators::dfvw;

using ::fast_io::manipulators::base;
using ::fast_io::manipulators::bin;
using ::fast_io::manipulators::oct;
using ::fast_io::manipulators::dec;
using ::fast_io::manipulators::hex;
using ::fast_io::manipulators::hexupper;
using ::fast_io::manipulators::hex0x;
using ::fast_io::manipulators::hex0xupper;

using ::fast_io::manipulators::decimal;
using ::fast_io::manipulators::general;
using ::fast_io::manipulators::fixed;
using ::fast_io::manipulators::scientific;
using ::fast_io::manipulators::hexfloat;

using ::fast_io::manipulators::comma_decimal;
using ::fast_io::manipulators::comma_general;
using ::fast_io::manipulators::comma_fixed;
using ::fast_io::manipulators::comma_scientific;
using ::fast_io::manipulators::comma_hexfloat;

using ::fast_io::manipulators::bitfieldvw;
} // namespace manipulators

namespace operations
{
using ::fast_io::operations::write_all;
using ::fast_io::operations::write_all_bytes;
using ::fast_io::operations::write_some;
using ::fast_io::operations::write_some_bytes;
using ::fast_io::operations::pwrite_all;
using ::fast_io::operations::pwrite_all_bytes;
using ::fast_io::operations::pwrite_some;
using ::fast_io::operations::pwrite_some_bytes;
using ::fast_io::operations::scatter_write_all;
using ::fast_io::operations::scatter_write_all_bytes;
using ::fast_io::operations::scatter_write_some;
using ::fast_io::operations::scatter_write_some_bytes;
using ::fast_io::operations::scatter_pwrite_all;
using ::fast_io::operations::scatter_pwrite_all_bytes;
using ::fast_io::operations::scatter_pwrite_some;
using ::fast_io::operations::scatter_pwrite_some_bytes;

using ::fast_io::operations::read_all;
using ::fast_io::operations::read_all_bytes;
using ::fast_io::operations::read_some;
using ::fast_io::operations::read_some_bytes;
using ::fast_io::operations::pread_all;
using ::fast_io::operations::pread_all_bytes;
using ::fast_io::operations::pread_some;
using ::fast_io::operations::pread_some_bytes;
using ::fast_io::operations::scatter_read_all;
using ::fast_io::operations::scatter_read_all_bytes;
using ::fast_io::operations::scatter_read_some;
using ::fast_io::operations::scatter_read_some_bytes;
using ::fast_io::operations::scatter_pread_all;
using ::fast_io::operations::scatter_pread_all_bytes;
using ::fast_io::operations::scatter_pread_some;
using ::fast_io::operations::scatter_pread_some_bytes;

namespace decay
{
using ::fast_io::operations::decay::scan_freestanding_decay;
using ::fast_io::operations::decay::print_freestanding_decay;
} // namespace decay

using ::fast_io::operations::print_freestanding;

} // namespace operations

} // namespace fast_io
