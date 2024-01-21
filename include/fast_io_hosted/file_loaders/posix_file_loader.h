#pragma once

namespace fast_io
{

namespace details
{

inline char *posix_load_address_options(int fd, ::std::size_t file_size, ::fast_io::posix_mmap_options const &options)
{
    if (file_size == 0)
        return ::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1));
    return ::std::bit_cast<char *>(sys_mmap(options.addr, file_size, options.prot, options.flags, fd, 0));
}

inline char *posix_load_address(int fd, ::std::size_t file_size)
{
    if (file_size == 0)
        return ::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1));
    return ::std::bit_cast<char *>(sys_mmap(nullptr, file_size, PROT_READ | PROT_WRITE,
                                            MAP_PRIVATE
#if defined(MAP_POPULATE)
                                                | MAP_POPULATE
#endif
                                            ,
                                            fd, 0));
}

inline void posix_unload_address(void *address, [[maybe_unused]] ::std::size_t file_size) noexcept
{
    if (address != (void *)-1) [[likely]]
        sys_munmap(address, file_size);
}

struct posix_file_loader_return_value_t
{
    char *address_begin;
    char *address_end;
};

inline posix_file_loader_return_value_t posix_load_address_impl(int fd)
{
    ::std::size_t size{posix_loader_get_file_size(fd)};
    auto add{posix_load_address(fd, size)};
    auto ed{add};
    if (ed != ::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1)))
    {
        ed += size;
    }
    return {add, ed};
}

template <typename... Args> inline auto posix_load_file_impl(Args &&...args)
{
    posix_file pf(::fast_io::freestanding::forward<Args>(args)...);
    return posix_load_address_impl(pf.fd);
}

inline posix_file_loader_return_value_t posix_load_address_options_impl(::fast_io::posix_mmap_options const &options,
                                                                        int fd)
{
    ::std::size_t size{posix_loader_get_file_size(fd)};
    auto add{posix_load_address_options(fd, size, options)};
    auto ed{add};
    if (ed != ::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1)))
    {
        ed += size;
    }
    return {add, ed};
}

template <typename... Args>
inline auto posix_load_file_options_impl(::fast_io::posix_mmap_options const &options, Args &&...args)
{
    posix_file pf(::fast_io::freestanding::forward<Args>(args)...);
    return posix_load_address_options_impl(options, pf.fd);
}

} // namespace details

class posix_file_loader
{
  public:
    using value_type = char;
    using pointer = char *;
    using const_pointer = char const *;
    using const_iterator = const_pointer;
    using iterator = pointer;
    using reference = char &;
    using const_reference = char const &;
    using size_type = ::std::size_t;
    using difference_type = ::std::ptrdiff_t;
    using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
    using reverse_iterator = ::std::reverse_iterator<iterator>;

    pointer address_begin{};
    pointer address_end{};
    inline explicit constexpr posix_file_loader() noexcept
        : address_begin(::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1))),
          address_end(::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1)))
    {
    }
    inline explicit posix_file_loader(posix_at_entry pate)
    {
        auto ret{::fast_io::details::posix_load_address_impl(pate.fd)};
        address_begin = ret.address_begin;
        address_end = ret.address_end;
    }
    inline explicit posix_file_loader(native_fs_dirent fsdirent, open_mode om = open_mode::in,
                                      perms pm = static_cast<perms>(436))
    {
        auto ret{::fast_io::details::posix_load_file_impl(fsdirent, om, pm)};
        address_begin = ret.address_begin;
        address_end = ret.address_end;
    }
    template <::fast_io::constructible_to_os_c_str T>
    inline explicit posix_file_loader(T const &filename, open_mode om = open_mode::in,
                                      perms pm = static_cast<perms>(436))
    {
        auto ret{::fast_io::details::posix_load_file_impl(filename, om, pm)};
        address_begin = ret.address_begin;
        address_end = ret.address_end;
    }
    template <::fast_io::constructible_to_os_c_str T>
    inline explicit posix_file_loader(native_at_entry ent, T const &filename, open_mode om = open_mode::in,
                                      perms pm = static_cast<perms>(436))
    {
        auto ret{::fast_io::details::posix_load_file_impl(ent, filename, om, pm)};
        address_begin = ret.address_begin;
        address_end = ret.address_end;
    }
    inline explicit posix_file_loader(posix_mmap_options const &options, posix_at_entry pate)
    {
        auto ret{::fast_io::details::posix_load_address_options_impl(options, pate.fd)};
        address_begin = ret.address_begin;
        address_end = ret.address_end;
    }
    inline explicit posix_file_loader(posix_mmap_options const &options, native_fs_dirent fsdirent,
                                      open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
    {
        auto ret{::fast_io::details::posix_load_file_options_impl(options, fsdirent, om, pm)};
        address_begin = ret.address_begin;
        address_end = ret.address_end;
    }
    template <::fast_io::constructible_to_os_c_str T>
    inline explicit posix_file_loader(posix_mmap_options const &options, T const &filename,
                                      open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
    {
        auto ret{::fast_io::details::posix_load_file_options_impl(options, filename, om, pm)};
        address_begin = ret.address_begin;
        address_end = ret.address_end;
    }
    template <::fast_io::constructible_to_os_c_str T>
    inline explicit posix_file_loader(posix_mmap_options const &options, native_at_entry ent, T const &filename,
                                      open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
    {
        auto ret{::fast_io::details::posix_load_file_options_impl(options, ent, filename, om, pm)};
        address_begin = ret.address_begin;
        address_end = ret.address_end;
    }

    posix_file_loader(posix_file_loader const &) = delete;
    posix_file_loader &operator=(posix_file_loader const &) = delete;
    constexpr posix_file_loader(posix_file_loader &&__restrict other) noexcept
        : address_begin(other.address_begin), address_end(other.address_end)
    {
        other.address_end = other.address_begin = ::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1));
    }
    posix_file_loader &operator=(posix_file_loader &&__restrict other) noexcept
    {
        ::fast_io::details::posix_unload_address(address_begin,
                                                 static_cast<::std::size_t>(address_end - address_begin));
        address_begin = other.address_begin;
        address_end = other.address_end;
        other.address_end = other.address_begin = ::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1));
        return *this;
    }

    constexpr pointer data() const noexcept
    {
        return address_begin;
    }
    constexpr bool empty() const noexcept
    {
        return address_begin == address_end;
    }
    constexpr ::std::size_t size() const noexcept
    {
        return static_cast<::std::size_t>(address_end - address_begin);
    }
    constexpr const_iterator cbegin() const noexcept
    {
        return address_begin;
    }
    constexpr const_iterator begin() const noexcept
    {
        return address_begin;
    }
    constexpr iterator begin() noexcept
    {
        return address_begin;
    }
    constexpr const_iterator cend() const noexcept
    {
        return address_end;
    }
    constexpr const_iterator end() const noexcept
    {
        return address_end;
    }
    constexpr iterator end() noexcept
    {
        return address_end;
    }
    constexpr ::std::size_t max_size() const noexcept
    {
        return SIZE_MAX;
    }
    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator{address_end};
    }
    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator{address_end};
    }
    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator{address_end};
    }
    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator{address_begin};
    }
    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator{address_begin};
    }
    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator{address_begin};
    }
    constexpr const_reference front() const noexcept
    {
        return *address_begin;
    }
    constexpr reference front() noexcept
    {
        return *address_begin;
    }
    constexpr const_reference back() const noexcept
    {
        return address_end[-1];
    }
    constexpr reference back() noexcept
    {
        return address_end[-1];
    }
    inline constexpr reference operator[](size_type size) noexcept
    {
        return address_begin[size];
    }
    inline constexpr const_reference operator[](size_type size) const noexcept
    {
        return address_begin[size];
    }
    inline void close()
    {
        ::fast_io::details::posix_unload_address(address_begin,
                                                 static_cast<::std::size_t>(address_end - address_begin));
        address_end = address_begin = ::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1));
    }
#if __has_cpp_attribute(nodiscard)
    [[nodiscard]]
#endif
    inline constexpr pointer release() noexcept
    {
        pointer temp{address_begin};
        address_end = address_begin = ::std::bit_cast<char *>(static_cast<::std::ptrdiff_t>(-1));
        return temp;
    }
    ~posix_file_loader()
    {
        ::fast_io::details::posix_unload_address(address_begin,
                                                 static_cast<::std::size_t>(address_end - address_begin));
    }
};

inline constexpr basic_io_scatter_t<char> print_alias_define(io_alias_t, posix_file_loader const &load) noexcept
{
    return {load.data(), load.size()};
}

} // namespace fast_io
