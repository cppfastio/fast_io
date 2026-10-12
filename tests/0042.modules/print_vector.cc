// module consumer smoke test: import fast_io, print + vector
// requires a built fast_io BMI: e.g.
//   clang++ --config=... -std=c++26 -fherbceptions --precompile -x c++-module \
//       share/fast_io/fast_io.cppm -I include -o fast_io.pcm
import fast_io;

int main()
{
	try
	{
		::fast_io::io::print("Hello World\n");
		::fast_io::vector<int> v{1, 2, 3};
		for (auto i : v)
		{
			::fast_io::io::print(i, " ");
		}
		::fast_io::io::print("done\n");
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln("unexpected error: ", e);
		return 1;
	}
	return 0;
}
