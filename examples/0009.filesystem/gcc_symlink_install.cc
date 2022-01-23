#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include<string_view>
#include<vector>
#include<algorithm>
#include<ranges>

int main(int argc,char const** argv)
{
	if(argc<2)
	{
		perr("Usage: ",fast_io::mnp::chvw(argv[0])," <dir path>\n");
		return 1;
	}
	std::vector<std::u8string> names;
	fast_io::dir_file df(::fast_io::mnp::os_c_str(argv[1]));
	using namespace std::string_view_literals;
	for(auto const& ent:current(at(df)))
	{
		auto ftype{type(ent)};
		if(ftype==fast_io::file_type::regular||ftype==fast_io::file_type::symlink)
		{
			auto exts{extension(ent)};
			std::u8string_view view(exts.cbegin(),exts.cend());
			if(view==u8".py"sv)	//excluse .py like libstdc++.so.6.0.29-gdb.py
				continue;
			auto name{filename(ent)};
			std::u8string_view name_view(name.cbegin(),name.cend());
			if(name_view==u8"libgcc_s.so"sv)	//libgcc_s.so should not be a symlink. It is a linker script
				continue;
			if(name_view.ends_with(u8".so"sv)||name_view.contains(u8".so."sv))
				names.emplace_back(name_view);
		}
	}
	if(names.empty())
		return 0;
	std::ranges::sort(names);
	std::size_t const sz{names.size()-1};
	for(std::size_t i{};i!=sz;++i)
	{
		std::size_t j{i};
		for(;j!=sz;++j)
		{
			if(names[j+1].size()<=names[i].size())
				break;
			if(!std::equal(names[i].cbegin(),names[i].cend(),names[j+1].cbegin()))
				break;
		}
		if(j==i)
			continue;
		native_unlinkat(at(df),names[i]);
		native_symlinkat(names[j],at(df),names[i]);
	}
}
