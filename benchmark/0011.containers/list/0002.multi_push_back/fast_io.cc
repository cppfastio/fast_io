#include <fast_io_dsal/list.h>
namespace test
{
template <typename T>
using list = ::fast_io::list<T>;
}
/*
Unfortunately fast_io internally sometimes uses list despite i try to avoid them. Well then better expose the APIs
*/
#define BENCH_LIST_COMMENT_STRING u8"fast_io::list<T>"
#include "main.h"
