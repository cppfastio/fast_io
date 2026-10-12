#include <fast_io.h>
#include <fast_io_dsal/str_swiss_map.h>
#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
	::fast_io::str_swiss_map<::std::size_t> mp;

	for (size_t i = 0; i < size;)
	{
		if (i >= size)
			break;
		uint8_t op = data[i] % 5u;
		++i;

		if (i >= size)
			break;
		uint8_t len = data[i] % 32u;
		++i;

		if (len == 0 || i + static_cast<size_t>(len) > size)
			continue;

		::fast_io::string_view key(reinterpret_cast<char const *>(data + i), static_cast<size_t>(len));
		i += static_cast<size_t>(len);

		switch (op)
		{
		case 0:
			mp.insert_key(key, static_cast<::std::size_t>(len));
			break;
		case 1:
			mp.insert_key_or_assign(key, static_cast<::std::size_t>(len));
			break;
		case 2:
			mp.erase_key(key);
			break;
		case 3:
			(void)mp.contains(key);
			break;
		case 4:
			(void)mp.find_key(key);
			break;
		}
	}
	return 0;
}
