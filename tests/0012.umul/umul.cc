#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	std::uint64_t high;
	std::uint64_t low=fast_io::details::intrinsics::umul(7941694176621304320ULL,11235582092889474423ULL,high);
	print(low," ",high);
}
