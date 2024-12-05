#include <fast_io.h>
#include <fast_io_legacy.h>
int main()
{
	{
		::fast_io::filebuf_file f{u8"filebuf.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::wfilebuf_file f{u8"wfilebuf.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::c_file f{u8"c_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::wc_file f{u8"wc_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u8c_file f{u8"u8c_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u16c_file f{u8"u16c_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u32c_file f{u8"u32c_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::posix_file f{u8"posix_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::wposix_file f{u8"wposix_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u8posix_file f{u8"u8posix_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u16posix_file f{u8"u16posix_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u32posix_file f{u8"u32posix_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}

#ifdef _WIN32
	{
		::fast_io::win32_file f{u8"win32_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::wwin32_file f{u8"wwin32_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u8win32_file f{u8"u8win32_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u16win32_file f{u8"u16win32_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u32win32_file f{u8"u32win32_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
#if !defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x300
	{
		::fast_io::nt_file f{u8"nt_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::wnt_file f{u8"wnt_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u8nt_file f{u8"u8nt_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u16nt_file f{u8"u16nt_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}
	{
		::fast_io::u32nt_file f{u8"u32nt_file.txt", ::fast_io::open_mode::out};
		for (::std::size_t i{}; i < 100; i++)
		{
			::fast_io::io::perrln(f, i);
		}
	}

#endif 

#endif
}
