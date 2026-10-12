#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <wlanapi.h>
#include <fast_io.h>
#include <fast_io_dsal/string_view.h>
#include <cstdint>
#pragma comment(lib, "wlanapi.lib")

class win32_wlan_file
{
public:
	explicit constexpr win32_wlan_file() noexcept = default;

	explicit win32_wlan_file(::std::uint_least32_t version)
	{
		::std::uint_least32_t negotiated_version{};
		if (WlanOpenHandle(version, nullptr, reinterpret_cast<PDWORD>(::std::addressof(negotiated_version)), reinterpret_cast<void **>(&handle)) != ERROR_SUCCESS)
		{
			::fast_io::io::panic("WlanOpenHandle failed\n");
		}
	}

	~win32_wlan_file()
	{
		if (handle)
		{
			WlanCloseHandle(reinterpret_cast<void *>(handle), nullptr);
		}
	}

	void *handle = nullptr;

	win32_wlan_file(win32_wlan_file const &) = delete;
	win32_wlan_file &operator=(win32_wlan_file const &) = delete;

	constexpr win32_wlan_file(win32_wlan_file &&other) noexcept : handle(other.handle)
	{
		other.handle = nullptr;
	}

	win32_wlan_file &operator=(win32_wlan_file &&other) noexcept
	{
		if (this != &other)
		{
			if (handle)
			{
				WlanCloseHandle(reinterpret_cast<HANDLE>(handle), nullptr);
			}
			handle = other.handle;
			other.handle = nullptr;
		}
		return *this;
	}
};

class win32_wlan_memory_file
{
public:
	explicit constexpr win32_wlan_memory_file() noexcept = default;

	~win32_wlan_memory_file()
	{
		if (hd)
		{
			WlanFreeMemory(hd);
		}
	}

	void *hd = nullptr;

	win32_wlan_memory_file(win32_wlan_memory_file const &) = delete;
	win32_wlan_memory_file &operator=(win32_wlan_memory_file const &) = delete;

	constexpr win32_wlan_memory_file(win32_wlan_memory_file &&other) noexcept : hd(other.hd)
	{
		other.hd = nullptr;
	}

	win32_wlan_memory_file &operator=(win32_wlan_memory_file &&other) noexcept
	{
		if (this != &other)
		{
			if (hd)
			{
				WlanFreeMemory(hd);
			}
			hd = other.hd;
			other.hd = nullptr;
		}
		return *this;
	}
};

inline void WINAPI wlan_notification_callback(PWLAN_NOTIFICATION_DATA data, void *ctx)
{
	if (!data || !ctx)
	{
		return;
	}
	if (data->NotificationCode == wlan_notification_acm_scan_complete)
	{
		::fast_io::io::print("[+] Scan complete\n");
	}
	else if (data->NotificationCode == wlan_notification_acm_scan_fail)
	{
		::fast_io::io::print("[-] Scan failed\n");
	}
	if (!SetEvent(ctx))
	{
		::fast_io::io::panic("SetEvent failure\n");
	}
}
inline ::fast_io::cstring_view signal_quality(::std::int_least32_t rssi_dbm) noexcept
{
	if (rssi_dbm >= -55)
	{
		return ::fast_io::cstring_view("Excellent");
	}
	if (rssi_dbm >= -65)
	{
		return ::fast_io::cstring_view("Good");
	}
	if (rssi_dbm >= -75)
	{
		return ::fast_io::cstring_view("Fair");
	}
	if (rssi_dbm >= -85)
	{
		return ::fast_io::cstring_view("Weak");
	}
	return ::fast_io::cstring_view("Very Weak");
}
inline constexpr ::std::uint_least32_t frequency_to_channel(::std::uint_least32_t freq_khz) noexcept
{
	::std::uint_least32_t freq_mhz = freq_khz / 1000u;
	if (freq_mhz >= 2412 && freq_mhz <= 2472)
	{
		return (freq_mhz - 2407) / 5; // Channels 1–13
	}
	else if (freq_mhz == 2484)
	{
		return 14;
	}
	else if (freq_mhz >= 5180 && freq_mhz <= 5825)
	{
		return (freq_mhz - 5000) / 5; // Channels 36–165
	}
	else if (freq_mhz >= 5955 && freq_mhz <= 7115)
	{
		return (freq_mhz - 5950) / 5; // Wi-Fi 6E channels
	}
	return 0; // Unknown or unsupported
}
inline constexpr ::fast_io::cstring_view frequency_to_band(::std::uint_least32_t freq_khz) noexcept
{
	std::uint_least32_t freq_mhz = freq_khz / 1000u;
	if (freq_mhz >= 2400 && freq_mhz <= 2500)
	{
		return ::fast_io::cstring_view("2.4 GHz");
	}
	else if (freq_mhz >= 5000 && freq_mhz <= 5900)
	{
		return ::fast_io::cstring_view("5 GHz");
	}
	else if (freq_mhz >= 5925 && freq_mhz <= 7125)
	{
		return ::fast_io::cstring_view("6 GHz");
	}
	return ::fast_io::cstring_view("Unknown");
}

int main()
{
	win32_wlan_file wlan(2);
	HANDLE scan_event = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (!scan_event)
	{
		::fast_io::io::panic("CreateEvent failed\n");
	}
	::fast_io::win32_file scan_event_file(scan_event);

	// Get interface
	win32_wlan_memory_file if_mem;
	WlanEnumInterfaces(wlan.handle, nullptr,
					   reinterpret_cast<WLAN_INTERFACE_INFO_LIST **>(&if_mem.hd));
	{
		auto *if_list = reinterpret_cast<WLAN_INTERFACE_INFO_LIST *>(if_mem.hd);
		GUID interface_guid = if_list->InterfaceInfo[0].InterfaceGuid;

		// Register callback
		WlanRegisterNotification(wlan.handle, WLAN_NOTIFICATION_SOURCE_ACM, TRUE,
								 wlan_notification_callback, scan_event, nullptr, nullptr);

		// Trigger scan
		WlanScan(wlan.handle, &interface_guid, nullptr, nullptr, nullptr);
	}
	// Wait for scan to complete (e.g., Sleep or message loop)
	auto start_time{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	DWORD wait_result = WaitForSingleObject(scan_event, 10000); // 10s timeout
	if (wait_result != WAIT_OBJECT_0)
	{
		::fast_io::io::perr("Scan did not complete in time\n");
	}
	::fast_io::io::print("Elapsed time for scanning: ",
						 ::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime) - start_time, "s\n");
	auto *if_list = reinterpret_cast<WLAN_INTERFACE_INFO_LIST *>(if_mem.hd);
	for (::std::uint_least32_t i{}; i != if_list->dwNumberOfItems; ++i)
	{
		GUID guid = if_list->InterfaceInfo[i].InterfaceGuid;

		win32_wlan_memory_file bss_mem;
		if (WlanGetNetworkBssList(wlan.handle, ::std::addressof(guid), nullptr,
								  dot11_BSS_type_any, FALSE, nullptr,
								  reinterpret_cast<WLAN_BSS_LIST **>(::std::addressof(bss_mem.hd))) != ERROR_SUCCESS)
		{
			::fast_io::io::perrln("WlanGetNetworkBssList failed:", i);
			continue;
		}

		auto *bss_list{reinterpret_cast<WLAN_BSS_LIST *>(bss_mem.hd)};
		for (::std::uint_least32_t j{}; j != bss_list->dwNumberOfItems; ++j)
		{
			WLAN_BSS_ENTRY const &entry = bss_list->wlanBssEntries[j];
			::fast_io::string_view ssid(reinterpret_cast<char const *>(entry.dot11Ssid.ucSSID), entry.dot11Ssid.uSSIDLength);
			if (ssid.is_empty())
			{
				ssid = ::fast_io::string_view("[Hidden]");
			}

			// Default to 20 MHz if no IE is found
			std::uint_least32_t channel_width_mhz = 20;

			// Parse IEs to determine actual channel width
			std::uint_least8_t const *ie_data = reinterpret_cast<std::uint_least8_t const *>(&entry) + entry.ulIeOffset;
			for (std::uint_least32_t i{}; i + 2 < entry.ulIeSize;)
			{
				std::uint_least8_t id = ie_data[i];
				std::uint_least8_t len = ie_data[i + 1];
				if (i + 2 + len > entry.ulIeSize)
				{
					break; // prevent overflow
				}

				std::uint_least8_t const *data = &ie_data[i + 2];

				// HT Operation IE (ID 0x3D)
				if (id == 0x3D && len >= 3)
				{
					std::uint_least8_t ht_info = data[1];
					std::uint_least8_t channel_width_flag = data[2]; // bit 0: 0 = 20 MHz, 1 = 40 MHz
					if (channel_width_flag & 0x01)
					{
						channel_width_mhz = 40;
					}
				}

				// VHT Operation IE (ID 0xC0)
				else if (id == 0xC0 && len >= 1)
				{
					std::uint_least8_t vht_channel_width = data[0]; // 0 = 20/40, 1 = 80, 2 = 160 or 80+80
					if (vht_channel_width == 1)
					{
						channel_width_mhz = 80;
					}
					else if (vht_channel_width == 2)
					{
						channel_width_mhz = 160;
					}
				}

				// You can add HE parsing here if needed

				i += 2 + len;
			}

			// Estimate channel range based on actual width
			std::uint_least32_t center_channel = frequency_to_channel(entry.ulChCenterFrequency);
			std::uint_least32_t ch_span = channel_width_mhz / 20;
			std::uint_least32_t ch_start = center_channel >= ch_span ? center_channel - ch_span : center_channel;
			std::uint_least32_t ch_end = center_channel + ch_span;

			::fast_io::io::print("SSID:", ssid, "\t"
												"Signal:",
								 entry.lRssi, " dBm (Quality:", signal_quality(entry.lRssi), ")\t"
																							 "Frequency:",
								 entry.ulChCenterFrequency, "kHz\t"
															"Band:",
								 frequency_to_band(entry.ulChCenterFrequency), "\t"
																			   "Channel:",
								 center_channel, " "
												 "(width:",
								 channel_width_mhz, " MHz, covers:", ch_start, "-", ch_end, ")\n");
		}
	}
}
