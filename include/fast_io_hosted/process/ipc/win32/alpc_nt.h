#pragma once

namespace fast_io
{
namespace win32::nt::details
{
// https://hfiref0x.github.io/X86_64/NT6_syscalls.html
// nt6x alpc

using nt_alpc_char_type = char16_t;

using nt_alpc_internal_char_type = char16_t;

using nt_alpc_internal_str = ::fast_io::containers::basic_string<nt_alpc_internal_char_type, ::fast_io::native_global_allocator>;

using nt_alpc_internal_tlc_str = ::fast_io::containers::basic_string<nt_alpc_internal_char_type, ::fast_io::native_thread_local_allocator>;

using nt_alpc_internal_strvw = ::fast_io::containers::basic_string_view<nt_alpc_internal_char_type>;

template <typename... Args>
constexpr inline nt_alpc_internal_str concat_nt_alpc_internal_str(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<nt_alpc_internal_char_type>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, nt_alpc_internal_char_type, nt_alpc_internal_str>(
			::fast_io::io_print_forward<nt_alpc_internal_char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::win32::nt::details::nt_alpc_internal_str");
		return {};
	}
}

template <typename... Args>
constexpr inline nt_alpc_internal_tlc_str concat_nt_alpc_internal_tlc_str(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<nt_alpc_internal_char_type>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, nt_alpc_internal_char_type, nt_alpc_internal_tlc_str>(
			::fast_io::io_print_forward<nt_alpc_internal_char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::win32::nt::details::nt_alpc_internal_tlc_str");
		return {};
	}
}

template <nt_family family>
struct nt_alpc_server_handle
{
	using alpc_message_alloc = ::fast_io::native_thread_local_allocator;

	void *server_handle{};
	void *server_section_handle{};
	::fast_io::win32::nt::alpc_message_attributes *server_message_attribute{};
	::std::byte *view_begin{};
	::std::byte *view_end{};

	nt_alpc_server_handle() noexcept = default;

	nt_alpc_server_handle(nt_alpc_server_handle const &) = delete;
	nt_alpc_server_handle &operator=(nt_alpc_server_handle const &) = delete;

	inline constexpr nt_alpc_server_handle(nt_alpc_server_handle &&other) noexcept
	{
		server_handle = other.server_handle;
		other.server_handle = nullptr;
		server_section_handle = other.server_section_handle;
		other.server_section_handle = nullptr;
		server_message_attribute = other.server_message_attribute;
		other.server_message_attribute = nullptr;
	}

	inline constexpr nt_alpc_server_handle &operator=(nt_alpc_server_handle &&other) noexcept
	{
		constexpr bool zw{family == nt_family::zw};

		if (server_section_handle)
		{
			::fast_io::win32::nt::nt_alpc_disconnect_port<zw>(server_section_handle);
		}
		server_section_handle = other.server_section_handle;
		other.server_section_handle = nullptr;

		if (server_handle)
		{
			::fast_io::win32::nt::nt_alpc_disconnect_port<zw>(server_handle);
		}
		server_handle = other.server_handle;
		other.server_handle = nullptr;

		if (server_message_attribute)
		{
			alpc_message_alloc::deallocate(server_message_attribute);
		}
		server_message_attribute = other.server_message_attribute;
		other.server_message_attribute = nullptr;
	}

	inline ~nt_alpc_server_handle()
	{
		close();
	}

	inline void close() noexcept
	{
		constexpr bool zw{family == nt_family::zw};

		if (server_section_handle)
		{
			::fast_io::win32::nt::nt_alpc_disconnect_port<zw>(server_section_handle);
			server_section_handle = nullptr;
		}

		if (server_handle)
		{
			::fast_io::win32::nt::nt_alpc_disconnect_port<zw>(server_handle);
			server_handle = nullptr;
		}

		if (server_message_attribute)
		{
			alpc_message_alloc::deallocate(server_message_attribute);
			server_message_attribute = nullptr;
		}
	}
};

template <nt_family family>
inline void *nt_family_create_alpc_ipc_server_port_impl(nt_alpc_char_type const *server_name, ::std::size_t server_name_size, ::fast_io::ipc_mode mode)
{
	constexpr bool zw{family == nt_family::zw};

	if (!::fast_io::details::is_valid_os_file_name(server_name, server_name_size)) [[unlikely]]
	{
		throw_nt_error(3221225524);
	}

	auto temp_ipc_name_tlc_str{concat_nt_alpc_internal_tlc_str(u"\\RPC Control\\fast_io_ipc_", ::fast_io::mnp::os_c_str_with_known_size(server_name, server_name_size))};

	::fast_io::win32::nt::unicode_string us{};
	us.Buffer = const_cast<char16_t *>(temp_ipc_name_tlc_str.c_str());
	auto const temp_ipc_name_tlc_str_size_bytes{temp_ipc_name_tlc_str.size_bytes()};
	us.Length = static_cast<::std::uint_least16_t>(temp_ipc_name_tlc_str_size_bytes);
	us.MaximumLength = static_cast<::std::uint_least16_t>(temp_ipc_name_tlc_str_size_bytes + sizeof(char16_t));

	::fast_io::win32::nt::object_attributes oa{};
	oa.Length = sizeof(::fast_io::win32::nt::object_attributes);
	oa.ObjectName = __builtin_addressof(us);

	::fast_io::win32::nt::alpc_port_attributes apa{};
	apa.Flags = 0x80000 /*ALPC_PORTFLG_ALLOW_DUP_OBJECT*/;
	apa.MaxMessageLength = ::fast_io::win32::nt::AlpcMaxAllowedMessageLength();
	apa.MemoryBandwidth = 0;     // default
	apa.MaxPoolUsage = 0;        // default
	apa.MaxSectionSize = 0;      // default
	apa.MaxViewSize = 0;         // default
	apa.MaxTotalSectionSize = 0; // default
	apa.DupObjectTypes = 0;      // default

	void *server_port_handle;

	check_nt_status(::fast_io::win32::nt::nt_alpc_create_port<zw>(
		__builtin_addressof(server_port_handle),
		__builtin_addressof(oa),
		__builtin_addressof(apa)));

	return server_port_handle;
}

template <nt_family family>
struct nt_family_create_alpc_ipc_server_paramenter
{
	using family_char_type = ::std::conditional_t<family == win32_family::wide_nt, char16_t, char>;
	ipc_mode im{};
	inline void *operator()(family_char_type const *filename, ::std::size_t filename_size)
	{
		return nt_family_create_alpc_ipc_server_port_impl<family>(filename, filename_size, im);
	}
};

template <nt_family family, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *nt_create_alpc_ipc_server_impl(T const &t, ipc_mode im)
{
	return ::fast_io::nt_api_common(t, nt_family_create_alpc_ipc_server_paramenter<family>{im});
}

template <nt_family family>
inline ::fast_io::win32::nt::alpc_message_attributes *nt_family_create_alpc_ipc_server_message_attribute_view_impl(void *server_port)
{
	constexpr bool zw{family == nt_family::zw};

	constexpr ::std::uint_least32_t message_attribute{
		0x80000000 /*ALPC_MESSAGE_SECURITY_ATTRIBUTE*/ |
		0x40000000 /*ALPC_MESSAGE_VIEW_ATTRIBUTE*/};

	auto const header_size{::fast_io::win32::nt::AlpcGetHeaderSize(message_attribute)};

	if (header_size == 0) [[unlikely]]
	{
		throw_nt_error(3221225485);
	}

	auto message_stroge{nt_alpc_server_handle<family>::alpc_message_alloc::allocate(header_size)}; // This function does not recycle

	using alpc_message_attributes_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::win32::nt::alpc_message_attributes *;

	auto p_msg_attr_aend{reinterpret_cast<alpc_message_attributes_may_alias_ptr>(message_stroge)};

	::std::size_t real_buf_size;

	check_nt_status(::fast_io::win32::nt::AlpcInitializeMessageAttribute(
		message_attribute,                 // the MessageAttribute
		p_msg_attr_aend,                   // pointer to allocated buffer that is used to holf attributes structures
		header_size,                       // buffer that has been allocated
		__builtin_addressof(real_buf_size) // the size that would be needed (in case of the buffer allocated was too small)
		));

	::std::size_t iNextMsgAttrBufferOffset{sizeof(::fast_io::win32::nt::alpc_message_attributes)}; // 4 bytes allocated attributes + 4 bytes valid attributes

	// ALPC_MESSAGE_SECURITY_ATTRIBUTE
	::fast_io::win32::nt::security_quality_of_service SecurityQos{};
	::fast_io::win32::nt::alpc_security_attr securityAttr{};

	SecurityQos.ImpersonationLevel = ::fast_io::win32::nt::security_impersonation_level::SecurityAnonymous; // SecurityIdentification;
	SecurityQos.ContextTrackingMode = 0 /*SECURITY_STATIC_TRACKING*/;
	SecurityQos.EffectiveOnly = 0;
	SecurityQos.Length = sizeof(SecurityQos);
	securityAttr.pQOS = __builtin_addressof(SecurityQos);
	securityAttr.Flags = 0; // 0x10000;
	check_nt_status(::fast_io::win32::nt::nt_alpc_create_security_context<zw>(server_port_handle, 0, __builtin_addressof(securityAttr)));
	::fast_io::freestanding::my_memmove(reinterpret_cast<::std::byte *>(p_msg_attr_aend) + iNextMsgAttrBufferOffset, __builtin_addressof(securityAttr), sizeof(securityAttr));
	iNextMsgAttrBufferOffset += sizeof(securityAttr);

	// ALPC_MESSAGE_VIEW_ATTRIBUTE
	::fast_io::win32::nt::alpc_data_view_attr viewAttr{};
	viewAttr.Flags = 0;         // unknown
	viewAttr.SectionHandle = 0; // Future allocation
	viewAttr.ViewBase = 0;      // Automatic assign
	viewAttr.ViewSize = 0;      // Future allocation

	// place ALPC_MESSAGE_VIEW_ATTRIBUTE structure
	::fast_io::freestanding::my_memmove(reinterpret_cast<::std::byte *>(p_msg_attr_aend) + iNextMsgAttrBufferOffset, __builtin_addressof(viewAttr), sizeof(viewAttr));
	iNextMsgAttrBufferOffset += sizeof(viewAttr);

	return p_msg_attr_aend;
}

template <nt_family family>
inline void *nt_family_add_section_view_to_alpc_ipc_server_message_attribute_view_impl(
	void *__restrict server_port_handle, ::fast_io::win32::nt::alpc_message_attributes *ama, ::std::size_t view_size)
{
	constexpr bool zw{family == nt_family::zw};

	void *server_section_handle;
	::std::size_t server_section_size;

	check_nt_status(::fast_io::win32::nt::nt_alpc_create_port_section<zw>(
		server_port_handle,                         //_In_ HANDLE PortHandle,
		0,                                          //_In_ ULONG Flags, unknown, 0x40000 found in rpcrt4.dll
		nullptr,                                    //_In_opt_ HANDLE SectionHandle,
		0x1000,                                     // _In_ SIZE_T SectionSize,
		__builtin_addressof(server_section_handle), //_Out_ HANDLE AlpcSectionHandle,
		__builtin_addressof(server_section_size)    //_Out_ PSIZE_T ActualSectionSize
		));

	::fast_io::win32::nt::alpc_data_view_attr viewAttr{};
	viewAttr.Flags = 0; // unknown
	viewAttr.SectionHandle = server_section_handle;
	viewAttr.ViewBase = 0; // Automatic assign
	viewAttr.ViewSize = view_size;
	check_nt_status(::fast_io::win32::nt::nt_alpc_create_section_view<zw>(
		server_port_handle,           //_In_ HANDLE PortHandle,
		0,                            // _Reserved_ ULONG Flags, unknown
		__builtin_addressof(viewAttr) //_Inout_ PALPC_DATA_VIEW_ATTR ViewAttributes
		));

	::std::size_t iNextMsgAttrBufferOffset{sizeof(::fast_io::win32::nt::alpc_message_attributes)};
	iNextMsgAttrBufferOffset += sizeof(::fast_io::win32::nt::security_quality_of_service);
	::fast_io::freestanding::my_memmove(reinterpret_cast<::std::byte *>(ama) + iNextMsgAttrBufferOffset, __builtin_addressof(viewAttr), sizeof(viewAttr));

	return server_section_handle;
}

template <nt_family family>
inline ::std::byte *nt_family_alpc_ipc_server_wait_for_connect_impl(
	void *server_pipe_handle, ::fast_io::win32::nt::alpc_message_attributes *ama,
	::std::byte *handshake_msg_beg, ::std::byte *handshake_msg_end)
{
	constexpr bool zw{family == nt_family::zw};

	::std::size_t receive_size{sizeof(::fast_io::win32::nt::port_message) + static_cast<::std::size_t>(handshake_msg_end - handshake_msg_beg)};
	auto tmp{static_cast<::fast_io::win32::nt::alpc_message *>(
#if __has_builtin(__builtin_alloca)
		__builtin_alloca
#elif defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__) && !defined(__CYGWIN__)
		_alloca
#else
		alloca
#endif
		(receive_size)};

	using port_message_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::win32::nt::port_message *;

	check_nt_status(::fast_io::win32::nt::nt_alpc_send_wait_receive_port<zw>(
		server_pipe_handle,
		0,                                                                      // no flags
		nullptr,                                                                // SendMessage
		nullptr,                                                                // SendMessageAttributes
		reinterpret_cast<port_message_may_alias_ptr>(__builtin_addressof(tmp)), // ReceiveBuffer
		__builtin_addressof(receive_size),                                      // BufferLength
		ama,                                                                    // ReceiveMessageAttributes
		nullptr                                                                 // no timeout
		));

	auto const actual_receive_size{static_cast<::std::size_t>(tmp.u1.Length)};

	return static_cast<::std::byte *>(::fast_io::freestanding::my_memcpy(handshake_msg_beg, tmp->PortMessage, actual_receive_size));
}

template <nt_family family>
inline void nt_family_alpc_ipc_server_disconnect_impl(void *client_pipe_handle)
{
	constexpr bool zw{family == nt_family::zw};

	check_nt_status(::fast_io::win32::nt::nt_alpc_disconnect_port<zw>(client_pipe_handle, 0););
}


} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
class basic_nt_family_alpc_ipc_server_observer
{
public:
	using native_handle_type = ::fast_io::win32::nt::details::nt_alpc_server_handle<family> *;
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	native_handle_type handle{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit operator bool() const noexcept
	{
		return handle != nullptr;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};

} // namespace fast_io
