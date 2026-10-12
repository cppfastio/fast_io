#include <fast_io.h>
#include <random>
#include <exception>
#include <system_error>

namespace wh_test
{

// Minimal input-only white-hole handle whose read always fails.
class failing_white_hole_device
{
public:
	using input_char_type = char;
	constexpr void close() noexcept
	{}
};

inline constexpr failing_white_hole_device input_stream_ref_define(failing_white_hole_device dev) noexcept
{
	return dev;
}

inline void read_all_bytes_underflow_define(failing_white_hole_device, ::std::byte *, ::std::size_t)
#if defined(__HERBCEPTIONS__)
	throws
#endif
{
#if defined(__HERBCEPTIONS__)
	throw throws::std::errc::io_error;
#else
	throw ::std::system_error(::std::make_error_code(::std::errc::io_error));
#endif
}

} // namespace wh_test

int main()
{
	::fast_io::basic_white_hole_engine<::wh_test::failing_white_hole_device> eng{};
	::std::uniform_int_distribution<int> dis{0, 100};

	::fast_io::perrln("noexcept(eng()) = ", noexcept(eng()));

	::fast_io::perr("--- eng() direct ---\n");
	try
	{
		::fast_io::perrln("eng() -> ", eng());
	}
	catch (::std::exception const &e)
	{
		::fast_io::perrln("eng() caught std::exception what() = ", ::fast_io::mnp::os_c_str(e.what()));
	}
	catch (...)
	{
		::fast_io::perr("eng() caught non-std::exception\n");
	}

	::fast_io::perr("--- dis(eng) ---\n");
	try
	{
		::fast_io::perrln("dis(eng) -> ", dis(eng));
	}
	catch (::std::exception const &e)
	{
		::fast_io::perrln("dis(eng) caught std::exception what() = ", ::fast_io::mnp::os_c_str(e.what()));
	}
	catch (...)
	{
		::fast_io::perr("dis(eng) caught non-std::exception\n");
	}

#if defined(__HERBCEPTIONS__)
	::fast_io::perr("--- eng.generate() raw ---\n");
	try
	{
		::fast_io::perrln("generate() -> ", eng.generate());
	}
	catch throws(::std::error e)
	{
		::fast_io::perrln("generate() caught herbception: ", ::fast_io::manipulators::name_message(e));
	}
#endif

	::fast_io::perr("--- fast_io::random_generate(dis, eng) ---\n");
	try
	{
		::fast_io::perrln("random_generate -> ", ::fast_io::random_generate(dis, eng));
	}
#if defined(__HERBCEPTIONS__)
	catch throws(::std::error e)
	{
		::fast_io::perrln(e);
	}
#endif
	catch (::std::exception const &e)
	{
		::fast_io::perrln("random_generate caught std::exception what() = ", ::fast_io::mnp::os_c_str(e.what()));
	}
	catch (...)
	{
		::fast_io::perr("random_generate caught non-std::exception\n");
	}
	return 0;
}
