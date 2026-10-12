module;

/*
fast_io.i18n — the locale module.

The locale container is a flat, position-independent struct image:
every pointer is an lc_rva<T> and every scatter an lc_scatter<T>, so the
bytes are usable from wherever they are found — a file mapping, an
embedded array, a wasm module, a resource. load_l10n is the hosted
loader: it maps the file, checks magic+version, and hands out the
pointer; callers in a sandbox take the pointer directly.

The loader implementation in src/locale/lcblob.cc is compiled into this
module: TLS first, then a global mutex map; mappings live forever;
locale files are opened relative to a single process-lifetime directory
handle, never by string path concatenation.
*/

#include <fast_io.h>
#include <fast_io_i18n.h>

#if !(((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && !defined(_LIBCPP_FREESTANDING)) || \
	   defined(FAST_IO_ENABLE_HOSTED_FEATURES)))
#ifndef FAST_IO_FREESTANDING
#define FAST_IO_FREESTANDING
#endif
#endif

// the loader — compiled inside the module so no separate fast_io_i18n
// library is needed to link
#include "../../src/locale/lcblob.cc"

export module fast_io.i18n;

export import fast_io;

#include "fast_io_inc/i18n.inc"
