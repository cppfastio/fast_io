#pragma once

namespace fast_io::win32::nt
{

struct driver_object
{
std::int_least16_t     Type;
std::int_least16_t     Size;
driver_object*   DeviceObject;
std::uint_least32_t    Flags;
void*            DriverStart;
std::uint_least32_t    DriverSize;
void*            DriverSection;
void*		 DriverExtension;
utf16_string     DriverName;
utf16_string*    HardwareDatabase;
void*		 FastIoDispatch;
void*		 DriverInit;
void*		 DriverStartIo;
void*		 DriverUnload;
void*		 MajorFunction[28];
};
}
