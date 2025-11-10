#include <chrono>
#include <fast_io.h>

int main()
{
	auto t = ::fast_io::native_thread{[](int param)
#if __cpp_static_call_operator >= 2020207L
								   static
#endif
							   noexcept {
								   ::fast_io::println("the param is: ", param);
#ifdef _WIN32
#ifdef _WIN32_WINDOWS
								   ::fast_io::println("the child thread id is: ", ::fast_io::this_thread::get_id());
#else
								   ::fast_io::println("the child thread id is: ", ::fast_io::mnp::pointervw(::fast_io::this_thread::get_id()));
#endif
#else
								   ::fast_io::println("the child thread id is: ", ::fast_io::this_thread::get_id());
#endif
								   //    ::fflush(stdout);
								   ::fast_io::this_thread::sleep_for(::std::chrono::seconds{1});
							   },
							   5};

	t.join();
#ifdef _WIN32
#ifdef _WIN32_WINDOWS
	::fast_io::println("the parent thread id is: ", ::fast_io::this_thread::get_id());
#else
	::fast_io::println("the parent thread id is: ", ::fast_io::mnp::pointervw(::fast_io::this_thread::get_id()));
#endif
#else
	::fast_io::println("the parent thread id is: ", ::fast_io::this_thread::get_id());
#endif


}
