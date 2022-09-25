#include<vector>
#include<cstdint>
namespace test
{
template<typename T>
using vector = ::std::vector<T>;
}
#define BENCH_VECTOR_COMMENT_STRING u8"std::vector<T>"
#include"main.h"