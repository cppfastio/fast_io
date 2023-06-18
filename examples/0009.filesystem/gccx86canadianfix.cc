#include<fast_io.h>
#include<fast_io_device.h>
#include<string_view>
#include<algorithm>
#include<functional>

using namespace fast_io::io;

int main(int argc,char **argv)
{
	if(argc<2)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage:",fast_io::mnp::os_c_str(*argv)," <gcc source directory>\n");
		return 1;
	}
	using namespace ::std::string_view_literals;

	constexpr
		::std::u8string_view vw(u8R"abc(case $GCC,$host_os in
  yes,cygwin* | yes,mingw* | yes,pw32* | yes,cegcc*)
    library_names_spec='$libname.dll.a'
    # DLL is installed to $(libdir)/../bin by postinstall_cmds
    postinstall_cmds='base_file=`basename \${file}`~
      dlpath=`$SHELL 2>&1 -c '\''. $dir/'\''\${base_file}'\''i; echo \$dlname'\''`~
      dldir=$destdir/`dirname \$dlpath`~
      test -d \$dldir || mkdir -p \$dldir~
      $install_prog $dir/$dlname \$dldir/$dlname~
      chmod a+x \$dldir/$dlname~
      if test -n '\''$stripme'\'' && test -n '\''$striplib'\''; then
        eval '\''$striplib \$dldir/$dlname'\'' || exit \$?;
      fi'
    postuninstall_cmds='dldll=`$SHELL 2>&1 -c '\''. $file; echo \$dlname'\''`~
      dlpath=$dir/\$dldll~
       $RM \$dlpath'
    shlibpath_overrides_runpath=yes)abc"sv);
	constexpr
		::std::u8string_view newvw(u8R"abc(case $GCC,$host_os in
  yes,cygwin* | yes,mingw* | yes,pw32* | yes,cegcc*)
    library_names_spec='$libname.dll.a'
    # DLL is installed to $(libdir)/../bin by postinstall_cmds
    # If user builds GCC with mulitlibs enabled, it should just install on $(libdir)
    # not on $(libdir)/../bin or 32 bits dlls would override 64 bit ones.
    if test ${multilib} = yes; then
    postinstall_cmds='base_file=`basename \${file}`~
      dlpath=`$SHELL 2>&1 -c '\''. $dir/'\''\${base_file}'\''i; echo \$dlname'\''`~
      dldir=$destdir/`dirname \$dlpath`~
      $install_prog $dir/$dlname $destdir/$dlname~
      chmod a+x $destdir/$dlname~
      if test -n '\''$stripme'\'' && test -n '\''$striplib'\''; then
        eval '\''$striplib $destdir/$dlname'\'' || exit \$?;
      fi'
    else
    postinstall_cmds='base_file=`basename \${file}`~
      dlpath=`$SHELL 2>&1 -c '\''. $dir/'\''\${base_file}'\''i; echo \$dlname'\''`~
      dldir=$destdir/`dirname \$dlpath`~
      test -d \$dldir || mkdir -p \$dldir~
      $install_prog $dir/$dlname \$dldir/$dlname~
      chmod a+x \$dldir/$dlname~
      if test -n '\''$stripme'\'' && test -n '\''$striplib'\''; then
        eval '\''$striplib \$dldir/$dlname'\'' || exit \$?;
      fi'
    fi
    postuninstall_cmds='dldll=`$SHELL 2>&1 -c '\''. $file; echo \$dlname'\''`~
      dlpath=$dir/\$dldll~
       $RM \$dlpath'
    shlibpath_overrides_runpath=yes)abc"sv);
	::std::boyer_moore_horspool_searcher searcher(reinterpret_cast<char const*>(vw.data()),
		reinterpret_cast<char const*>(vw.data())+vw.size());
	fast_io::dir_file df(fast_io::mnp::os_c_str(argv[1]));
	for(auto ent : recursive(at(df)))
	{
		if(type(ent)!=fast_io::file_type::regular)
		{
			continue;
		}
		std::u8string_view fnm_view(u8filename(ent));
		if(fnm_view==u8"configure"sv||fnm_view==u8"libtool.m4"sv)
		{
			{
				fast_io::native_file_loader loader(drt(ent));
				auto bg{loader.data()};
				auto ed{loader.data()+loader.size()};
				auto it{std::search(bg,ed,searcher)};
				if(it==ed)
				{
					continue;
				}
			}
			fast_io::allocation_file_loader loader(drt(ent));
			fast_io::obuf_file obf(drt(ent));
			for(auto it{loader.data()},ed{it+loader.size()};;)
			{
				auto retit{std::search(it,ed,searcher)};
				::fast_io::operations::write_all(obf,it,retit);
				if(retit==ed)
				{
					break;
				}
				::fast_io::operations::write_all_bytes(obf,
					reinterpret_cast<::std::byte const*>(newvw.data()),
					reinterpret_cast<::std::byte const*>(newvw.data()+newvw.size()));
				it=retit+vw.size();
			}
		}
	}
}
