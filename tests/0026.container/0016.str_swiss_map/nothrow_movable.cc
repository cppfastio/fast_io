#include <fast_io.h>
#include <fast_io_dsal/str_swiss_map.h>

// basic_str_swiss_map requires the mapped type to be nothrow movable: slots
// are move-constructed into the new table when it grows and move-assigned
// into their probe positions when tombstones are rehashed in place.
struct tracked_mapped
{
	static inline ::std::size_t move_constructs{};
	static inline ::std::size_t move_assigns{};
	::std::size_t val{};

	tracked_mapped() noexcept = default;
	explicit tracked_mapped(::std::size_t v) noexcept
		: val(v)
	{}
	tracked_mapped(tracked_mapped const &o) noexcept
		: val(o.val)
	{}
	tracked_mapped &operator=(tracked_mapped const &o) noexcept
	{
		val = o.val;
		return *this;
	}
	tracked_mapped(tracked_mapped &&o) noexcept
		: val(o.val)
	{
		++move_constructs;
	}
	tracked_mapped &operator=(tracked_mapped &&o) noexcept
	{
		val = o.val;
		++move_assigns;
		return *this;
	}
};

static_assert(::std::movable<tracked_mapped>);
static_assert(::std::is_nothrow_move_constructible_v<tracked_mapped>);
static_assert(::std::is_nothrow_move_assignable_v<tracked_mapped>);

inline constexpr ::std::size_t make_key(::std::size_t i, char *buf) noexcept
{
	buf[0] = 'k';
	for (::std::size_t d{}; d != 16; ++d)
	{
		buf[16 - d] = static_cast<char>("0123456789abcdef"[i & 0xfu]);
		i >>= 4u;
	}
	return 17;
}

int main()
{
	// grow: inserting past the growth bound relocates every slot by move
	// construction into the larger table
	{
		::fast_io::str_swiss_map<tracked_mapped> mp;
		char keybuf[17];
		for (::std::size_t i{}; i != 100; ++i)
		{
			mp.insert_key(::fast_io::string_view{keybuf, make_key(i, keybuf)}, tracked_mapped{i});
		}
		::fast_io::io::println("size=", mp.size(), " move_constructs=", tracked_mapped::move_constructs);
		if (mp.size() != 100 || !tracked_mapped::move_constructs)
		{
			::fast_io::fast_terminate();
		}
		for (::std::size_t i{}; i != 100; ++i)
		{
			auto it{mp.find_key(::fast_io::string_view{keybuf, make_key(i, keybuf)})};
			if (it == mp.end() || it->mapped().val != i)
			{
				::fast_io::fast_terminate();
			}
		}
	}

	// in-place rehash: on a table wider than one probe group, erasures mark
	// slots deleted; once growth_left is exhausted while tombstones remain,
	// the next insertion rehashes in place and move-assigns the survivors
	{
		tracked_mapped::move_assigns = 0;
		::fast_io::str_swiss_map<tracked_mapped> mp;
		mp.reserve(24); // capacity 31 > group width: erasures can tombstone
		char keybuf[17];
		::std::size_t next{};
		::std::size_t round_base{};
		for (::std::size_t round{}; round != 1000 && !tracked_mapped::move_assigns; ++round)
		{
			while (mp.size() < 28)
			{
				mp.insert_key(::fast_io::string_view{keybuf, make_key(next, keybuf)}, tracked_mapped{next});
				++next;
			}
			for (::std::size_t i{round_base}; i != next; ++i)
			{
				mp.erase_key(::fast_io::string_view{keybuf, make_key(i, keybuf)});
			}
			round_base = next;
		}
		::fast_io::io::print("move_assigns=", tracked_mapped::move_assigns, " after ", next, " inserts\n");
		if (!tracked_mapped::move_assigns)
		{
			::fast_io::fast_terminate();
		}
	}

	::fast_io::io::print("nothrow movable mapped type survived grow and rehash.\n");
}
