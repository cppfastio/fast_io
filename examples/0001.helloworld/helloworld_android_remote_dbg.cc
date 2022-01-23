#include<fast_io.h>
#include<android/log.h>

int main()
{
	print(fast_io::dbg(ANDROID_LOG_FATAL,"fast_io"),
		std::source_location::current(),"\n",
		utc(fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)),"\n"
		"Hello World remote debugger from fast_io\n");
	//it will print to android adb with logcat. Level is FATAL. tag is "fast_io"
}
