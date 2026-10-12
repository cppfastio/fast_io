#if defined(__HERBCEPTIONS__)
// Re-export <herbceptions/error> so module users get std::error and the
// throwable errc domains transparently without including the header.
export namespace std
{
using ::std::error;
using ::std::error_domain;
using ::std::error_domain_singleton;
using ::std::io_scatter_t;
using ::std::error_reporter_encoding;
using ::std::error_reporter_io_cookie_function;
using ::std::error_query_information;
using ::std::legacy_exception_abi;
using ::std::errc;
using ::std::cmath_errc;
using ::std::parse_errc;

using ::std::operator==;
} // namespace std

export namespace fast_io::manipulators
{
using ::fast_io::manipulators::herbceptions_query_information;
using ::fast_io::manipulators::name;
using ::fast_io::manipulators::message;
using ::fast_io::manipulators::name_message;
using ::fast_io::manipulators::posix_equivalent_name;
using ::fast_io::manipulators::posix_equivalent_message;
using ::fast_io::manipulators::posix_equivalent_name_message;
} // namespace fast_io::manipulators
#endif
