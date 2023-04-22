#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>
#include<algorithm>
#include<functional>

using namespace fast_io::io;

int main(int argc, char **argv)
{
	using namespace ::fast_io::mnp;
	if(argc<3)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage: ",os_c_str(*argv)," <wow addons directory> <new version string>\n");
		return 1;
	}
	std::string_view versionstring{argv[2]};
	using namespace std::string_view_literals;
	fast_io::dir_file df(os_c_str(argv[1]));
	constexpr
		::std::u8string_view vw(u8"## Interface: ");
	::std::boyer_moore_horspool_searcher searcher(reinterpret_cast<char const*>(vw.data()),
		reinterpret_cast<char const*>(vw.data())+vw.size());
	for(auto ent : recursive(at(df)))
	{
		if(type(ent) != fast_io::file_type::regular)
		{
			continue;	
		}
		std::u8string_view ext(u8extension(ent));
		if(ext==u8".toc"sv)
		{
			fast_io::allocation_file_loader loader(drt(ent));
			auto i{loader.cbegin()};
			auto bg{loader.data()};
			auto ed{loader.data()+loader.size()};
			auto it{std::search(bg,ed,searcher)};
			if(it==ed)
			{
				continue;
			}
			if(it-loader.data()>3)
			{
				continue;
			}
			auto itvwsze{it+vw.size()};
			auto nextn{std::find(itvwsze,ed,u8'\n')};
			fast_io::obuf_file obf(drt(ent));
			write(obf,bg,itvwsze);
			print(obf,versionstring);
			write(obf,nextn,ed);
		}
	}
}