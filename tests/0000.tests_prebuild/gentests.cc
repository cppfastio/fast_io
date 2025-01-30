/*
 * Generate CMakeLists.txt for tests, examples and benchmarks
 *
 * test properties are read from .test_prop.toml file in the directory
 * supported properties: refer to `file_entry_t`
 */

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <fast_io.h>
#include <fast_io_device.h>
using namespace fast_io::io;

#undef windows
#undef linux
#undef macos

enum class will_fail_t : std::uint8_t
{
	never = 0,
	compiletime,
	runtime,
};
enum class platform_t : std::uint8_t
{
	all = 0,
	windows = 1 << 0,
	linux = 1 << 1,
	macos = 1 << 2,
};
struct file_entry_t
{
	bool ignore{};
	bool interactive{};
	bool asan{};
	will_fail_t will_fail{};
	platform_t platform{};
};
inline constexpr will_fail_t operator|(will_fail_t const &lhs, will_fail_t const &rhs)
{
	if (lhs == will_fail_t::never)
	{
		return rhs;
	}
	else if (lhs == will_fail_t::compiletime)
	{
		return will_fail_t::compiletime;
	}
	else
	{
		if (rhs == will_fail_t::compiletime)
		{
			return will_fail_t::compiletime;
		}
		else
		{
			return will_fail_t::runtime;
		}
	}
}
inline constexpr platform_t operator|(platform_t const &lhs, platform_t const &rhs)
{
	return static_cast<platform_t>(static_cast<std::underlying_type_t<platform_t>>(lhs) | static_cast<std::underlying_type_t<platform_t>>(rhs));
}
inline constexpr bool intersects(platform_t const &lhs, platform_t const &rhs)
{
	return static_cast<std::underlying_type_t<platform_t>>(lhs) & static_cast<std::underlying_type_t<platform_t>>(rhs);
}
inline constexpr file_entry_t operator|(file_entry_t const &lhs, file_entry_t const &rhs)
{
	return {
		.ignore = lhs.ignore || rhs.ignore,
		.interactive = lhs.interactive || rhs.interactive,
		.asan = lhs.asan || rhs.asan,
		.will_fail = lhs.will_fail | rhs.will_fail,
		.platform = lhs.platform | rhs.platform};
}
using file_property_t = std::unordered_map<std::u8string, file_entry_t>;

inline platform_t global_platform;
inline bool msvc{};

inline void parse_prop_files(fast_io::native_file_loader &&file, file_property_t &file_properties)
{
	std::unordered_map<std::u8string, std::unordered_map<std::u8string, std::u8string>> file_contents;

	fast_io::u8ibuffer_view u8fv{reinterpret_cast<char8_t *>(file.begin()), reinterpret_cast<char8_t *>(file.end())};
	std::unordered_map<std::u8string, std::u8string> *curr_entry{};
	for (std::u8string line; scan<true>(u8fv, fast_io::mnp::line_get<char8_t>(line));)
	{
		std::u8string_view linevw{line};
		linevw = linevw.substr(std::ranges::find_if_not(linevw, fast_io::char_category::is_c_space<char8_t>) - linevw.begin());
		linevw = linevw.substr(0, linevw.find_first_of(u8'#'));
#if defined(__cpp_lib_ranges_find_last) && __cpp_lib_ranges_find_last >= 202207L
		linevw = linevw.substr(0, std::ranges::find_last_if_not(linevw, fast_io::char_category::is_c_space<char8_t>).begin() - linevw.begin() + 1);
#else
		auto rit = std::ranges::find_if_not(linevw.rbegin(), linevw.rend(), fast_io::char_category::is_c_space<char8_t>);
		linevw = linevw.substr(0, linevw.rend() - rit);
#endif
		if (linevw.empty())
		{
			continue;
		}
		if (linevw.front() == u8'[')
		{
			if (linevw.back() != u8']')
			{
				panic("invalid property file. unclosed bracket for sections\n");
			}
			auto section_name = linevw.substr(1, linevw.size() - 2);
			section_name = section_name.substr(std::ranges::find_if_not(section_name, fast_io::char_category::is_c_space<char8_t>) - section_name.begin());
#if defined(__cpp_lib_ranges_find_last) && __cpp_lib_ranges_find_last >= 202207L
			section_name = section_name.substr(0, std::ranges::find_last_if_not(section_name, fast_io::char_category::is_c_space<char8_t>).begin() - section_name.begin() + 1);
#else
			auto rit = std::ranges::find_if_not(section_name.rbegin(), section_name.rend(), fast_io::char_category::is_c_space<char8_t>);
			section_name = section_name.substr(0, section_name.rend() - rit);
#endif
			if (section_name.front() == u8'"')
			{
				if (section_name.back() != u8'"')
				{
					panic("invalid property file. unclosed quote for section name\n");
				}
				section_name = section_name.substr(1, section_name.size() - 2);
			}
#if defined(__cpp_lib_associative_heterogeneous_insertion) && __cpp_lib_associative_heterogeneous_insertion >= 202311L
			auto [itr, res]{file_contents.try_emplace(section_name)};
#else
			auto [itr, res]{file_contents.try_emplace(std::u8string{section_name})};
#endif
			if (!res)
			{
				panic("invalid property file. duplicate section\n");
			}
			curr_entry = &itr->second;
		}
		else
		{
			if (!curr_entry)
			{
				panic("invalid property file. gobal property entry not allowed\n");
			}
			auto eq_pos{linevw.find(u8'=')};
			if (eq_pos == std::u8string_view::npos)
			{
				panic("invalid property file. entry should be in form of key-value pair\n");
			}
			std::u8string_view key{linevw.begin(), linevw.begin() + eq_pos};
			std::u8string_view value{linevw.begin() + eq_pos + 1, linevw.end()};
			key = key.substr(std::ranges::find_if_not(key, fast_io::char_category::is_c_space<char8_t>) - key.begin());
			value = value.substr(std::ranges::find_if_not(value, fast_io::char_category::is_c_space<char8_t>) - value.begin());
#if defined(__cpp_lib_ranges_find_last) && __cpp_lib_ranges_find_last >= 202207L
			key = key.substr(0, std::ranges::find_last_if_not(key, fast_io::char_category::is_c_space<char8_t>).begin() - key.begin() + 1);
			value = value.substr(0, std::ranges::find_last_if_not(value, fast_io::char_category::is_c_space<char8_t>).begin() - value.begin() + 1);
#else
			auto ritkey = std::ranges::find_if_not(key.rbegin(), key.rend(), fast_io::char_category::is_c_space<char8_t>);
			key = key.substr(0, key.rend() - ritkey);
			auto ritvalue = std::ranges::find_if_not(value.rbegin(), value.rend(), fast_io::char_category::is_c_space<char8_t>);
			value = value.substr(0, value.rend() - ritvalue);
#endif
			if (key.front() == u8'"')
			{
				if (key.back() != u8'"')
				{
					panic("invalid property file. unclosed quote for key\n");
				}
				key = key.substr(1, key.size() - 2);
			}
			if (key.empty())
			{
				panic("invalid property file. key cannot be empty\n");
			}
			if (value.empty())
			{
				panic("invalid property file. value cannot be empty\n");
			}
#if defined(__cpp_lib_associative_heterogeneous_insertion) && __cpp_lib_associative_heterogeneous_insertion >= 202311L
			curr_entry->try_emplace(key, value);
#else
			curr_entry->try_emplace(std::u8string{key}, std::u8string{value});
#endif
		}
	}

	for (auto &[filename, entry] : file_contents)
	{
		if (entry.empty())
		{
			continue;
		}
		auto &file{file_properties.try_emplace(filename).first->second};
		if (auto itr{entry.find(u8"ignore")}; itr != entry.end())
		{
			if (itr->second == u8"true")
			{
				file.ignore = true;
			}
			else if (itr->second == u8"false")
			{
				file.ignore = false;
			}
			else
			{
				panic("invalid property file. `ignore` should be of boolean type\n");
			}
		}
		if (auto itr{entry.find(u8"interactive")}; itr != entry.end())
		{
			if (itr->second == u8"true")
			{
				file.interactive = true;
			}
			else if (itr->second == u8"false")
			{
				file.interactive = false;
			}
			else
			{
				panic("invalid property file. `interactive` should be of boolean type\n");
			}
		}
		if (auto itr{entry.find(u8"asan")}; itr != entry.end())
		{
			if (itr->second == u8"true")
			{
				file.asan = true;
			}
			else if (itr->second == u8"false")
			{
				file.asan = false;
			}
			else
			{
				panic("invalid property file. `asan` should be of boolean type\n");
			}
		}
		if (auto itr{entry.find(u8"will_fail")}; itr != entry.end())
		{
			if (itr->second == u8"\"never\"")
			{
				file.will_fail = will_fail_t::never;
			}
			else if (itr->second == u8"\"compiletime\"")
			{
				file.will_fail = will_fail_t::compiletime;
			}
			else if (itr->second == u8"\"runtime\"")
			{
				file.will_fail = will_fail_t::runtime;
			}
			else
			{
				panic("invalid property file. `will_fail` should be one of never, compiletime or runtime\n");
			}
		}
		if (auto itr{entry.find(u8"platform")}; itr != entry.end())
		{
			if (itr->second == u8"\"all\"")
			{
				file.platform = platform_t::all;
			}
			else if (itr->second == u8"\"windows\"")
			{
				file.platform = platform_t::windows;
			}
			else if (itr->second == u8"\"linux\"")
			{
				file.platform = platform_t::linux;
			}
			else if (itr->second == u8"\"macos\"")
			{
				file.platform = platform_t::macos;
			}
			else
			{
				panic("invalid property file. `platform` should be one of all, windows, linux or macos\n");
			}
		}
	}
}

inline bool gen_cmake_file(fast_io::native_io_observer nio, std::u8string_view prefix, file_entry_t prop, bool test);

inline void cmake_gen_rec_dir_def(fast_io::u8obuf_file &cmake_file, fast_io::native_io_observer nio, std::u8string_view filename, std::u8string_view prefix, file_entry_t const &prop, bool test)
{
	if (prop.ignore)
	{
		return;
	}
	std::u8string new_prefix{fast_io::u8concat_std(prefix, filename, u8".")};
	fast_io::dir_file dir{at(nio), filename};
	if (!gen_cmake_file(dir, new_prefix, prop, test))
	{
		return;
	}
	print(cmake_file, u8"include(${CMAKE_CURRENT_LIST_DIR}/", filename, u8"/CMakeLists.txt)\n");
}

inline void cmake_gen_file_def(fast_io::u8obuf_file &cmake_file, std::u8string_view targetname, auto &&filename, file_entry_t const &prop, bool test)
{
	if (prop.ignore || prop.will_fail == will_fail_t::compiletime)
	{
		return;
	}
	if (prop.platform != platform_t::all && !intersects(prop.platform, global_platform))
	{
		return;
	}
	print(cmake_file, u8"add_executable(", targetname, u8" ${CMAKE_CURRENT_LIST_DIR}/", filename, u8")\n");
	if (prop.interactive)
	{
		return;
	}
	if (prop.will_fail == will_fail_t::runtime)
	{
		return;
	}
	if (prop.asan)
	{
		if (msvc)
		{
			print(cmake_file, u8"set_target_properties(", targetname, u8" PROPERTIES CMAKE_BUILD_TYPE RelWithDebInfo CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} /fsanitize=address\")\n");
		}
		else
		{
			print(cmake_file, u8"set_target_properties(", targetname, u8" PROPERTIES CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -fsanitize=address\")\n");
		}
	}
	if (test)
	{
		print(cmake_file, u8"add_test(", targetname, u8" ", targetname, u8")\n");
	}
	// // set_tests_properties does not work. why?
	// if (prop.will_fail == will_fail_t::runtime)
	// 	print(cmake_file, u8"set_tests_properties(", targetname, u8" PROPERTIES WILL_FAIL TRUE)\n");
}

inline bool gen_cmake_file(fast_io::native_io_observer nio, std::u8string_view prefix, file_entry_t prop, bool test)
{
	bool generated{};
	fast_io::u8obuf_file cmake_file;
	try
	{
		cmake_file = std::move(fast_io::u8obuf_file{at(nio), R"(CMakeLists.txt)", fast_io::open_mode::creat | fast_io::open_mode::excl});
	}
	catch (fast_io::error)
	{
		perr(fast_io::u8err(), u8"CMakeLists.txt alreay exists in ", prefix, u8"\n");
		return true;
	}
	file_property_t file_props;
	try
	{
		fast_io::native_file_loader _{at(nio), R"(.test_prop.toml)"};
		parse_prop_files(fast_io::native_file_loader{at(nio), R"(.test_prop.toml)"}, file_props);
	}
	catch (fast_io::error)
	{}
	for (auto const &entry : current(at(nio)))
	{
		auto filename{std::u8string_view{u8filename(entry)}};
		if (is_dot(entry))
		{
			continue;
		}
		file_entry_t curr_prop;
		if (auto itr{file_props.find(std::u8string{filename})}; itr != file_props.end())
		{
			curr_prop = itr->second | prop;
		}
		else
		{
			curr_prop = prop;
		}
		switch (type(entry))
		{
			using enum fast_io::file_type;
		case directory:
			cmake_gen_rec_dir_def(cmake_file, nio, filename, prefix, curr_prop, test);
			generated = true;
			break;
		default:
			std::u8string_view ext{u8extension(entry)};
			if (ext == u8".cpp" || ext == u8".cc")
			{
				cmake_gen_file_def(cmake_file, fast_io::u8concat_std(prefix, u8stem(entry)), filename, curr_prop, test);
				generated = true;
			}
			break;
		}
	}
	return generated;
}

int main(int argc, char *argv[])
{
	using namespace std::literals;
	if (argc < 2)
	{
		if (argc == 0)
		{
			return 1;
		}
		perr("Usage: ", fast_io::mnp::os_c_str(argv[0]), " <TARGET> <COMMAND-LINE-STYLE?>.\n"
														 "Target name is one of windows, linux or macos.\n"
														 "Command-line-style(optional) is one of gcc(default) or msvc\n");
		return 1;
	}
	if (argv[1] == "all"sv)
	{
		global_platform = platform_t::all;
	}
	else if (argv[1] == "windows"sv)
	{
		global_platform = platform_t::windows;
	}
	else if (argv[1] == "linux"sv)
	{
		global_platform = platform_t::linux;
	}
	else if (argv[1] == "macos"sv)
	{
		global_platform = platform_t::macos;
	}
	else
	{
		panic("Invalid target. Should be one of all, windows, linux or macos.\n");
	}
	if (argc == 3)
	{
		if (argv[2] == "gcc"sv)
		{
			msvc = false;
		}
		else if (argv[2] == "msvc"sv)
		{
			msvc = true;
		}
		else
		{
			panic("Invalid command-line-style. Should be one of gcc or msvc.\n");
		}
	}
	fast_io::dir_file tests_dir{R"(./tests)"};
	fast_io::dir_file examples_dir{R"(./examples)"};
	fast_io::dir_file benchmark_dir{R"(./benchmark)"};
	gen_cmake_file(tests_dir, u8"tests.", {}, true);
	gen_cmake_file(examples_dir, u8"examples.", {}, true);
	gen_cmake_file(benchmark_dir, u8"benchmark.", {}, false);
}
