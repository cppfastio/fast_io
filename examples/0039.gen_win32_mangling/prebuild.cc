/**
 * @brief generate win32 and nt apis' manglings
 */

#include <cstdint>
#include <cstdlib>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>

enum class argtype : ::std::uint_least32_t
{
	normal = 0,
	pointer,
	va_args,
};

/**
 * @brief generate apis.ignore.cc's main function body
 */
inline auto gen_apis(
	::fast_io::u8string_view prefix,
	::fast_io::native_file_loader const &context,
	::fast_io::u8string &function_declaration,
	::fast_io::u8string &symbols) noexcept
{
	::fast_io::u8string main_body{};
	function_declaration.append(::fast_io::u8concat_fast_io(u8"namespace ", prefix, u8" {\n"));
	auto const &&header_size = context.size();
	for (::std::size_t i{}; i < header_size; ++i)
	{
		// FAST_IO_DLLIMPORT
		if (context[i] != u8'F')
		{
			while (context[i] != '\n')
			{
				++i;
			}
			continue;
		}
		i += 18;
		function_declaration.append(u8"FAST_IO_DLLIMPORT ");
		// <RetType>
		while (context[i] != u8'F')
		{
			function_declaration.push_back(context[i++]);
		}
		// FAST_IO_WINSTDCALL & FAST_IO_WINCDECL
		while (context[i] != u8' ')
		{
			function_declaration.push_back(context[i++]);
		}
		// funciton name
		main_body.append(::fast_io::u8concat_fast_io(u8"\t::", ::fast_io::mnp::os_c_str(prefix), u8"::"));
		do
		{
			function_declaration.push_back(context[i]);
			main_body.push_back(context[++i]);
		} while (context[i] != u8'(');
		++i;
		function_declaration.append(u8"(");

		// funciton signature
		if (context[i] == u8')')
		{
			// function()
			main_body.append(u8");\n");
			function_declaration.push_back(u8')');
		}
		else
		{
			// function(args...)
			::std::size_t brace_counter{1};
			argtype arg_type = argtype::normal;
			while (brace_counter != 0)
			{
				function_declaration.push_back(context[i]);
				if (context[i] == u8'*')
				{
					arg_type = argtype::pointer;
				}
				else if (context[i] == u8'.')
				{
					arg_type = argtype::va_args;
				}
				else if (context[i] == u8'(')
				{
					++brace_counter;
				}
				else if (context[i] == u8')')
				{
					--brace_counter;
				}
				else if (context[i] == u8',' && brace_counter == 1)
				{
					switch (arg_type)
					{
					case argtype::normal:
						main_body.append(u8"{},");
						break;
					case argtype::pointer:
						main_body.append(u8"nullptr,");
						break;
					case argtype::va_args:
						[[fallthrough]];
					default:
						::fast_io::unreachable();
					}
					arg_type = argtype::normal;
				}
				++i;
			}
			switch (arg_type) {
			case argtype::normal:
				main_body.append(u8"{});\n");
				break;
			case argtype::pointer:
				main_body.append(u8"nullptr);\n");
				break;
			case argtype::va_args:
				main_body.append(u8");\n");
				break;
			default:
				::fast_io::unreachable();
			}
		}
		function_declaration.append(u8" noexcept;\n");
		// FAST_IO_WINSTDCALL_RENAME
		while (context[i++] != u8'(')
		{
		}
		while (context[i] != u8')')
		{
			symbols.push_back(context[i++]);
		}
		symbols.push_back(u8'\n');
		while (context[++i] != u8'\n')
		{
		}
	}
	function_declaration.append(u8"}\n");
	return main_body;
}

::fast_io::u8string build_target_asm(::fast_io::u8string_view target) noexcept
{
	return ::fast_io::u8concat_fast_io(u8"build ", target, u8".s: toasm apis.ignore.cc\n  target = ", target,
									   u8"\nbuild ", target, u8": compile ../", target, u8".cc\nbuild run_", target, u8": run ", target, u8"\n");
}

::fast_io::u8string gen_ninja_build(char8_t const *windows_msvc_sysroot = nullptr)
{
	::fast_io::u8string result{u8R"(
rule toasm
  command = clang++ -S $in -o $out -O2 -I ../../../include -std=c++23 --target=$target)"};
	if (windows_msvc_sysroot != nullptr)
	{
		result.append(::fast_io::u8concat_fast_io(u8" --sysroot=\"", ::fast_io::mnp::os_c_str(windows_msvc_sysroot), u8"\"\n"));
	}
	result.append(u8R"(
rule compile
  command = clang++ $in -o $out -O2 -I ../../../include -std=c++23 -lntdll

rule run
  command = ./$in
)");

	result.append(build_target_asm(u8"x86_64-windows-msvc"));
	result.append(build_target_asm(u8"i686-windows-msvc"));
	result.append(build_target_asm(u8"arm-windows-msvc"));
	result.append(build_target_asm(u8"arm64ec-windows-msvc"));

	// 	result.append(R"(
	// default build/x86_64-windows-msvc.s build/i686-windows-msvc.s build/arm-windows-msvc.s build/arm64ec-windows-msvc.s
	// )");

	return result;
}

int main(int argc, char *const *const argv)
{
	if (argc > 2) [[unlikely]]
	{
		fast_io::perr("Usage: build <WINDOWS-MSVC-SYSROOT>");
		return 1;
	}

	::fast_io::native_mkdirat(::fast_io::at_fdcwd(), "build");

	auto const &&nt_header = ::fast_io::native_file_loader{"../../include/fast_io_hosted/platforms/nt/nt_api.h"};
	auto const &&win32_header = ::fast_io::native_file_loader{"../../include/fast_io_hosted/platforms/win32/apis.h"};

	::fast_io::u8string win32_declaration{}, nt_declaration{}, win32_symbols{}, nt_symbols{};
	// process win32 apis.h
	auto &&main_body = gen_apis(u8"fast_io::win32", win32_header, win32_declaration, win32_symbols);
	// process nt_api.h
	main_body.append(gen_apis(u8"fast_io::win32::nt", nt_header, nt_declaration, nt_symbols));

	::fast_io::native_file win32_symbols_file{"build/win32_symbols.txt", ::fast_io::open_mode::out};
	::fast_io::io::print(win32_symbols_file, ::fast_io::mnp::code_cvt(win32_symbols));
	::fast_io::native_file nt_symbols_file{"build/nt_symbols.txt", ::fast_io::open_mode::out};
	::fast_io::io::print(nt_symbols_file, ::fast_io::mnp::code_cvt(nt_symbols));

	// generate apis.ignore.cc
	::fast_io::native_file apis{"build/apis.ignore.cc", ::fast_io::open_mode::out};
	::fast_io::io::print(apis, ::fast_io::mnp::code_cvt(::fast_io::u8concat_fast_io(u8R"(
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_hosted/platforms/win32/win32_definitions.h>
#include <fast_io_hosted/platforms/nt/nt_definitions.h>
)",
																					win32_declaration,
																					nt_declaration,
																					u8R"(
int main() noexcept {
)",
																					main_body,
																					::fast_io::mnp::chvw('}'))));

	::fast_io::native_file build_ninja{"build/build.ninja", ::fast_io::open_mode::out};
	if (argc == 2)
	{
		::std::size_t len_argv = strlen(argv[1]);
		if (argv[1][len_argv - 1] == '\\')
		{
			argv[1][len_argv - 1] = 0;
		}
		::fast_io::io::print(build_ninja, ::fast_io::mnp::code_cvt(gen_ninja_build(reinterpret_cast<char8_t *>(argv[1]))));
	}
	else
	{
		::fast_io::io::print(build_ninja, ::fast_io::mnp::code_cvt(gen_ninja_build()));
	}

	return 0;
}
