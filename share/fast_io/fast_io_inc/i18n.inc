export namespace fast_io
{

// the wire structs and the resolver helpers — freestanding-safe:
// a caller-supplied lc_locale pointer works everywhere
namespace l10n
{
using ::fast_io::l10n::lc_rva;
using ::fast_io::l10n::lc_scatter;
using ::fast_io::l10n::lc_u32;
using ::fast_io::l10n::lc_s32;
using ::fast_io::l10n::lc_get_rva;
using ::fast_io::l10n::lc_get_scatter;
using ::fast_io::l10n::lc_get_all;
using ::fast_io::l10n::magic;
using ::fast_io::l10n::blob_version;
using ::fast_io::l10n::lc_locale;
using ::fast_io::l10n::locale_charset;
using ::fast_io::l10n::locale_charset_name;
using ::fast_io::l10n::basic_lc_identification;
using ::fast_io::l10n::basic_lc_monetary;
using ::fast_io::l10n::basic_lc_numeric;
using ::fast_io::l10n::basic_lc_time_era;
using ::fast_io::l10n::basic_lc_time;
using ::fast_io::l10n::basic_lc_messages;
using ::fast_io::l10n::basic_lc_paper;
using ::fast_io::l10n::basic_lc_telephone;
using ::fast_io::l10n::basic_lc_name;
using ::fast_io::l10n::basic_lc_address;
using ::fast_io::l10n::basic_lc_measurement;
using ::fast_io::l10n::basic_lc_keyboard;
using ::fast_io::l10n::basic_lc_all;
using ::fast_io::l10n::l10n_load_flags;
#ifndef FAST_IO_FREESTANDING
using ::fast_io::l10n::load_l10n;
#endif
} // namespace l10n

// the formatting layer — imbue/lc_* against any lc_locale pointer
using ::fast_io::lc_ctx;
using ::fast_io::lc_load_ctx;
using ::fast_io::lc_imbuer;
using ::fast_io::imbue;
using ::fast_io::basic_lc_general_concat;
using ::fast_io::basic_lc_concat;
using ::fast_io::lc_concat;
using ::fast_io::lc_concatln;

namespace manipulators
{
using ::fast_io::manipulators::lc_dump_t;
using ::fast_io::manipulators::lc_dump;
} // namespace manipulators

} // namespace fast_io
