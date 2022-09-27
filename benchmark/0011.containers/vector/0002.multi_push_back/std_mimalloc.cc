#include<vector>
#include<cstdint>
#include<mimalloc-2.0/mimalloc.h>
namespace test
{
template<typename T>
using vector = ::std::vector<T,mi_stl_allocator<T>>;
}
#define BENCH_VECTOR_COMMENT_STRING u8"std::vector<T,mi_stl_allocator<T>>"
#include"main.h"
