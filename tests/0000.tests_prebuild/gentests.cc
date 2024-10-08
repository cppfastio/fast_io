#include <string>
#include <string_view>
#include <vector>
#include <fast_io.h>
#include <fast_io_device.h>
using namespace fast_io::io;

bool gen_cmake_file(fast_io::native_io_observer nio, std::u8string_view prefix);

void cmake_gen_rec_dir_def(fast_io::u8obuf_file& cmake_file, fast_io::native_io_observer nio, std::u8string_view filename, std::u8string_view prefix) {
	std::u8string new_prefix{fast_io::u8concat_std(prefix, filename, u8".")};
	fast_io::dir_file dir{at(nio), filename};
	if (!gen_cmake_file(dir, new_prefix)) return;
	print(cmake_file, u8"include(${CMAKE_CURRENT_LIST_DIR}/", filename, u8"/CMakeLists.txt)\n");
}

void cmake_gen_file_def(fast_io::u8obuf_file& cmake_file, std::u8string_view targetname, auto&& filename) {
	print(cmake_file, u8"add_executable(", targetname, u8" ${CMAKE_CURRENT_LIST_DIR}/", filename, u8")\n");
	print(cmake_file, u8"add_test(", targetname, u8" ", targetname, u8")\n");
}

bool gen_cmake_file(fast_io::native_io_observer nio, std::u8string_view prefix) {
	bool generated{};
	fast_io::u8obuf_file cmake_file;
	try {
		cmake_file = std::move(fast_io::u8obuf_file{at(nio), R"(CMakeLists.txt)", fast_io::open_mode::creat | fast_io::open_mode::excl});
	} catch (...) {
		perr(fast_io::u8err(), u8"CMakeLists.txt alreay exists in ", prefix, u8". \n");
		return true;
	}
	std::vector<std::u8string> ignore_files;
	try {
		fast_io::native_file_loader ignore_file{at(nio), R"(.test_ignore)"};
		fast_io::u8ibuffer_view u8fv{reinterpret_cast<char8_t*>(ignore_file.begin()), reinterpret_cast<char8_t*>(ignore_file.end())};
		for (std::u8string line; scan<true>(u8fv, fast_io::mnp::line_get<char8_t>(line));) {
			line = line.substr(0, line.find_first_of(u8'#'));
#if defined(__cpp_lib_ranges_find_last) && __cpp_lib_ranges_find_last >= 202207L
			line = line.substr(0, std::ranges::find_last_if_not(line, fast_io::char_category::is_c_space<char8_t>).begin() - line.begin() + 1);
#else
			auto rit = std::ranges::find_if_not(line.rbegin(), line.rend(), fast_io::char_category::is_c_space<char8_t>);
			line = line.substr(0, line.rend() - rit);
#endif
			if (line.empty()) continue;
			ignore_files.emplace_back(std::move(line));
		}
	} catch (...) {}
	for (auto const& entry : current(at(nio))) {
		auto filename{std::u8string_view{u8filename(entry)}};
		if (is_dot(entry) || std::ranges::find(ignore_files, filename) != ignore_files.end())
			continue;
		switch (type(entry)) {
			using enum fast_io::file_type;
		case directory:
			cmake_gen_rec_dir_def(cmake_file, nio, filename, prefix);
			generated = true;
			break;
		default:
			std::u8string_view ext{u8extension(entry)};
			if (ext == u8".cpp" || ext == u8".cc") {
				cmake_gen_file_def(cmake_file, fast_io::u8concat_std(prefix, u8stem(entry)), filename);
				generated = true;
			}
			break;
		}
	}
	return generated;
}

int main() {
	fast_io::dir_file tests_dir{R"(./tests)"};
	fast_io::dir_file examples_dir{R"(./examples)"};
	fast_io::dir_file benchmark_dir{R"(./benchmark)"};
	gen_cmake_file(tests_dir, u8"tests.");
	gen_cmake_file(examples_dir, u8"examples.");
	gen_cmake_file(benchmark_dir, u8"benchmark.");
}
