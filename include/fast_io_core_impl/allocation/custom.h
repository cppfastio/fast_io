#pragma once

namespace fast_io
{

class custom_global_allocator;

#if defined(FAST_IO_DISABLE_CUSTOM_THREAD_LOCAL_ALLOCATOR)
using custom_thread_local_allocator = custom_global_allocator
#else
class custom_thread_local_allocator;
#endif

}
