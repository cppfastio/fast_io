#include <fast_io.h>

thread_local fast_io::timestamp_scan_state_t<char> state{};

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr, std::size_t n) noexcept
{
	fast_io::iso8601_timestamp tsp{};
	auto begin{ reinterpret_cast<char const*>(ptr) };
	auto end{ begin + n };
	for (;;) {
		auto ret{ ::fast_io::details::scn_ctx_define_iso8601_impl<false>(state, begin, end, tsp) };
		if (ret.code != fast_io::parse_code::partial)
			break;
	}
	return 0;
}