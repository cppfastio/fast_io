export namespace fast_io
{
using ::fast_io::basic_io_scatter_t;
using ::fast_io::char_literal_v;
using ::fast_io::pr_rsv_size;
using ::fast_io::pr_rsv_to_iterator_unchecked;

using ::fast_io::io_alias_type_t;

using ::fast_io::io_reserve_type_t;
using ::fast_io::io_reserve_type;
using ::fast_io::print_alias_define;
using ::fast_io::print_reserve_size;
using ::fast_io::print_reserve_define;
using ::fast_io::print_define_internal_shift;

using ::fast_io::print_scatter_define;

using ::fast_io::scan_contiguous_define;
using ::fast_io::scan_context_type;
using ::fast_io::scan_context_define;
using ::fast_io::scan_context_eof_define;

using ::fast_io::unreachable;
using ::fast_io::noexcept_call;

using ::fast_io::little_endian;
using ::fast_io::big_endian;
using ::fast_io::byte_swap;

using ::fast_io::error;
using ::fast_io::error_type_t;
using ::fast_io::error_type;
using ::fast_io::error_domain;
using ::fast_io::is_domain;
using ::fast_io::to_code;
using ::fast_io::throw_parse_code;
using ::fast_io::parse_domain_value;

using ::fast_io::cstr_len;
using ::fast_io::cstr_nlen;

using ::fast_io::parse_by_scan;
using ::fast_io::parse_code;

using ::fast_io::io_dup;
using ::fast_io::open_mode;

using ::fast_io::basic_io_lockable;
using ::fast_io::basic_io_lockable_nonmovable;

#if !defined(__clang__) // The gcc and msvc module does not support non-exported functions adl matching
using ::fast_io::input_stream_ref_define;
using ::fast_io::output_stream_ref_define;
using ::fast_io::io_stream_ref_define;

using ::fast_io::input_decorators_ref_define;
using ::fast_io::output_decorators_ref_define;
// using ::fast_io::io_decorators_ref_define;

using ::fast_io::ibuffer_begin;
using ::fast_io::ibuffer_curr;
using ::fast_io::ibuffer_end;
using ::fast_io::ibuffer_set_curr;
// using ::fast_io::status_scan_define;
using ::fast_io::read_some_underflow_define;
// using ::fast_io::read_all_underflow_define;
using ::fast_io::read_some_bytes_underflow_define;
#ifdef _WIN32
using ::fast_io::read_all_bytes_underflow_define;
#endif
// using ::fast_io::scatter_read_some_bytes_underflow_define;
// using ::fast_io::scatter_read_all_bytes_underflow_define;
using ::fast_io::scatter_read_some_underflow_define;
//  using ::fast_io::scatter_read_all_underflow_define;
// using ::fast_io::ibuffer_underflow_never_define;
using ::fast_io::pread_some_bytes_underflow_define;
// using ::fast_io::pread_all_bytes_underflow_define;
using ::fast_io::scatter_pread_some_bytes_underflow_define;
// using ::fast_io::scatter_pread_all_bytes_underflow_define;
using ::fast_io::pread_some_underflow_define;
using ::fast_io::pread_all_underflow_define;
using ::fast_io::scatter_pread_some_underflow_define;
using ::fast_io::scatter_pread_all_underflow_define;
using ::fast_io::ibuffer_minimum_size_define;
using ::fast_io::ibuffer_minimum_size_underflow_all_prepare_define;
// using ::fast_io::input_stream_mutex_ref_define;
// using ::fast_io::output_stream_mutex_ref_define;
using ::fast_io::io_stream_mutex_ref_define;
using ::fast_io::input_stream_unlocked_ref_define;
using ::fast_io::output_stream_unlocked_ref_define;
using ::fast_io::io_stream_unlocked_ref_define;
using ::fast_io::obuffer_begin;
using ::fast_io::obuffer_curr;
using ::fast_io::obuffer_end;
using ::fast_io::obuffer_set_curr;
// using ::fast_io::status_print_define;
using ::fast_io::write_some_overflow_define;
using ::fast_io::write_all_overflow_define;
using ::fast_io::write_some_bytes_overflow_define;
using ::fast_io::write_all_bytes_overflow_define;
// using ::fast_io::scatter_write_some_bytes_overflow_define;
// using ::fast_io::scatter_write_all_bytes_overflow_define;
using ::fast_io::scatter_write_some_overflow_define;
using ::fast_io::scatter_write_all_overflow_define;
// using ::fast_io::obuffer_overflow_never_define;
// using ::fast_io::output_stream_char_put_overflow_define;
using ::fast_io::pwrite_some_bytes_overflow_define;
using ::fast_io::pwrite_all_bytes_overflow_define;
using ::fast_io::scatter_pwrite_some_bytes_overflow_define;
// using ::fast_io::scatter_pwrite_all_bytes_overflow_define;
using ::fast_io::pwrite_some_overflow_define;
using ::fast_io::pwrite_all_overflow_define;
using ::fast_io::scatter_pwrite_some_overflow_define;
using ::fast_io::scatter_pwrite_all_overflow_define;
// using ::fast_io::obuffer_is_line_buffering_define;
using ::fast_io::input_stream_require_secure_clear_define;
// using ::fast_io::output_stream_require_secure_clear_define;
using ::fast_io::io_stream_require_secure_clear_define;
// using ::fast_io::input_stream_buffer_flush_define;
using ::fast_io::output_stream_buffer_flush_define;
using ::fast_io::io_stream_buffer_flush_define;
#endif

namespace manipulators
{
    
using ::fast_io::manipulators::print_alias_define;
using ::fast_io::manipulators::print_reserve_size;
using ::fast_io::manipulators::print_reserve_define;

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

using ::fast_io::manipulators::leb128_get;
using ::fast_io::manipulators::leb128_put;

using ::fast_io::manipulators::boolalpha;

using ::fast_io::manipulators::pointervw;
using ::fast_io::manipulators::handlevw;
using ::fast_io::manipulators::methodvw;
using ::fast_io::manipulators::addrvw;
using ::fast_io::manipulators::dfvw;

using ::fast_io::manipulators::os_c_str;
using ::fast_io::manipulators::os_c_str_carr;

using ::fast_io::manipulators::rgvw;

using ::fast_io::manipulators::cond;

using ::fast_io::manipulators::code_cvt;
using ::fast_io::manipulators::code_cvt_os_c_str;
using ::fast_io::manipulators::os_c_str_null_terminated;
using ::fast_io::manipulators::os_c_str_null_terminated_carr;
using ::fast_io::manipulators::os_c_str_or_nullptr;
using ::fast_io::manipulators::os_c_str_with_known_size;

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

using ::fast_io::manipulators::width;
using ::fast_io::manipulators::left;
using ::fast_io::manipulators::middle;
using ::fast_io::manipulators::right;
using ::fast_io::manipulators::internal;

using ::fast_io::manipulators::bitfieldvw;

using ::fast_io::manipulators::ip_generic;
#if 0
using ::fast_io::manipulators::ip_scan_generic;
#endif
using ::fast_io::manipulators::cur_src_loc;
} // namespace manipulators

namespace mnp = manipulators;

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

using ::fast_io::operations::output_stream_ref;

namespace decay
{
using ::fast_io::operations::decay::scan_freestanding_decay;
using ::fast_io::operations::decay::print_freestanding_decay;

using ::fast_io::operations::decay::stream_ref_decay_lock_guard;
using ::fast_io::operations::decay::unlock_stream_ref_decay_lock_guard;
using ::fast_io::operations::decay::output_stream_mutex_ref_decay;
using ::fast_io::operations::decay::output_stream_unlocked_ref_decay;

} // namespace decay

using ::fast_io::operations::print_freestanding;

} // namespace operations

// filesystem
using ::fast_io::access_how;
using ::fast_io::file_lock_mode;
using ::fast_io::mmap_prot;
using ::fast_io::mmap_flags;
using ::fast_io::utime_flags;
using ::fast_io::unix_timestamp_option;
using ::fast_io::well_known_directory;
// flags enum
using ::fast_io::operator&;
using ::fast_io::operator|;
using ::fast_io::operator^;
using ::fast_io::operator~;
using ::fast_io::operator&=;
using ::fast_io::operator|=;
using ::fast_io::operator^=;

namespace intrinsics
{
// simd
using ::fast_io::intrinsics::simd_vector;
using ::fast_io::intrinsics::optimal_simd_vector_run_with_cpu_instruction_size;
using ::fast_io::intrinsics::is_all_zeros;
using ::fast_io::intrinsics::can_intrinsics_accelerate_mask_countr;
using ::fast_io::intrinsics::vector_mask_countr_one;
using ::fast_io::intrinsics::vector_mask_countr_zero;

using ::fast_io::intrinsics::operator+;
using ::fast_io::intrinsics::operator-;
using ::fast_io::intrinsics::operator*;
using ::fast_io::intrinsics::operator/;
using ::fast_io::intrinsics::operator&;
using ::fast_io::intrinsics::operator|;
using ::fast_io::intrinsics::operator^;
using ::fast_io::intrinsics::operator<<;
using ::fast_io::intrinsics::operator>>;
using ::fast_io::intrinsics::operator<;
using ::fast_io::intrinsics::operator>;
using ::fast_io::intrinsics::operator<=;
using ::fast_io::intrinsics::operator>=;
using ::fast_io::intrinsics::operator==;
using ::fast_io::intrinsics::operator!=;
using ::fast_io::intrinsics::wrap_add;
using ::fast_io::intrinsics::wrap_sub;

} // namespace intrinsics

namespace bitops
{
// remove trailing zeros
using ::fast_io::bitops::rtz_iec559;
using ::fast_io::bitops::rtz;
} // namespace bitops

// network
using ::fast_io::posix_empty_network_service;
using ::fast_io::sock_family;
using ::fast_io::sock_protocol;
using ::fast_io::sock_type;
using ::fast_io::posix_sockaddr;
using ::fast_io::posix_in_addr;
using ::fast_io::posix_sockaddr_in;
using ::fast_io::posix_in6_addr;
using ::fast_io::posix_sockaddr_in6;
using ::fast_io::posix_sockaddr_storage;
#if 0
using ::fast_io::scan_ipv4_context_phase;
using ::fast_io::scan_ipv6_context_phase;
using ::fast_io::ip_scan_state_t;
using ::fast_io::ipv4_scan_state_t;
using ::fast_io::ipv6_scan_state_t;
using ::fast_io::ip_port_scan_state_t;
#endif

using ::fast_io::basic_timestamp;
using ::fast_io::unix_timestamp;
using ::fast_io::win32_timestamp;
using ::fast_io::csharp_timestamp;
using ::fast_io::year0_timestamp;
using ::fast_io::universe_timestamp;

using ::fast_io::operator==;
using ::fast_io::operator!=;
using ::fast_io::operator<=>;
using ::fast_io::operator+;
using ::fast_io::operator-;
using ::fast_io::operator/;
using ::fast_io::operator+=;
using ::fast_io::operator-=;
using ::fast_io::operator/=;
using ::fast_io::operator*;
using ::fast_io::operator~;
using ::fast_io::operator&;
using ::fast_io::operator|;
using ::fast_io::operator^;
using ::fast_io::operator&=;
using ::fast_io::operator|=;
using ::fast_io::operator^=;

using ::fast_io::operator++;

} // namespace fast_io
