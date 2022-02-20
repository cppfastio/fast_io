#pragma once

namespace fast_io
{

template<stream T,open_mode interface_mode>
struct basic_file_wrapper:public T
{
	using typename T::char_type;
	using typename T::native_handle_type;
	using typename T::file_factory_type;
	constexpr basic_file_wrapper() noexcept=default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr basic_file_wrapper(native_hd hd) noexcept:T(hd){}
	explicit constexpr basic_file_wrapper(file_factory_type&& factory) noexcept:T(static_cast<file_factory_type&&>(factory)){}
	constexpr basic_file_wrapper(native_fs_dirent fsdirent,open_mode m=interface_mode,perms p=static_cast<perms>(436)):
		T(fsdirent,m|interface_mode,p){}

	template<::fast_io::constructible_to_os_c_str pathtype>
	explicit constexpr basic_file_wrapper(pathtype const& filename,open_mode om=interface_mode,perms pm=static_cast<perms>(436)):T(filename,om|interface_mode,pm){}
	template<::fast_io::constructible_to_os_c_str pathtype>
	explicit constexpr basic_file_wrapper(native_at_entry nate,pathtype const& filename,open_mode om=interface_mode,perms pm=static_cast<perms>(436)):T(nate,filename,om|interface_mode,pm){}

	explicit constexpr basic_file_wrapper(io_temp_t):T(io_temp){}
};

template<input_stream T>
using input_file_wrapper = basic_file_wrapper<T,open_mode::in>;
template<output_stream T>
using output_file_wrapper = basic_file_wrapper<T,open_mode::out>;

template<stream T>
using directory_file_wrapper = basic_file_wrapper<T,open_mode::directory>;


}
