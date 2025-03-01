#pragma once

namespace fast_io
{
struct win32_9xa_dirent
{
	void *file_struct{};
	::fast_io::win32::details::win32_9xa_dir_handle_path_str find_path{};

	::fast_io::win32_9xa_dir_handle d_handle{};
	file_type d_type{};
	[[maybe_unused]] ::std::uint_least64_t d_ino{};
	::fast_io::win32::details::win32_9xa_dir_handle_path_str filename{};

	inline constexpr ~win32_9xa_dirent()
	{
		if (file_struct) [[likely]]
		{
			::fast_io::win32::FindClose(file_struct);
		}
	}
};

namespace win32::details
{
inline bool set_win32_9xa_dirent(win32_9xa_dirent &entry, bool start)
{
	::fast_io::win32::win32_find_dataa wfda{};
	if (start)
	{
		entry.find_path = ::fast_io::win32::details::concat_win32_9xa_dir_handle_path_str(::fast_io::mnp::code_cvt(entry.d_handle.path), u8"\\*");
		entry.file_struct = ::fast_io::win32::FindFirstFileA(reinterpret_cast<char const *>(entry.find_path.c_str()), __builtin_addressof(wfda));
	}
	else
	{
		if (::fast_io::win32::FindNextFileA(entry.file_struct, __builtin_addressof(wfda)) == 0) [[unlikely]]
		{
			return false;
		}
	}

	entry.filename = ::fast_io::win32::details::concat_win32_9xa_dir_handle_path_str(::fast_io::mnp::code_cvt_os_c_str(wfda.cFileName));

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

inline bool set_win32_9xa_dirent_first(win32_9xa_dirent &entry)
{
	return set_win32_9xa_dirent(entry, true);
}

inline bool win32_9xa_dirent_next(win32_9xa_dirent &entry)
{
	return set_win32_9xa_dirent(entry, false);
}

} // namespace win32::details

struct win32_9xa_directory_entry
{
	using native_char_type = char8_t;
	using char_type = char8_t;
	win32_9xa_dirent *entry{};
	inline explicit constexpr operator win32_9xa_dir_io_observer() const noexcept
	{
		return {entry->d_handle};
	}
};

inline constexpr win32_9xa_at_entry at(win32_9xa_directory_entry ndet) noexcept
{
	return win32_9xa_at_entry{ndet.entry->d_handle};
}

inline constexpr ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t>
native_filename(win32_9xa_directory_entry pioe) noexcept
{
	auto &ent{*pioe.entry};
	return {ent.filename.c_str(), ent.filename.size()};
}

inline constexpr win32_9xa_fs_dirent drt(win32_9xa_directory_entry pioe) noexcept
{
	auto &ent{*pioe.entry};
	return win32_9xa_fs_dirent{ent.d_handle, {ent.filename.c_str(), ent.filename.size()}};
}

inline ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t>
u8filename(win32_9xa_directory_entry pioe) noexcept
{
	using char8_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;
	auto &ent{*pioe.entry};

	return {reinterpret_cast<char8_may_alias_const_ptr>(ent.filename.c_str()), ent.filename.size()};
}

inline constexpr ::std::uint_least64_t inode_ul64(win32_9xa_directory_entry) noexcept
{
	return 0;
}

inline constexpr file_type type(win32_9xa_directory_entry pioe) noexcept
{
	return pioe.entry->d_type;
}

inline bool is_dot(win32_9xa_directory_entry ent) noexcept
{
	::std::size_t const native_d_namlen{ent.entry->filename.size()};
	char8_t const *native_d_name_ptr{ent.entry->filename.c_str()};
	return ((native_d_namlen == 1 && *native_d_name_ptr == u8'.') ||
			(native_d_namlen == 2 && *native_d_name_ptr == u8'.' && native_d_name_ptr[1] == u8'.'));
}

struct win32_9xa_family_directory_iterator
{
	win32_9xa_dirent *entry{};
	bool finish{};
};

inline win32_9xa_directory_entry operator*(win32_9xa_family_directory_iterator pdit) noexcept
{
	return {pdit.entry};
}

inline win32_9xa_family_directory_iterator &operator++(win32_9xa_family_directory_iterator &pdit)
{
	if (!win32::details::win32_9xa_dirent_next(*pdit.entry)) [[unlikely]]
	{
		pdit.finish = true;
	}

	return pdit;
}

inline constexpr bool operator==(::std::default_sentinel_t, win32_9xa_family_directory_iterator b) noexcept
{
	return b.finish;
}

inline constexpr bool operator==(win32_9xa_family_directory_iterator b, [[maybe_unused]] ::std::default_sentinel_t other) noexcept
{
	return b.finish;
}

inline constexpr bool operator!=([[maybe_unused]] ::std::default_sentinel_t other, win32_9xa_family_directory_iterator b) noexcept
{
	return !b.finish;
}

inline constexpr bool operator!=(win32_9xa_family_directory_iterator b, [[maybe_unused]] ::std::default_sentinel_t other) noexcept
{
	return !b.finish;
}

struct basic_win32_9xa_directory_generator
{
	win32_9xa_dirent entry{};
	inline constexpr basic_win32_9xa_directory_generator() noexcept = default;
	inline explicit constexpr basic_win32_9xa_directory_generator(::fast_io::win32_9xa_dir_handle directory_handle) noexcept
	{
		entry.d_handle = ::std::move(directory_handle);
	}

	inline basic_win32_9xa_directory_generator(basic_win32_9xa_directory_generator const &) = default;
	inline basic_win32_9xa_directory_generator &operator=(basic_win32_9xa_directory_generator const &) = default;
	inline basic_win32_9xa_directory_generator(basic_win32_9xa_directory_generator &&__restrict other) noexcept
		: entry(::std::move(other.entry))
	{
	}
	inline basic_win32_9xa_directory_generator &
	operator=(basic_win32_9xa_directory_generator &&__restrict other) noexcept
	{
		// There is no need to check the 'this' pointer as there are no side effects
		entry = ::std::move(other.entry);
		return *this;
	}
	inline ~basic_win32_9xa_directory_generator()
	{
	}
};

inline win32_9xa_family_directory_iterator begin(basic_win32_9xa_directory_generator &pdg)
{
	win32::details::set_win32_9xa_dirent_first(pdg.entry);
	return {__builtin_addressof(pdg.entry), false};
}

inline ::std::default_sentinel_t end(basic_win32_9xa_directory_generator const &) noexcept
{
	return {};
}

inline basic_win32_9xa_directory_generator current(win32_9xa_at_entry nate)
{
	return basic_win32_9xa_directory_generator(::std::move(nate.handle));
}

template <typename StackType>
struct basic_win32_9xa_recursive_directory_iterator
{
	using stack_type = StackType;
	::fast_io::win32_9xa_dir_handle root_handle{};
	void *root_file_struct{};
	win32_9xa_dirent *entry{};
	stack_type stack;
	bool finish{};
	inline constexpr basic_win32_9xa_recursive_directory_iterator() = default;

	inline explicit constexpr basic_win32_9xa_recursive_directory_iterator(::fast_io::win32_9xa_dir_handle rh, win32_9xa_dirent *dp)
		: root_handle(::std::move(rh)), entry(dp)
	{}

	inline basic_win32_9xa_recursive_directory_iterator(basic_win32_9xa_recursive_directory_iterator const &) = delete;
	inline basic_win32_9xa_recursive_directory_iterator &operator=(basic_win32_9xa_recursive_directory_iterator const &) = delete;
	inline basic_win32_9xa_recursive_directory_iterator(basic_win32_9xa_recursive_directory_iterator &&) noexcept = default;
	inline basic_win32_9xa_recursive_directory_iterator &operator=(basic_win32_9xa_recursive_directory_iterator &&) noexcept = default;
};

template <typename StackType>
struct basic_win32_9xa_recursive_directory_generator
{
	using stack_type = StackType;
	::fast_io::win32_9xa_dir_handle root_handle{};
	win32_9xa_dirent entry{};

	inline constexpr basic_win32_9xa_recursive_directory_generator() = default;
	inline explicit constexpr basic_win32_9xa_recursive_directory_generator(::fast_io::win32_9xa_dir_handle rhd)
		: root_handle(::std::move(rhd))
	{
	}
	inline basic_win32_9xa_recursive_directory_generator(basic_win32_9xa_recursive_directory_generator const &) = delete;
	inline basic_win32_9xa_recursive_directory_generator &
	operator=(basic_win32_9xa_recursive_directory_generator const &) = delete;
	inline constexpr basic_win32_9xa_recursive_directory_generator(
		basic_win32_9xa_recursive_directory_generator &&__restrict other) noexcept
		: root_handle(::std::move(other.root_handle)), entry(::std::move(other.entry))
	{
	}
	inline constexpr basic_win32_9xa_recursive_directory_generator &
	operator=(basic_win32_9xa_recursive_directory_generator &&__restrict other) noexcept
	{
		// There is no need to check the 'this' pointer as there are no side effects
		root_handle = ::std::move(other.root_handle);
		entry = ::std::move(other.entry);
		return *this;
	}
	inline constexpr ~basic_win32_9xa_recursive_directory_generator()
	{
	}
};

struct win32_9xa_dir_file_stack_type
{
	win32_9xa_dir_file dirf{};
	void *file_struct{};

	inline win32_9xa_dir_file_stack_type() noexcept = default;
	inline win32_9xa_dir_file_stack_type(win32_9xa_dir_file o_dirf, void *o_file_struct) noexcept
		: dirf(::std::move(o_dirf)), file_struct(o_file_struct)
	{}

	inline win32_9xa_dir_file_stack_type(win32_9xa_dir_file_stack_type const &) = delete;
	inline win32_9xa_dir_file_stack_type &operator=(win32_9xa_dir_file_stack_type const &) = delete;

	inline win32_9xa_dir_file_stack_type(win32_9xa_dir_file_stack_type &&other) noexcept
		: dirf(::std::move(other.dirf)), file_struct(other.file_struct)
	{
		other.file_struct = nullptr;
	}

	inline win32_9xa_dir_file_stack_type &operator=(win32_9xa_dir_file_stack_type &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}

		if (this->file_struct) [[likely]]
		{
			::fast_io::win32::FindClose(this->file_struct);
		}
		dirf = ::std::move(other.dirf);
		this->file_struct = other.file_struct;
		other.file_struct = nullptr;

		return *this;
	}

	inline ~win32_9xa_dir_file_stack_type()
	{
		if (file_struct) [[likely]]
		{
			::fast_io::win32::FindClose(file_struct);
		}
	}
};

using win32_9xa_recursive_directory_generator =
	basic_win32_9xa_recursive_directory_generator<::fast_io::containers::vector<win32_9xa_dir_file_stack_type, ::fast_io::win32_family_api_encoding_converter<win32_family::ansi_9x>::allocator_type>>;

template <typename StackType>
inline ::std::size_t depth(basic_win32_9xa_recursive_directory_iterator<StackType> const &prdit) noexcept
{
	return prdit.stack.size();
}

template <typename StackType>
inline basic_win32_9xa_recursive_directory_iterator<StackType> &operator++(basic_win32_9xa_recursive_directory_iterator<StackType> &prdit)
{
	for (;;)
	{
		prdit.finish = false;
		if (prdit.stack.empty())
		{
			prdit.entry->d_handle = prdit.root_handle;
			prdit.entry->file_struct = prdit.root_file_struct;
			if (!win32::details::win32_9xa_dirent_next(*prdit.entry))
			{
				prdit.finish = true;
				return prdit;
			}
		}
		else
		{
			auto &back{prdit.stack.back()};
			prdit.entry->d_handle = back.dirf.handle;
			prdit.entry->file_struct = back.file_struct;
			if (back.file_struct == nullptr)
			{
				win32::details::set_win32_9xa_dirent_first(*prdit.entry);
				back.file_struct = prdit.entry->file_struct;
			}
			if (!win32::details::win32_9xa_dirent_next(*prdit.entry))
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
				win32_9xa_dir_file{win32_9xa_at_entry{prdit.stack.empty() ? prdit.root_handle : prdit.stack.back().dirf.handle},
								   ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t>{native_d_name_ptr, native_d_namlen},
								   open_mode::directory},
				nullptr);
		}
		return prdit;
	}
	return prdit;
}

template <typename StackType>
inline void pop(basic_win32_9xa_recursive_directory_iterator<StackType> &prdit)
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
inline basic_win32_9xa_recursive_directory_iterator<StackType>
begin(basic_win32_9xa_recursive_directory_generator<StackType> &prg) noexcept
{
	basic_win32_9xa_recursive_directory_iterator<StackType> prdit{prg.root_handle, __builtin_addressof(prg.entry)};

	prdit.entry->d_handle = prg.root_handle;
	bool finish{win32::details::set_win32_9xa_dirent_first(*prdit.entry)};
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
				win32_9xa_dir_file{win32_9xa_at_entry{prdit.root_handle},
								   ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t>{native_d_name_ptr, native_d_namlen},
								   open_mode::directory},
				nullptr);
		}
	}
	return prdit;
}

template <typename StackType>
inline ::std::default_sentinel_t
end(basic_win32_9xa_recursive_directory_generator<StackType> const &) noexcept
{
	return {};
}

template <typename StackType>
inline win32_9xa_directory_entry
operator*(basic_win32_9xa_recursive_directory_iterator<StackType> const &prdit) noexcept
{
	return {prdit.entry};
}

template <typename StackType>
inline bool operator==(::std::default_sentinel_t,
					   basic_win32_9xa_recursive_directory_iterator<StackType> const &b) noexcept
{
	return b.stack.empty() && b.finish;
}

template <typename StackType>
inline bool operator==(basic_win32_9xa_recursive_directory_iterator<StackType> const &b,
					   [[maybe_unused]] ::std::default_sentinel_t sntnl) noexcept
{
	return b.stack.empty() && b.finish;
}

template <typename StackType>
inline bool operator!=([[maybe_unused]] ::std::default_sentinel_t sntnl,
					   basic_win32_9xa_recursive_directory_iterator<StackType> const &b) noexcept
{
	return !(b.stack.empty() && b.finish);
}

template <typename StackType>
inline bool operator!=(basic_win32_9xa_recursive_directory_iterator<StackType> const &b,
					   [[maybe_unused]] ::std::default_sentinel_t sntnl) noexcept
{
	return !(b.stack.empty() && b.finish);
}

inline win32_9xa_recursive_directory_generator recursive(win32_9xa_at_entry nate)
{
	return win32_9xa_recursive_directory_generator{::std::move(nate.handle)};
}

inline auto native_extension(win32_9xa_directory_entry ent) noexcept
{
	auto &et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<false, char8_t, char8_t>(et.filename.c_str(), et.filename.size());
}

inline auto native_stem(win32_9xa_directory_entry ent) noexcept
{
	auto &et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<true, char8_t, char8_t>(et.filename.c_str(), et.filename.size());
}

inline auto u8extension(win32_9xa_directory_entry ent) noexcept
{
	auto &et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<false, char8_t, char8_t>(et.filename.c_str(), et.filename.size());
}

inline auto u8stem(win32_9xa_directory_entry ent) noexcept
{
	auto &et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<true, char8_t, char8_t>(et.filename.c_str(), et.filename.size());
}

#if !defined(__CYGWIN__) && defined(_WIN32_WINDOWS)
using native_directory_entry = win32_9xa_directory_entry;
#endif

} // namespace fast_io
