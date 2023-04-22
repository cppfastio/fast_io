#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main(int argc,char** argv)
{
	if(argc<2)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage: ",::fast_io::mnp::os_c_str(*argv)," <dir>\n");
		return 1;
	}
	using namespace std::string_view_literals;
	fast_io::dir_file df(fast_io::mnp::os_c_str(argv[1]));
	std::size_t rs_count{};
	std::size_t cpp_count{};
	std::size_t c_count{};
	std::size_t h_count{};
	std::size_t hpp_count{};
	std::size_t mm_count{};
	std::size_t py_count{};
	std::size_t assembly_count{};
	for(auto ent: recursive(at(df)))
	{
		if(type(ent)!=fast_io::file_type::regular)
		{
			continue;
		}
		std::u8string_view exten{u8extension(ent)};
		if(exten==u8".rs"sv)
		{
			++rs_count;
		}
		else if(exten==u8".cpp"sv||exten==u8".cc"sv||exten==u8".cxx"sv)
		{
			++cpp_count;
		}
		else if(exten==u8".h"sv)
		{
			++h_count;
		}
		else if(exten==u8".c"sv)
		{
			++c_count;
		}
		else if(exten==u8".hpp"sv)
		{
			++hpp_count;
		}
		else if(exten==u8".mm"sv)
		{
			++mm_count;
		}
		else if(exten==u8".s"sv)
		{
			++assembly_count;
		}
		else if(exten==u8".py"sv)
		{
			++py_count;
		}
	}
	println("counts of rust source files (*.rs): ",rs_count,"\n"
	"counts of C++ source files (*.cc,*.cpp,*.cxx): ",cpp_count,"\n"
	"counts of C source files (*.c): ",c_count,"\n"
	"counts of C / C++ common header files (*.h): ",h_count,"\n"
	"counts of C++ header files (*.hpp): ",hpp_count,"\n"
	"counts of all C++ files: ",cpp_count+h_count+hpp_count,"\n"
	"counts of all C/C++ files: ",c_count+cpp_count+h_count+hpp_count,"\n"
	"counts of all Objective C/C++: ",mm_count,"\n"
	"counts of assembly files: ",assembly_count,"\n"
	"counts of python files:",py_count);
}