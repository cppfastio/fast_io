// locale_dump — decode + disassemble a .bin locale blob for verification.
// The file IS the lc_locale struct — dump resolves members by rva and
// prints what each field points at.
//
// usage: locale_dump <file.bin>

#include <herbceptions/error>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include "localedef.h"
#include <fast_io_dsal/impl/misc/push_macros.h>

namespace
{
using namespace ::fast_io_i18n;
namespace ilc = ::fast_io::l10n;

void dump_program(::fast_io::u8string_view prog, unsigned indent) FAST_IO_HERBCEPTIONS_THROWS
{
	using namespace ::fast_io_i18n::binfmt;
	reader r{prog};
	auto pad{[](unsigned n) FAST_IO_HERBCEPTIONS_THROWS {
		for (unsigned i{}; i < n; ++i)
		{
			::fast_io::print(" ");
		}
	}};
	while (!r.is_empty())
	{
		node_head h;
		next_tag(r, h);
		pad(indent);
		switch (h.code)
		{
		case op_literal:
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::u8string_view bytes;
			r.get_bytes(n, bytes);
			::fast_io::println("literal [", n, "]");
			break;
		}
		case op_field:
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::println("field children=", n);
			for (::std::uint_least64_t k{}; k < n; ++k)
			{
				node_head c;
				next_tag(r, c);
				pad(indent + 2);
				::fast_io::print("param ", c.code, " kind=", (int)c.kind);
				if (c.kind == kind_uleb)
				{
					::std::uint_least64_t v{};
					r.get_leb(v);
					::fast_io::print(" = ", v);
				}
				else if (c.kind == kind_sleb)
				{
					::std::int_least64_t v{};
					r.get_leb(v);
					::fast_io::print(" = ", v);
				}
				else if (c.kind == kind_bytes)
				{
					::std::uint_least64_t v{};
					r.get_leb(v);
					::fast_io::u8string_view bytes;
					r.get_bytes(v, bytes);
					::fast_io::print(" [", v, "]");
				}
				::fast_io::print("\n");
			}
			break;
		}
		case op_pct:
			if (h.kind == kind_uleb)
			{
				::std::uint_least64_t v{};
				r.get_leb(v);
				::fast_io::println("pct conv=", v);
			}
			else
			{
				::std::uint_least64_t n{};
				r.get_leb(n);
				::fast_io::println("pct children=", n);
				for (::std::uint_least64_t k{}; k < n; ++k)
				{
					node_head c;
					next_tag(r, c);
					pad(indent + 2);
					::std::uint_least64_t v{};
					r.get_leb(v);
					::fast_io::println("param ", c.code, " = ", v);
				}
			}
			break;
		default:
			::fast_io::println("node code=", h.code, " kind=", (int)h.kind);
			skip_payload(r, h);
			break;
		}
	}
}

void print_scatter(char const *base, char const *img, ::std::uint_least32_t off,
				   char const *fname, bool is_program) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const *s{reinterpret_cast<ilc::lc_scatter<char> const *>(img + off)};
	::std::uint_least32_t const rva{ilc::lc_u32(s->ref.off)};
	::std::uint_least32_t const len{ilc::lc_u32(s->len)};
	::fast_io::print("  ", ::fast_io::mnp::os_c_str(fname), " rva=", rva, " len=", len);
	if (rva == 0)
	{
		::fast_io::println(" (absent)");
		return;
	}
	if (is_program)
	{
		::fast_io::print("\n");
		dump_program(::fast_io::u8string_view{
						 reinterpret_cast<char8_t const *>(base + rva), len},
					 4);
		return;
	}
	if (len <= 48)
	{
		::fast_io::println(" \"", ::fast_io::string_view{base + rva, len}, "\"");
	}
	else
	{
		::fast_io::println(" \"", ::fast_io::string_view{base + rva, 48}, "...\"");
	}
}

} // namespace

int main(int argc, char **argv)
try
{
	if (argc != 2)
	{
		::fast_io::perrln("usage: locale_dump <file.bin>");
		return 1;
	}
	::fast_io::native_file_loader fmp(::fast_io::mnp::os_c_str(argv[1]));
	char const *base{reinterpret_cast<char const *>(fmp.data())};
	if (fmp.size() < sizeof(ilc::lc_locale))
	{
		::fast_io::perrln("not a locale blob (too small)");
		return 1;
	}
	auto const *loc{reinterpret_cast<ilc::lc_locale const *>(base)};
	if (ilc::lc_u32(loc->magic) != ilc::magic)
	{
		::fast_io::perrln("bad magic");
		return 1;
	}
	::fast_io::println("version=", ilc::lc_u32(loc->version),
					   " total=", ilc::lc_u32(loc->total),
					   " codeset=", ilc::lc_u32(loc->codeset),
					   " (file ", fmp.size(), ")");
	if (auto nm{ilc::lc_get_scatter(base, loc->name)}; nm.base != nullptr)
	{
		::fast_io::println("name=\"",
						   ::fast_io::string_view{
							   reinterpret_cast<char const *>(nm.base), nm.len},
						   "\"");
	}
	::fast_io::string_view const slot_names[]{"charset", "utf8", "utf16", "utf32"};
	::std::uint_least32_t const slots[4]{ilc::lc_u32(loc->all.off),
										 ilc::lc_u32(loc->u8all.off),
										 ilc::lc_u32(loc->u16all.off),
										 ilc::lc_u32(loc->u32all.off)};
	for (::std::size_t c{}; c < 4; ++c)
	{
		::std::uint_least32_t const rva{slots[c]};
		if (rva == 0)
		{
			continue;
		}
		char const *img{base + rva};
		if (c == 0 && slots[0] == slots[1])
		{
			::fast_io::println("charset @", rva, " (aliases utf8)");
		}
		else
		{
			::fast_io::println(slot_names[c], " @", rva);
		}
		for (::std::size_t cat{}; cat < lcblob::cat_count; ++cat)
		{
			auto const &sch{lcblob::cat_schemas[cat]};
			auto const &m{lcblob::details::cat_maps[cat]};
			bool printed{};
			for (::std::size_t f{}; f < sch.fields.size(); ++f)
			{
				auto const &def{sch.fields[f]};
				auto const off{m.off[f]};
				// absent check: first u32 of the member
				::std::uint_least32_t const first_u32{ilc::lc_u32(
					*reinterpret_cast<::std::uint_least32_t const *>(
						img + off))};
				if (first_u32 == 0)
				{
					continue;
				}
				if (!printed)
				{
					::fast_io::println(" ",
									   ::fast_io::mnp::os_c_str(sch.glibc_name));
					printed = true;
				}
				switch (def.tag)
				{
				case lcblob::slot_tag::string:
					print_scatter(base, img, off, def.name, false);
					break;
				case lcblob::slot_tag::program:
					print_scatter(base, img, off, def.name, true);
					break;
				case lcblob::slot_tag::bytes:
					print_scatter(base, img, off, def.name, false);
					break;
				case lcblob::slot_tag::strlist:
					if (m.aux[f])
					{
						for (::std::size_t k{}; k < m.aux[f]; ++k)
						{
							print_scatter(base, img, off + k * 8,
										  def.name, false);
						}
					}
					else
					{
						print_scatter(base, img, off, def.name, false);
					}
					break;
				case lcblob::slot_tag::eralist:
					print_scatter(base, img, off, def.name, false);
					break;
				case lcblob::slot_tag::integer:
					::fast_io::println(
						"  ", ::fast_io::mnp::os_c_str(def.name), " = ",
						ilc::lc_s32(first_u32));
					break;
				case lcblob::slot_tag::int3:
					::fast_io::println(
						"  ", ::fast_io::mnp::os_c_str(def.name), " = ",
						ilc::lc_s32(first_u32), " ",
						ilc::lc_s32(*reinterpret_cast<
									::std::uint_least32_t const *>(img + off + 4)),
						" ",
						ilc::lc_s32(*reinterpret_cast<
									::std::uint_least32_t const *>(img + off + 8)));
					break;
				default:
					break;
				}
			}
		}
	}
	return 0;
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}

#include <fast_io_dsal/impl/misc/pop_macros.h>
