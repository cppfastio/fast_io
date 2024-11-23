#include <fast_io.h>

int main(int argc, char** argv)
{
	{
		::fast_io::io::perr("base=2, u8\n");
		constexpr char8_t base{ 2 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=2, i8\n");

		constexpr char8_t base{ 2 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	} 
	{
		::fast_io::io::perr("base=2, u16\n");

		constexpr char8_t base{ 2 };
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=2, i16\n");

		constexpr char8_t base{ 2 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=2, u32\n");

		constexpr char8_t base{ 2 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=2, i32\n");

		constexpr char8_t base{ 2 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=3, u8\n");
		constexpr char8_t base{ 3 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=3, i8\n");

		constexpr char8_t base{ 3 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=3, u16\n");

		constexpr char8_t base{ 3 };
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=3, i16\n");

		constexpr char8_t base{ 3 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=3, u32\n");

		constexpr char8_t base{ 3 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=3, i32\n");

		constexpr char8_t base{ 3 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	// b4
	{
		::fast_io::io::perr("base=4, u8\n");
		constexpr char8_t base{ 4 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=4, i8\n");

		constexpr char8_t base{ 4 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=4, u16\n");

		constexpr char8_t base{ 4};
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=4, i16\n");

		constexpr char8_t base{ 4 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=4, u32\n");

		constexpr char8_t base{ 4 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=4, i32\n");

		constexpr char8_t base{ 4 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	// b5
	{
		::fast_io::io::perr("base=5, u8\n");
		constexpr char8_t base{ 5 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=5, i8\n");

		constexpr char8_t base{ 5 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=5, u16\n");

		constexpr char8_t base{ 5 };
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=5, i16\n");

		constexpr char8_t base{ 5 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=5, u32\n");

		constexpr char8_t base{ 5 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=5, i32\n");

		constexpr char8_t base{ 5 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	// b6
	{
		::fast_io::io::perr("base=6, u8\n");
		constexpr char8_t base{ 6 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=6, i8\n");

		constexpr char8_t base{ 6 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=6, u16\n");

		constexpr char8_t base{ 6 };
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=6, i16\n");

		constexpr char8_t base{ 6 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=6, u32\n");

		constexpr char8_t base{ 6 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=6, i32\n");

		constexpr char8_t base{ 6 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	// b7
	{
		::fast_io::io::perr("base=7, u8\n");
		constexpr char8_t base{ 7 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=7, i8\n");

		constexpr char8_t base{ 7 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=7, u16\n");

		constexpr char8_t base{ 7 };
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=7, i16\n");

		constexpr char8_t base{ 7 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=7, u32\n");

		constexpr char8_t base{ 7 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=7, i32\n");

		constexpr char8_t base{7 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	// b8
	{
		::fast_io::io::perr("base=8, u8\n");
		constexpr char8_t base{ 8 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=8, i8\n");

		constexpr char8_t base{ 8 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=8, u16\n");

		constexpr char8_t base{ 8 };
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=8, i16\n");

		constexpr char8_t base{ 8 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=8, u32\n");

		constexpr char8_t base{ 8 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=8, i32\n");

		constexpr char8_t base{ 8 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	// b9
	{
		::fast_io::io::perr("base=9, u8\n");
		constexpr char8_t base{ 9 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=9, i8\n");

		constexpr char8_t base{ 9 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=9, u16\n");

		constexpr char8_t base{ 9 };
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=9, i16\n");

		constexpr char8_t base{ 9 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=9, u32\n");

		constexpr char8_t base{ 9 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=9, i32\n");

		constexpr char8_t base{9 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	// b10
	{
		::fast_io::io::perr("base=10, u8\n");
		constexpr char8_t base{10 };
		using tyupe = ::std::uint_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=10, i8\n");

		constexpr char8_t base{ 10 };
		using tyupe = ::std::int_least8_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=10, u16\n");

		constexpr char8_t base{ 10 };
		using tyupe = ::std::uint_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=10, i16\n");

		constexpr char8_t base{ 10 };
		using tyupe = ::std::int_least16_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=10, u32\n");

		constexpr char8_t base{ 10 };
		using tyupe = ::std::uint_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}
	{
		::fast_io::io::perr("base=10, i32\n");

		constexpr char8_t base{ 10 };
		using tyupe = ::std::int_least32_t;
		for (tyupe i = 0; i < ::std::numeric_limits<tyupe>::max(); i++)
		{
			char8_t buf[128]{};
			::fast_io::u8obuffer_view obf{ buf,buf + 128 };
			::fast_io::io::perr(obf, ::fast_io::mnp::base<base>(i));
			tyupe res;
			::fast_io::details::scan_int_contiguous_none_space_part_define_impl<base>(buf, buf + 128, res);
			if (res != i)
			{
				::fast_io::io::panicln("error: ", i);
			}
		}
	}

}
