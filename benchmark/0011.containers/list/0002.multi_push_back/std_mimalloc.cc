#include <list>
#include <cstdint>
#include <mimalloc-2.0/mimalloc.h>
namespace test
{
template <typename T>
using list = ::std::list<T, mi_stl_allocator<T>>;
}
#define BENCH_LIST_COMMENT_STRING u8"std::list<T,mi_stl_allocator<T>>"
#include "main.h"
