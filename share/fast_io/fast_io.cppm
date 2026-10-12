module;

#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/tuple.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/sized_vector.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/sized_string.h>
#include <fast_io_dsal/list.h>
#include <fast_io_dsal/forward_list.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/queue.h>
#include <fast_io_dsal/priority_queue.h>
#include <fast_io_dsal/stack.h>
#include <fast_io_dsal/bitvec.h>
#include <fast_io_dsal/span.h>
#include <fast_io_dsal/index_span.h>
#include <fast_io_dsal/str_swiss_set.h>
#include <fast_io_dsal/str_swiss_map.h>
#include <fast_io_dsal/str_btree_set.h>
#include <fast_io_dsal/str_btree_map.h>
#include <fast_io_dsal/str_ranked_btree_set.h>
#include <fast_io_dsal/str_ranked_btree_map.h>

#if !(((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && !defined(_LIBCPP_FREESTANDING)) || \
	   defined(FAST_IO_ENABLE_HOSTED_FEATURES)))
#ifndef FAST_IO_FREESTANDING
#define FAST_IO_FREESTANDING
#endif
#endif

export module fast_io;

#include "fast_io_inc/herbceptions.inc"
#include "fast_io_inc/core.inc"
#include "fast_io_inc/core/allocation.inc"
#include "fast_io_inc/freestanding.inc"
#include "fast_io_inc/intrinsics.inc"

#ifndef FAST_IO_FREESTANDING
#include "fast_io_inc/hosted.inc"

#include "fast_io_inc/hosted/posix.inc"

#if defined(_WIN32) || defined(__CYGWIN__)
#include "fast_io_inc/hosted/nt.inc"
#include "fast_io_inc/hosted/win32.inc"
#endif

#include "fast_io_inc/legacy/c.inc"
#include "fast_io_inc/device.inc"
#include "fast_io_inc/io_buffer.inc"
#endif

/*
io functions
*/
#include "fast_io_inc/io.inc"

/*
containers
*/
#include "fast_io_inc/dsal/array.inc"
#include "fast_io_inc/dsal/tuple.inc"
#include "fast_io_inc/dsal/vector.inc"
#include "fast_io_inc/dsal/sized_vector.inc"
#include "fast_io_inc/dsal/string_view.inc"
#include "fast_io_inc/dsal/string.inc"
#include "fast_io_inc/dsal/sized_string.inc"
#include "fast_io_inc/dsal/list.inc"
#include "fast_io_inc/dsal/forward_list.inc"
#include "fast_io_inc/dsal/deque.inc"
#include "fast_io_inc/dsal/queue.inc"
#include "fast_io_inc/dsal/priority_queue.inc"
#include "fast_io_inc/dsal/stack.inc"
#include "fast_io_inc/dsal/bitvec.inc"
#include "fast_io_inc/dsal/span.inc"
#include "fast_io_inc/dsal/index_span.inc"
#include "fast_io_inc/dsal/str_swiss_set.inc"
#include "fast_io_inc/dsal/str_swiss_map.inc"
#include "fast_io_inc/dsal/str_btree_set.inc"
#include "fast_io_inc/dsal/str_btree_map.inc"
#include "fast_io_inc/dsal/str_ranked_btree_set.inc"
#include "fast_io_inc/dsal/str_ranked_btree_map.inc"

/*
details
*/
#include "fast_io_inc/detail.inc"
