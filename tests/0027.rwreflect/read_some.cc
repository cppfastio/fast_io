#include<fast_io.h>
#include<fast_io_core_impl/rwreflect/impl.h>
#include<fast_io_core_impl/rwreflect/reflectread.h>

namespace fast_io
{

inline ::std::byte* read_some_bytes_define(
	::fast_io::posix_io_observer piob,::std::byte* first,::std::byte* last)
{
	return read(piob,first,last);
}

inline ::std::byte const* write_some_bytes_define(
	::fast_io::posix_io_observer piob,::std::byte const* first,::std::byte const* last)
{
	return write(piob,first,last);
}

}

int main()
{
	::fast_io::posix_file nf(u8"wsme.txt",::fast_io::open_mode::in);
	char buffer[20]="";
	char buffer1[20]="";

	::fast_io::io_scatter_t scat[2]
	{{buffer,6},{buffer1,6}};

	::fast_io::scatter_read_all_bytes(nf,scat,2);
	::fast_io::reads_all(nf,buffer,buffer+6);

	::fast_io::posix_file nfout(u8"wsmeout.txt",::fast_io::open_mode::out);
	::fast_io::scatter_write_all_bytes(nfout,scat,2);
	::fast_io::write_all(nfout,buffer,buffer+6);
}
