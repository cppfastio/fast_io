// module consumer: vector<c_file> emplace_back propagates a herbception across
// the module boundary when the file open fails.
import fast_io;

int main()
{
	try
	{
		::fast_io::vector<::fast_io::c_file> v;
		v.emplace_back("/tmp/fast_io_module_test_tmp.txt", ::fast_io::open_mode::out);
		v.emplace_back("/definitely/not/existing/dir/f", ::fast_io::open_mode::in);
		::fast_io::io::perr("unreachable\n");
		return 2;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln("caught through module: ", e,
							  " equivalent:",
							  e.equivalent(::std::errc::no_such_file_or_directory));
		return !e.equivalent(::std::errc::no_such_file_or_directory);
	}
}
