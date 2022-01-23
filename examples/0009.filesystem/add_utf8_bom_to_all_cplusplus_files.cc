#include<fast_io.h>
#include<fast_io_device.h>
#include<string_view>
#include<array>
#include<algorithm>
#include<cstring>

int main(int argc,char** argv)
{
	if(argc<2)
	{
		perr("Usage: ",fast_io::mnp::chvw(*argv)," <source directory>\n");
		return 1;
	}
	using namespace std::string_view_literals;
	constexpr std::array<char8_t,3> utf8bom{0xEF,0xBB,0xBF};
	fast_io::dir_file df(::fast_io::mnp::os_c_str(argv[1]));
	for(auto const& ent : recursive(at(df)))
	{
		auto exts{extension(ent)};
		std::u8string_view view(exts.cbegin(),exts.cend());
		if(view!=u8".cc"sv&&view!=u8".h"sv&&view!=u8".cpp"sv&&view!=u8".cxx"sv&&view!=u8".hpp"sv)
			continue;
		fast_io::allocation_file_loader loader(drt(ent));	
		if(loader.size()>=utf8bom.size()&&std::memcmp(loader.data(),utf8bom.data(),3u)==0)
			continue;
		fast_io::obuf_file obf(drt(ent),fast_io::open_mode::out);
		write(obf,utf8bom.cbegin(),utf8bom.cend());
		print(obf,loader);
	}
}
