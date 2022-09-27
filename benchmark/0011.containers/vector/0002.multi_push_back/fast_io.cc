#include<fast_io_dsal/vector.h>
namespace test
{
template<typename T>
using vector = ::fast_io::vector<T>;
}
/*
Unfortunately fast_io internally sometimes uses vector despite i try to avoid them. Well then better expose the APIs
*/
#define BENCH_VECTOR_COMMENT_STRING u8"fast_io::vector<T>"
#include"main.h"
