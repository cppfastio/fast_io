#pragma once

namespace fast_io
{
struct win9x_dirent
{
	void *file_struct{};
	::fast_io::u8string find_path{};

	::fast_io::win9x_dir_handle d_handle{};
	file_type d_type{};
	::std::uint_least64_t d_ino{};
	::fast_io::u8string filename{};

	~win9x_dirent()
	{
		if (file_struct) [[likely]]
		{
			::fast_io::win32::FindClose(file_struct);
		}
	}
};

namespace win32::details
{
inline bool set_win9x_dirent(win9x_dirent &entry, bool start)
{
	::fast_io::win32::win32_find_dataa wfda{};
	if (start)
	{
		entry.find_path = ::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt(entry.d_handle.path), u8"\\*");
		entry.file_struct = ::fast_io::win32::FindFirstFileA(reinterpret_cast<char const *>(entry.find_path.c_str()), __builtin_addressof(wfda));
	}
	else
	{
		if (::fast_io::win32::FindNextFileA(entry.file_struct, __builtin_addressof(wfda)) == 0) [[unlikely]]
		{
			return false;
		}
	}

	entry.filename = ::fast_io::u8string{::fast_io::u8concat_fast_io(::fast_io::mnp::code_cvt_os_c_str(wfda.cFileName))};

	if (wfda.dwFileAttributes & 0x400)
	{
		entry.d_type = file_type::symlink;
	}
	else if (wfda.dwFileAttributes & 0x40)
	{
		entry.d_type = file_type::character;
	}
	else if (wfda.dwFileAttributes & 0x10)
	{
		entry.d_type = file_type::directory;
	}
	else if (wfda.dwFileAttributes & 0x1000)
	{
		entry.d_type = file_type::remote;
	}
	else
	{
		entry.d_type = file_type::regular;
	}

	return true;
}

inline bool set_win9x_dirent_first(win9x_dirent &entry)
{
	return set_win9x_dirent(entry, true);
}

inline bool win9x_dirent_next(win9x_dirent &entry)
{
	return set_win9x_dirent(entry, false);
}

} // namespace win32::details

struct win9x_directory_entry
{
	using native_char_type = char8_t;
	using char_type = char8_t;
	win9x_dirent *entry{};
	template <nt_family family, ::std::integral ch_type>
	explicit constexpr operator basic_nt_family_io_observer<family, ch_type>() const noexcept
	{
		return {entry->d_handle.handle};
	}
	template <win32_family family, ::std::integral ch_type>
	explicit constexpr operator basic_win32_family_io_observer<family, ch_type>() const noexcept
	{
		return {entry->d_handle.handle};
	}
	explicit constexpr operator win9x_dir_io_observer() const noexcept
	{
		return {entry->d_handle};
	}
};

inline constexpr win9x_at_entry at(win9x_directory_entry ndet) noexcept
{
	return win9x_at_entry{ndet.entry->d_handle};
}

inline constexpr ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t>
native_filename(win9x_directory_entry pioe) noexcept
{
	auto &ent{*pioe.entry};
	return {ent.filename.c_str(), ent.filename.size()};
}

inline constexpr win9x_fs_dirent drt(win9x_directory_entry pioe) noexcept
{
	auto &ent{*pioe.entry};
	return win9x_fs_dirent{ent.d_handle, {ent.filename.c_str(), ent.filename.size()}};
}

inline ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t>
u8filename(win9x_directory_entry pioe) noexcept
{
	using char8_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;
	auto &ent{*pioe.entry};

	return {reinterpret_cast<char8_may_alias_const_ptr>(ent.filename.c_str()), ent.filename.size()};
}

inline constexpr ::std::uint_least64_t inode_ul64(win9x_directory_entry) noexcept
{
	return 0;
}

inline constexpr file_type type(win9x_directory_entry pioe) noexcept
{
	return pioe.entry->d_type;
}

inline bool is_dot(win9x_directory_entry ent) noexcept
{
	::std::size_t const native_d_namlen{ent.entry->filename.size()};
	char8_t const *native_d_name_ptr{ent.entry->filename.c_str()};
	return ((native_d_namlen == 1 && *native_d_name_ptr == u8'.') ||
			(native_d_namlen == 2 && *native_d_name_ptr == u8'.' && native_d_name_ptr[1] == u8'.'));
}

struct win9x_family_directory_iterator
{
	win9x_dirent *entry{};
	bool finish{};
};

inline win9x_directory_entry operator*(win9x_family_directory_iterator pdit) noexcept
{
	return {pdit.entry};
}

inline win9x_family_directory_iterator &operator++(win9x_family_directory_iterator &pdit)
{
	if (!win32::details::win9x_dirent_next(*pdit.entry)) [[unlikely]]
	{
		pdit.finish = true;
	}

	return pdit;
}

inline constexpr bool operator==(::std::default_sentinel_t, win9x_family_directory_iterator b) noexcept
{
	return b.finish;
}

inline constexpr bool operator==(win9x_family_directory_iterator b, ::std::default_sentinel_t other) noexcept
{
	return b.finish;
}

inline constexpr bool operator!=(::std::default_sentinel_t other, win9x_family_directory_iterator b) noexcept
{
	return !b.finish;
}

inline constexpr bool operator!=(win9x_family_directory_iterator b, ::std::default_sentinel_t other) noexcept
{
	return !b.finish;
}

struct basic_win9x_directory_generator
{
	win9x_dirent entry{};
	constexpr basic_win9x_directory_generator() noexcept = default;
	explicit constexpr basic_win9x_directory_generator(::fast_io::win9x_dir_handle directory_handle) noexcept
	{
		entry.d_handle = ::std::move(directory_handle);
	}

	basic_win9x_directory_generator(basic_win9x_directory_generator const &) = default;
	basic_win9x_directory_generator &operator=(basic_win9x_directory_generator const &) = default;
	basic_win9x_directory_generator(basic_win9x_directory_generator &&__restrict other) noexcept
		: entry(::std::move(other.entry))
	{
		other.entry.d_handle.handle = nullptr;
	}
	basic_win9x_directory_generator &
	operator=(basic_win9x_directory_generator &&__restrict other) noexcept
	{
		entry = ::std::move(other.entry);
		other.entry.d_handle.handle = nullptr;
		return *this;
	}
	~basic_win9x_directory_generator()
	{
		entry.d_handle.handle = nullptr;
	}
};

inline win9x_family_directory_iterator begin(basic_win9x_directory_generator &pdg)
{
	win32::details::set_win9x_dirent_first(pdg.entry);
	return {__builtin_addressof(pdg.entry), false};
}

inline ::std::default_sentinel_t end(basic_win9x_directory_generator const &) noexcept
{
	return {};
}

inline basic_win9x_directory_generator current(win9x_at_entry nate)
{
	return basic_win9x_directory_generator(::std::move(nate.handle));
}

template <typename StackType>
struct basic_win9x_recursive_directory_iterator
{
	using stack_type = StackType;
	::fast_io::win9x_dir_handle root_handle{};
	void *root_file_struct{};
	win9x_dirent *entry{};
	stack_type stack;
	bool finish{};
	constexpr basic_win9x_recursive_directory_iterator() = default;

	explicit constexpr basic_win9x_recursive_directory_iterator(::fast_io::win9x_dir_handle rh, win9x_dirent *dp)
		: root_handle(::std::move(rh)), entry(dp)
	{}

	basic_win9x_recursive_directory_iterator(basic_win9x_recursive_directory_iterator const &) = delete;
	basic_win9x_recursive_directory_iterator &operator=(basic_win9x_recursive_directory_iterator const &) = delete;
	basic_win9x_recursive_directory_iterator(basic_win9x_recursive_directory_iterator &&) noexcept = default;
	basic_win9x_recursive_directory_iterator &operator=(basic_win9x_recursive_directory_iterator &&) noexcept = default;
};

template <typename StackType>
struct basic_win9x_recursive_directory_generator
{
	using stack_type = StackType;
	::fast_io::win9x_dir_handle root_handle{};
	win9x_dirent entry{};

	constexpr basic_win9x_recursive_directory_generator() = default;
	explicit constexpr basic_win9x_recursive_directory_generator(::fast_io::win9x_dir_handle rhd)
		: root_handle(::std::move(rhd))
	{
	}
	basic_win9x_recursive_directory_generator(basic_win9x_recursive_directory_generator const &) = delete;
	basic_win9x_recursive_directory_generator &
	operator=(basic_win9x_recursive_directory_generator const &) = delete;
	constexpr basic_win9x_recursive_directory_generator(
		basic_win9x_recursive_directory_generator &&__restrict other) noexcept
		: root_handle(::std::move(other.root_handle)), entry(::std::move(other.entry))
	{
		other.root_handle.handle = nullptr;
		entry.d_handle.handle = nullptr;
	}
	constexpr basic_win9x_recursive_directory_generator &
	operator=(basic_win9x_recursive_directory_generator &&__restrict other) noexcept
	{
		root_handle = ::std::move(other.root_handle);
		entry = ::std::move(other.entry);
		return *this;
	}
	constexpr ~basic_win9x_recursive_directory_generator()
	{
	}
};

struct win9x_dir_file_stack_type
{
	win9x_dir fd{};
	void *file_struct{};

	~win9x_dir_file_stack_type()
	{
		if (file_struct) [[likely]]
		{
			::fast_io::win32::FindClose(file_struct);
		}
	}
};

using win9x_recursive_directory_generator =
	basic_win9x_recursive_directory_generator<::fast_io::containers::vector<win9x_dir_file_stack_type, ::fast_io::win32_family_api_encoding_converter<win32_family::ansi_9x>::allocator_type>>;

template <typename StackType>
inline ::std::size_t depth(basic_win9x_recursive_directory_iterator<StackType> const &prdit) noexcept
{
	return prdit.stack.size();
}

template <typename StackType>
inline basic_win9x_recursive_directory_iterator<StackType> &operator++(basic_win9x_recursive_directory_iterator<StackType> &prdit)
{
	for (;;)
	{
		prdit.finish = false;
		if (prdit.stack.empty())
		{
			prdit.entry->d_handle = prdit.root_handle;
			prdit.entry->file_struct = prdit.root_file_struct;
			if (!win32::details::win9x_dirent_next(*prdit.entry))
			{
				prdit.finish = true;
				return prdit;
			}
		}
		else
		{
			auto &back{prdit.stack.back()};
			prdit.entry->d_handle = back.fd.handle;
			prdit.entry->file_struct = back.file_struct;
			if (back.file_struct == nullptr)
			{
				win32::details::set_win9x_dirent_first(*prdit.entry);
				back.file_struct = prdit.entry->file_struct;
			}
			if (!win32::details::win9x_dirent_next(*prdit.entry))
			{
				prdit.finish = true;
				prdit.stack.pop_back();

				continue;
			}
		}
		if (prdit.entry->d_type == file_type::directory)
		{
			::std::size_t const native_d_namlen{prdit.entry->filename.size()};
			char8_t const *native_d_name_ptr{prdit.entry->filename.c_str()};
			if ((native_d_namlen == 1 && *native_d_name_ptr == u8'.') ||
				(native_d_namlen == 2 && *native_d_name_ptr == u8'.' && native_d_name_ptr[1] == u8'.'))
			{
				continue;
			}
			prdit.stack.emplace_back(
				win9x_dir{win9x_at_entry{prdit.stack.empty() ? prdit.root_handle : prdit.stack.back().fd.handle},
						  ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t>{native_d_name_ptr, native_d_namlen},
						  open_mode::directory},
				nullptr);
		}
		return prdit;
	}
	return prdit;
}

template <typename StackType>
inline void pop(basic_win9x_recursive_directory_iterator<StackType> &prdit)
{
	if (prdit.stack.empty())
	{
		prdit.entry = nullptr;
	}
	else
	{
		prdit.stack.pop_back();
		++prdit;
	}
}

template <typename StackType>
inline basic_win9x_recursive_directory_iterator<StackType>
begin(basic_win9x_recursive_directory_generator<StackType> &prg) noexcept
{
	basic_win9x_recursive_directory_iterator<StackType> prdit{prg.root_handle, __builtin_addressof(prg.entry)};

	prdit.entry->d_handle = prg.root_handle;
	bool finish{win32::details::set_win9x_dirent_first(*prdit.entry)};
	prdit.root_file_struct = prdit.entry->file_struct;

	if (finish && prdit.entry->d_type == file_type::directory)
	{
		auto &ent{*prdit.entry};
		char8_t const *native_d_name_ptr{ent.filename.c_str()};
		::std::size_t const native_d_namlen{ent.filename.size()};
		if ((native_d_namlen == 1 && *native_d_name_ptr == u8'.') ||
			(native_d_namlen == 2 && *native_d_name_ptr == u8'.' && native_d_name_ptr[1] == u8'.'))
		{
			++prdit;
		}
		else
		{
			prdit.stack.emplace_back(
				win9x_dir{win9x_at_entry{prdit.root_handle},
						  ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t>{native_d_name_ptr, native_d_namlen},
						  open_mode::directory},
				nullptr);
		}
	}
	return prdit;
}

template <typename StackType>
inline ::std::default_sentinel_t
end(basic_win9x_recursive_directory_generator<StackType> const &) noexcept
{
	return {};
}

template <typename StackType>
inline win9x_directory_entry
operator*(basic_win9x_recursive_directory_iterator<StackType> const &prdit) noexcept
{
	return {prdit.entry};
}

template <typename StackType>
inline bool operator==(::std::default_sentinel_t,
					   basic_win9x_recursive_directory_iterator<StackType> const &b) noexcept
{
	return b.stack.empty() && b.finish;
}

template <typename StackType>
inline bool operator==(basic_win9x_recursive_directory_iterator<StackType> const &b,
					   ::std::default_sentinel_t sntnl) noexcept
{
	return b.stack.empty() && b.finish;
}

template <typename StackType>
inline bool operator!=(::std::default_sentinel_t sntnl,
					   basic_win9x_recursive_directory_iterator<StackType> const &b) noexcept
{
	return !(b.stack.empty() && b.finish);
}

template <typename StackType>
inline bool operator!=(basic_win9x_recursive_directory_iterator<StackType> const &b,
					   ::std::default_sentinel_t sntnl) noexcept
{
	return !(b.stack.empty() && b.finish);
}

inline win9x_recursive_directory_generator recursive(win9x_at_entry nate)
{
	return win9x_recursive_directory_generator{nate.handle};
}

inline auto native_extension(win9x_directory_entry ent) noexcept
{
	auto &et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<false, char8_t, char8_t>(et.filename.c_str(), et.filename.size());
}

inline auto native_stem(win9x_directory_entry ent) noexcept
{
	auto &et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<true, char8_t, char8_t>(et.filename.c_str(), et.filename.size());
}

inline auto u8extension(win9x_directory_entry ent) noexcept
{
	auto &et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<false, char8_t, char8_t>(et.filename.c_str(), et.filename.size());
}

inline auto u8stem(win9x_directory_entry ent) noexcept
{
	auto &et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<true, char8_t, char8_t>(et.filename.c_str(), et.filename.size());
}

#ifndef __CYGWIN__
using native_directory_entry = win9x_directory_entry;
#endif


} // namespace fast_io
