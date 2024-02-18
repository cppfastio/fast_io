#include <list>
#include <cstdint>
namespace test
{
template <typename T>
using list = ::std::list<T>;
}
#define BENCH_LIST_COMMENT_STRING u8"std::list<T>"
#include "main.h"