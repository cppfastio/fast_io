﻿#pragma once

#pragma comment(linker, "/alternatename:__imp_?GetLastError@win32@fast_io@@YGIXZ=__imp__GetLastError@0")
#pragma comment(linker, "/alternatename:__imp_?LoadLibraryA@win32@fast_io@@YGPAXPBD@Z=__imp__LoadLibraryA@4")
#pragma comment(linker, "/alternatename:__imp_?LoadLibraryW@win32@fast_io@@YGPAXPB_S@Z=__imp__LoadLibraryW@4")
#pragma comment(linker, "/alternatename:__imp_?LoadLibraryExA@win32@fast_io@@YGPAXPBDPAXI@Z=__imp__LoadLibraryExA@12")
#pragma comment(linker, "/alternatename:__imp_?LoadLibraryExW@win32@fast_io@@YGPAXPB_SPAXI@Z=__imp__LoadLibraryExW@12")
#pragma comment(linker, \
				"/alternatename:__imp_?FormatMessageA@win32@fast_io@@YGIIPBDIIPADIPAX@Z=__imp__FormatMessageA@28")
#pragma comment(linker, \
				"/alternatename:__imp_?FormatMessageW@win32@fast_io@@YGIIPB_SIIPA_SIPAX@Z=__imp__FormatMessageW@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?CreateFileMappingA@win32@fast_io@@YGPAXPAXPAUsecurity_attributes@12@IIIPBD@Z=__imp__CreateFileMappingA@24")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?CreateFileMappingW@win32@fast_io@@YGPAXPAXPAUsecurity_attributes@12@IIIPB_S@Z=__imp__CreateFileMappingW@24")
#pragma comment(linker, "/alternatename:__imp_?MapViewOfFile@win32@fast_io@@YGPAXPAXIIII@Z=__imp__MapViewOfFile@20")
#pragma comment(linker, "/alternatename:__imp_?SetEndOfFile@win32@fast_io@@YGHPAX@Z=__imp__SetEndOfFile@4")
#pragma comment(linker, "/alternatename:__imp_?UnmapViewOfFile@win32@fast_io@@YGHPBX@Z=__imp__UnmapViewOfFile@4")
#pragma comment(linker, \
				"/alternatename:__imp_?WriteFile@win32@fast_io@@YGHPAXPBXIPAIPAUoverlapped@12@@Z=__imp__WriteFile@20")
#pragma comment(linker, \
				"/alternatename:__imp_?ReadFile@win32@fast_io@@YGHPAXPBXIPAIPAUoverlapped@12@@Z=__imp__ReadFile@20")
#pragma comment(linker, "/alternatename:__imp_?SetFilePointer@win32@fast_io@@YGIPAXHPAHI@Z=__imp__SetFilePointer@16")
#pragma comment(linker, \
				"/alternatename:__imp_?SetFilePointerEx@win32@fast_io@@YGHPAX_JPA_JI@Z=__imp__SetFilePointerEx@20")
#pragma comment(linker, \
				"/alternatename:__imp_?DuplicateHandle@win32@fast_io@@YGHPAX00PAPAXIHI@Z=__imp__DuplicateHandle@28")
#pragma comment(linker, "/alternatename:__imp_?GetStdHandle@win32@fast_io@@YGPAXI@Z=__imp__GetStdHandle@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?CreatePipe@win32@fast_io@@YGHPAPAX0PAUsecurity_attributes@12@I@Z=__imp__CreatePipe@16")
#pragma comment(linker, "/alternatename:__imp_?FreeLibrary@win32@fast_io@@YGHPAX@Z=__imp__FreeLibrary@4")
#pragma comment(linker, "/alternatename:__imp_?GetProcAddress@win32@fast_io@@YGP6GHX_EPAXPBD@Z=__imp__GetProcAddress@8")
#pragma comment(linker, "/alternatename:__imp_?GetModuleHandleA@win32@fast_io@@YGPAXPBD@Z=__imp__GetModuleHandleA@4")
#pragma comment(linker, "/alternatename:__imp_?GetModuleHandleW@win32@fast_io@@YGPAXPB_S@Z=__imp__GetModuleHandleW@4")
#pragma comment(linker, \
				"/alternatename:__imp_?WaitForSingleObject@win32@fast_io@@YGIPAXI@Z=__imp__WaitForSingleObject@8")
#pragma comment(linker, "/alternatename:__imp_?CancelIo@win32@fast_io@@YGIPAX@Z=__imp__CancelIo@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetFileInformationByHandle@win32@fast_io@@YGHPIAXPIAUby_handle_file_information@12@@Z=__imp__GetFileInformationByHandle@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetUserDefaultLocaleName@win32@fast_io@@YGHPA_SH@Z=__imp__GetUserDefaultLocaleName@8")
#pragma comment(linker, "/alternatename:__imp_?GetUserDefaultLCID@win32@fast_io@@YGIXZ=__imp__GetUserDefaultLCID@0")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetSystemTimePreciseAsFileTime@win32@fast_io@@YGXPAUfiletime@12@@Z=__imp__GetSystemTimePreciseAsFileTime@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetSystemTimeAsFileTime@win32@fast_io@@YGXPAUfiletime@12@@Z=__imp__GetSystemTimeAsFileTime@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?QueryUnbiasedInterruptTime@win32@fast_io@@YGHPA_K@Z=__imp__QueryUnbiasedInterruptTime@4")
#pragma comment( \
	linker, "/alternatename:__imp_?QueryPerformanceCounter@win32@fast_io@@YGHPA_J@Z=__imp__QueryPerformanceCounter@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?QueryPerformanceFrequency@win32@fast_io@@YGHPA_J@Z=__imp__QueryPerformanceFrequency@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetProcessTimes@win32@fast_io@@YGHPAXPAUfiletime@12@111@Z=__imp__GetProcessTimes@20")
#pragma comment( \
	linker, "/alternatename:__imp_?GetThreadTimes@win32@fast_io@@YGHPAXPAUfiletime@12@111@Z=__imp__GetThreadTimes@20")
#pragma comment(linker, \
				"/alternatename:__imp_?GetHandleInformation@win32@fast_io@@YGHPAXPAI@Z=__imp__GetHandleInformation@8")
#pragma comment(linker, \
				"/alternatename:__imp_?SetHandleInformation@win32@fast_io@@YGHPAXII@Z=__imp__SetHandleInformation@12")
#pragma comment(linker, "/alternatename:__imp_?GetTempPathA@win32@fast_io@@YGIIPAD@Z=__imp__GetTempPathA@8")
#pragma comment(linker, "/alternatename:__imp_?GetTempPathW@win32@fast_io@@YGIIPA_S@Z=__imp__GetTempPathW@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?CreateFileA@win32@fast_io@@YGPAXPBDIIPAUsecurity_attributes@12@IIPAX@Z=__imp__CreateFileA@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?CreateFileW@win32@fast_io@@YGPAXPB_SIIPAUsecurity_attributes@12@IIPAX@Z=__imp__CreateFileW@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?CreateIoCompletionPort@win32@fast_io@@YGPAXPAX0II@Z=__imp__CreateIoCompletionPort@16")
#pragma comment(linker, "/alternatename:__imp_?SystemFunction036@win32@fast_io@@YGHPAXI@Z=__imp__SystemFunction036@8")
#pragma comment(linker, "/alternatename:__imp_?CloseHandle@win32@fast_io@@YGHPAX@Z=__imp__CloseHandle@4")
#pragma comment(linker, \
				"/alternatename:__imp_?LockFileEx@win32@fast_io@@YGHPAXIIIIPAUoverlapped@12@@Z=__imp__LockFileEx@24")
#pragma comment( \
	linker, "/alternatename:__imp_?UnlockFileEx@win32@fast_io@@YGHPAXIIIPAUoverlapped@12@@Z=__imp__UnlockFileEx@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?DeviceIoControl@win32@fast_io@@YGHPAXI0I0I0PAUoverlapped@12@@Z=__imp__DeviceIoControl@32")
#pragma comment(linker, "/alternatename:__imp_?GetFileType@win32@fast_io@@YGIPAX@Z=__imp__GetFileType@4")
#pragma comment(linker, "/alternatename:__imp_?GetACP@win32@fast_io@@YGIXZ=__imp__GetACP@0")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetEnvironmentVariableA@win32@fast_io@@YGIPBDPADI@Z=__imp__GetEnvironmentVariableA@12")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetEnvironmentVariableW@win32@fast_io@@YGIPB_SPA_SI@Z=__imp__GetEnvironmentVariableW@12")
#pragma comment(linker, "/alternatename:__imp_?MessageBoxA@win32@fast_io@@YGIPAXPBD1I@Z=__imp__MessageBoxA@16")
#pragma comment(linker, "/alternatename:__imp_?MessageBoxW@win32@fast_io@@YGIPAXPB_S1I@Z=__imp__MessageBoxW@16")
#pragma comment(linker, "/alternatename:__imp_?GetConsoleMode@win32@fast_io@@YGHPAXPAI@Z=__imp__GetConsoleMode@8")
#pragma comment(linker, "/alternatename:__imp_?SetConsoleMode@win32@fast_io@@YGHPAXI@Z=__imp__SetConsoleMode@8")
#pragma comment(linker, "/alternatename:__imp_?ReadConsoleA@win32@fast_io@@YGHPAX0IPAI0@Z=__imp__ReadConsoleA@20")
#pragma comment(linker, "/alternatename:__imp_?ReadConsoleW@win32@fast_io@@YGHPAX0IPAI0@Z=__imp__ReadConsoleW@20")
#pragma comment(linker, "/alternatename:__imp_?WriteConsoleA@win32@fast_io@@YGHPAXPBXIPAI0@Z=__imp__WriteConsoleA@20")
#pragma comment(linker, "/alternatename:__imp_?WriteConsoleW@win32@fast_io@@YGHPAXPBXIPAI0@Z=__imp__WriteConsoleW@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetConsoleScreenBufferInfo@win32@fast_io@@YGHPAXPAUconsole_screen_buffer_info@12@@Z=__imp__GetConsoleScreenBufferInfo@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ScrollConsoleScreenBufferA@win32@fast_io@@YGHPAXPBUsmall_rect@12@1Ucoord@12@PBUchar_info@12@@Z=__imp__ScrollConsoleScreenBufferA@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ScrollConsoleScreenBufferW@win32@fast_io@@YGHPAXPBUsmall_rect@12@1Ucoord@12@PBUchar_info@12@@Z=__imp__ScrollConsoleScreenBufferW@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?SetConsoleCursorPosition@win32@fast_io@@YGHPAXUcoord@12@@Z=__imp__SetConsoleCursorPosition@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?InitializeCriticalSection@win32@fast_io@@YGXPAX@Z=__imp__InitializeCriticalSection@4")
#pragma comment(linker, \
				"/alternatename:__imp_?EnterCriticalSection@win32@fast_io@@YGXPAX@Z=__imp__EnterCriticalSection@4")
#pragma comment( \
	linker, "/alternatename:__imp_?TryEnterCriticalSection@win32@fast_io@@YGHPAX@Z=__imp__TryEnterCriticalSection@4")
#pragma comment(linker, \
				"/alternatename:__imp_?LeaveCriticalSection@win32@fast_io@@YGXPAX@Z=__imp__LeaveCriticalSection@4")
#pragma comment(linker, \
				"/alternatename:__imp_?DeleteCriticalSection@win32@fast_io@@YGXPAX@Z=__imp__DeleteCriticalSection@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSADuplicateSocketA@win32@fast_io@@YGHPAXIPAUwsaprotocol_infoa@12@@Z=__imp__WSADuplicateSocketA@12")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSADuplicateSocketW@win32@fast_io@@YGXPAXIPAUwsaprotocol_infow@12@@Z=__imp__WSADuplicateSocketW@12")
#pragma comment(linker, "/alternatename:__imp_?WSACleanup@win32@fast_io@@YGHXZ=__imp__WSACleanup@0")
#pragma comment(linker, "/alternatename:__imp_?WSAStartup@win32@fast_io@@YGHIPAUwsadata@12@@Z=__imp__WSAStartup@8")
#pragma comment(linker, "/alternatename:__imp_?WSAGetLastError@win32@fast_io@@YGHXZ=__imp__WSAGetLastError@0")
#pragma comment(linker, "/alternatename:__imp_?closesocket@win32@fast_io@@YGHI@Z=__imp__closesocket@4")
#pragma comment( \
	linker, "/alternatename:__imp_?WSASocketW@win32@fast_io@@YGIHHHPAUwsaprotocol_infow@12@II@Z=__imp__WSASocketW@24")
#pragma comment( \
	linker, "/alternatename:__imp_?WSASocketA@win32@fast_io@@YGIHHHPAUwsaprotocol_infoa@12@II@Z=__imp__WSASocketA@24")
#pragma comment(linker, "/alternatename:__imp_?bind@win32@fast_io@@YGHIPBXH@Z=__imp__bind@12")
#pragma comment(linker, "/alternatename:__imp_?listen@win32@fast_io@@YGHIH@Z=__imp__listen@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSAAccept@win32@fast_io@@YGIIPBXPAHP6GXPAUwsabuf@12@2PAUqualityofservice@12@322PAII@_EI@Z=__imp__WSAAccept@20")
#pragma comment(linker, "/alternatename:__imp_?ioctlsocket@win32@fast_io@@YGHIJPAI@Z=__imp__ioctlsocket@12")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSASend@win32@fast_io@@YGHIPAUwsabuf@12@IPAIIPAUoverlapped@12@P6GXII2I@_E@Z=__imp__WSASend@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSASendMsg@win32@fast_io@@YGHIPAUwsamsg@12@IPAIPAUoverlapped@12@P6GXII2I@_E@Z=__imp__WSASendMsg@24")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSASendTo@win32@fast_io@@YGHIPAUwsabuf@12@IPAIIPBXHPAUoverlapped@12@P6GXII3I@_E@Z=__imp__WSASendTo@36")
#pragma comment(linker, "/alternatename:__imp_?recv@win32@fast_io@@YGHIPADHH@Z=__imp__recv@16")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSARecv@win32@fast_io@@YGHIPAUwsabuf@12@IPAI1PAUoverlapped@12@P6GXII2I@_E@Z=__imp__WSARecv@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSARecvFrom@win32@fast_io@@YGHIPAUwsabuf@12@IPAI1PAXPAHPAUoverlapped@12@P6GXII4I@_E@Z=__imp__WSARecvFrom@36")
#pragma comment(linker, "/alternatename:__imp_?connect@win32@fast_io@@YGHIPBXH@Z=__imp__connect@12")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?WSAConnect@win32@fast_io@@YGHIPBXHPAUwsabuf@12@1PAUqualityofservice@12@2@Z=__imp__WSAConnect@28")
#pragma comment(linker, "/alternatename:__imp_?recvfrom@win32@fast_io@@YGHIPADHHPAXPAH@Z=__imp__recvfrom@24")
#pragma comment(linker, "/alternatename:__imp_?sendto@win32@fast_io@@YGHIPBDHHPBXH@Z=__imp__sendto@24")
#pragma comment(linker, "/alternatename:__imp_?shutdown@win32@fast_io@@YGHIPBXH@Z=__imp__shutdown@8")
#pragma comment(linker, "/alternatename:__imp_?GetCurrentProcessId@win32@fast_io@@YGIXZ=__imp__GetCurrentProcessId@0")
#pragma comment(linker, "/alternatename:__imp_?FlushFileBuffers@win32@fast_io@@YGHPAX@Z=__imp__FlushFileBuffers@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetQueuedCompletionStatus@win32@fast_io@@YGHPAXPAI1PAUoverlapped@12@I@Z=__imp__GetQueuedCompletionStatus@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?freeaddrinfo@win32@fast_io@@YGXPAU?$win32_family_addrinfo@$0A@@12@@Z=__imp__freeaddrinfo@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?FreeAddrInfoW@win32@fast_io@@YGXPAU?$win32_family_addrinfo@$00@12@@Z=__imp__FreeAddrInfoW@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?getaddrinfo@win32@fast_io@@YGHPBD0PBU?$win32_family_addrinfo@$0A@@12@PAPAU312@@Z=__imp__getaddrinfo@16")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetAddrInfoW@win32@fast_io@@YGHPB_S0PBU?$win32_family_addrinfo@$00@12@PAPAU312@@Z=__imp__GetAddrInfoW@16")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?CryptAcquireContextA@win32@fast_io@@YGHPAIPB_Q1II@Z=__imp__CryptAcquireContextA@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?CryptAcquireContextW@win32@fast_io@@YGHPAIPB_S1II@Z=__imp__CryptAcquireContextW@20")
#pragma comment(linker, "/alternatename:__imp_?CryptReleaseContext@win32@fast_io@@YGHII@Z=__imp__CryptReleaseContext@8")
#pragma comment(linker, "/alternatename:__imp_?CryptGenRandom@win32@fast_io@@YGHIIPAE@Z=__imp__CryptGenRandom@12")
#pragma comment(linker, "/alternatename:__imp_?RegOpenKeyA@win32@fast_io@@YGHIPB_QPAI@Z=__imp__RegOpenKeyA@12")
#pragma comment(linker, "/alternatename:__imp_?RegOpenKeyW@win32@fast_io@@YGHIPB_SPAI@Z=__imp__RegOpenKeyW@12")
#pragma comment(linker, \
				"/alternatename:__imp_?RegQueryValueExA@win32@fast_io@@YGHIPB_QPAI1PAX1@Z=__imp__RegQueryValueExA@24")
#pragma comment(linker, \
				"/alternatename:__imp_?RegQueryValueExW@win32@fast_io@@YGHIPB_SPAI1PAX1@Z=__imp__RegQueryValueExW@24")
#pragma comment(linker, "/alternatename:__imp_?RegCloseKey@win32@fast_io@@YGHI@Z=__imp__RegCloseKey@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?GetTimeZoneInformation@win32@fast_io@@YGIPAUtime_zone_information@12@@Z=__imp__GetTimeZoneInformation@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?rtl_nt_status_to_dos_error@nt@win32@fast_io@@YGII@Z=__imp__RtlNtStatusToDosError@4")
#pragma comment(linker, "/alternatename:__imp_?NtClose@nt@win32@fast_io@@YGIPAX@Z=__imp__NtClose@4")
#pragma comment(linker, "/alternatename:__imp_?ZwClose@nt@win32@fast_io@@YGIPAX@Z=__imp__ZwClose@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtCreateFile@nt@win32@fast_io@@YGIPAPAXIPAUobject_attributes@123@PAUio_status_block@123@PA_JIIIIPAXI@Z=__imp__NtCreateFile@44")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwCreateFile@nt@win32@fast_io@@YGIPAPAXIPAUobject_attributes@123@PAUio_status_block@123@PA_JIIIIPAXI@Z=__imp__ZwCreateFile@44")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtCreateSection@nt@win32@fast_io@@YGIPIAPAXIPIAUobject_attributes@123@PA_KIIPIAX@Z=__imp__NtCreateSection@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwCreateSection@nt@win32@fast_io@@YGIPIAPAXIPIAUobject_attributes@123@PA_KIIPIAX@Z=__imp__ZwCreateSection@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtQueryInformationProcess@nt@win32@fast_io@@YGIPIAXW4process_information_class@123@PAXIPAI@Z=__imp__NtQueryInformationProcess@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwQueryInformationProcess@nt@win32@fast_io@@YGIPIAXW4process_information_class@123@PAXIPAI@Z=__imp__ZwQueryInformationProcess@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtWriteFile@nt@win32@fast_io@@YGIPAX0P6AX0PAUio_status_block@123@I@_E01PBXIPA_JPAI@Z=__imp__NtWriteFile@36")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwWriteFile@nt@win32@fast_io@@YGIPAX0P6AX0PAUio_status_block@123@I@_E01PBXIPA_JPAI@Z=__imp__ZwWriteFile@36")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtReadFile@nt@win32@fast_io@@YGIPAX0P6AX0PAUio_status_block@123@I@_E01PBXIPA_JPAI@Z=__imp__NtReadFile@36")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwReadFile@nt@win32@fast_io@@YGIPAX0P6AX0PAUio_status_block@123@I@_E01PBXIPA_JPAI@Z=__imp__ZwReadFile@36")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtQueryDirectoryFile@nt@win32@fast_io@@YGIPAX0P6AX0PAUio_status_block@123@I@_E010IW4file_information_class@123@HPAUunicode_string@123@H@Z=__imp__NtQueryDirectoryFile@44")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwQueryDirectoryFile@nt@win32@fast_io@@YGIPAX0P6AX0PAUio_status_block@123@I@_E010IW4file_information_class@123@HPAUunicode_string@123@H@Z=__imp__ZwQueryDirectoryFile@44")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtQuerySection@nt@win32@fast_io@@YGIPAXW4section_information_class@123@0IPAI@Z=__imp__NtQuerySection@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwQuerySection@nt@win32@fast_io@@YGIPAXW4section_information_class@123@0IPAI@Z=__imp__ZwQuerySection@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtQueryInformationFile@nt@win32@fast_io@@YGIPIAXPIAUio_status_block@123@0IW4file_information_class@123@@Z=__imp__NtQueryInformationFile@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwQueryInformationFile@nt@win32@fast_io@@YGIPIAXPIAUio_status_block@123@0IW4file_information_class@123@@Z=__imp__ZwQueryInformationFile@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtSetInformationFile@nt@win32@fast_io@@YGIPIAXPIAUio_status_block@123@0IW4file_information_class@123@@Z=__imp__NtSetInformationFile@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwSetInformationFile@nt@win32@fast_io@@YGIPIAXPIAUio_status_block@123@0IW4file_information_class@123@@Z=__imp__ZwSetInformationFile@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtDuplicateObject@nt@win32@fast_io@@YGIPAX00PAPAXIII@Z=__imp__NtDuplicateObject@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwDuplicateObject@nt@win32@fast_io@@YGIPAX00PAPAXIII@Z=__imp__ZwDuplicateObject@28")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtWaitForSingleObject@nt@win32@fast_io@@YGIPAXHPA_K@Z=__imp__NtWaitForSingleObject@12")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwWaitForSingleObject@nt@win32@fast_io@@YGIPAXHPA_K@Z=__imp__ZwWaitForSingleObject@12")
#pragma comment(linker, "/alternatename:__imp_?NtSetSystemTime@nt@win32@fast_io@@YGIPA_K0@Z=__imp__NtSetSystemTime@8")
#pragma comment(linker, "/alternatename:__imp_?ZwSetSystemTime@nt@win32@fast_io@@YGIPA_K0@Z=__imp__ZwSetSystemTime@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtCreateProcess@nt@win32@fast_io@@YGIPAPAXIPAUobject_attributes@123@PAXI222@Z=__imp__NtCreateProcess@32")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwCreateProcess@nt@win32@fast_io@@YGIPAPAXIPAUobject_attributes@123@PAXI222@Z=__imp__ZwCreateProcess@32")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?rtl_dos_path_name_to_nt_path_name_u@nt@win32@fast_io@@YGEPB_SPAUunicode_string@123@PAPB_SPAUrtl_relative_name_u@123@@Z=__imp__RtlDosPathNameToNtPathName_U@16")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?rtl_dos_path_name_to_nt_path_name_u_with_status@nt@win32@fast_io@@YGIPB_SPAUunicode_string@123@PAPB_SPAUrtl_relative_name_u@123@@Z=__imp__RtlDosPathNameToNtPathName_U_WithStatus@16")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?rtl_free_unicode_string@nt@win32@fast_io@@YGXPAUunicode_string@123@@Z=__imp__RtlFreeUnicodeString@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlInitializeCriticalSection@nt@win32@fast_io@@YGXPAX@Z=__imp__RtlInitializeCriticalSection@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlEnterCriticalSection@nt@win32@fast_io@@YGXPAX@Z=__imp__RtlEnterCriticalSection@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlTryEnterCriticalSection@nt@win32@fast_io@@YGHPAX@Z=__imp__RtlTryEnterCriticalSection@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlLeaveCriticalSection@nt@win32@fast_io@@YGXPAX@Z=__imp__RtlLeaveCriticalSection@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlDeleteCriticalSection@nt@win32@fast_io@@YGXPAX@Z=__imp__RtlDeleteCriticalSection@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlCreateUserThread@nt@win32@fast_io@@YGIPAX0HIII00PAPAXPAUclient_id@123@@Z=__imp__RtlCreateUserThread@40")
#pragma comment(linker, "/alternatename:__imp_?NtResumeThread@nt@win32@fast_io@@YGIPAXPAI@Z=__imp__NtResumeThread@8")
#pragma comment(linker, "/alternatename:__imp_?ZwResumeThread@nt@win32@fast_io@@YGIPAXPAI@Z=__imp__ZwResumeThread@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtLockFile@nt@win32@fast_io@@YGIPAX0P6AX0PAUio_status_block@123@I@_E01PA_J3IEE@Z=__imp__NtLockFile@40")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwLockFile@nt@win32@fast_io@@YGIPAX0P6AX0PAUio_status_block@123@I@_E01PA_J3IEE@Z=__imp__ZwLockFile@40")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtUnlockFile@nt@win32@fast_io@@YGIPAXPAUio_status_block@123@PA_J2I@Z=__imp__NtUnlockFile@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwUnlockFile@nt@win32@fast_io@@YGIPAXPAUio_status_block@123@PA_J2I@Z=__imp__ZwUnlockFile@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtFlushBuffersFile@nt@win32@fast_io@@YGIPAXPAUio_status_block@123@@Z=__imp__NtFlushBuffersFile@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwFlushBuffersFile@nt@win32@fast_io@@YGIPAXPAUio_status_block@123@@Z=__imp__ZwFlushBuffersFile@8")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtFlushBuffersFileEx@nt@win32@fast_io@@YGIPAXI0IPAUio_status_block@123@@Z=__imp__NtFlushBuffersFileEx@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwFlushBuffersFileEx@nt@win32@fast_io@@YGIPAXI0IPAUio_status_block@123@@Z=__imp__ZwFlushBuffersFileEx@20")
#pragma comment(linker, "/alternatename:__imp_?DbgPrint@nt@win32@fast_io@@YAIPBDZZ=__imp__DbgPrint")
#pragma comment(linker, "/alternatename:__imp_?DbgPrintEx@nt@win32@fast_io@@YAIIIPBDZZ=__imp__DbgPrintEx")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlCreateProcessParameters@nt@win32@fast_io@@YGIPAPAUrtl_user_process_parameters@123@PAUunicode_string@123@111PAX1111@Z=__imp__RtlCreateProcessParameters@40")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlCreateProcessParametersEx@nt@win32@fast_io@@YGIPAPAUrtl_user_process_parameters@123@PAUunicode_string@123@111PAX1111I@Z=__imp__RtlCreateProcessParametersEx@44")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlDestroyProcessParameters@nt@win32@fast_io@@YGIPAUrtl_user_process_parameters@123@@Z=__imp__RtlDestroyProcessParameters@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtCreateUserProcess@nt@win32@fast_io@@YGIPAX0IIPAUobject_attributes@123@1IIPAUrtl_user_process_parameters@123@PAUps_create_info@123@PAUps_attribute_list@123@@Z=__imp__NtCreateUserProcess@44")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwCreateUserProcess@nt@win32@fast_io@@YGIPAX0IIPAUobject_attributes@123@1IIPAUrtl_user_process_parameters@123@PAUps_create_info@123@PAUps_attribute_list@123@@Z=__imp__ZwCreateUserProcess@44")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlCreateUserProcess@nt@win32@fast_io@@YGIPAUunicode_string@123@IPAUrtl_user_process_parameters@123@PAUsecurity_descriptor@123@2PAXE33PAUrtl_user_process_information@123@@Z=__imp__RtlCreateUserProcess@40")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtMapViewOfSection@nt@win32@fast_io@@YGIPAX0PAPAXIIPBTlarge_integer@123@PAIW4section_inherit@123@II@Z=__imp__NtMapViewOfSection@40")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwMapViewOfSection@nt@win32@fast_io@@YGIPAX0PAPAXIIPBTlarge_integer@123@PAIW4section_inherit@123@II@Z=__imp__ZwMapViewOfSection@40")
#pragma comment( \
	linker, "/alternatename:__imp_?NtUnmapViewOfSection@nt@win32@fast_io@@YGIPAX0@Z=__imp__NtUnmapViewOfSection@8")
#pragma comment( \
	linker, "/alternatename:__imp_?ZwUnmapViewOfSection@nt@win32@fast_io@@YGIPAX0@Z=__imp__ZwUnmapViewOfSection@8")
#pragma comment(linker, "/alternatename:__imp_?SetConsoleCP@win32@fast_io@@YGHI@Z=__imp__SetConsoleCP@4")
#pragma comment(linker, "/alternatename:__imp_?SetConsoleOutputCP@win32@fast_io@@YGHI@Z=__imp__SetConsoleOutputCP@4")
#pragma comment(linker, "/alternatename:__imp_?GetConsoleCP@win32@fast_io@@YGIXZ=__imp__GetConsoleCP@0")
#pragma comment(linker, "/alternatename:__imp_?GetConsoleOutputCP@win32@fast_io@@YGIXZ=__imp__GetConsoleOutputCP@0")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtReadVirtualMemory@nt@win32@fast_io@@YGIPAX00IPAI@Z=__imp__NtReadVirtualMemory@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwReadVirtualMemory@nt@win32@fast_io@@YGIPAX00IPAI@Z=__imp__ZwReadVirtualMemory@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtWriteVirtualMemory@nt@win32@fast_io@@YGIPAX00IPAI@Z=__imp__NtWriteVirtualMemory@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwWriteVirtualMemory@nt@win32@fast_io@@YGIPAX00IPAI@Z=__imp__ZwWriteVirtualMemory@20")
#pragma comment(linker, "/alternatename:__imp_?RtlAcquirePebLock@nt@win32@fast_io@@YGIXZ=__imp__RtlAcquirePebLock@0")
#pragma comment(linker, "/alternatename:__imp_?RtlReleasePebLock@nt@win32@fast_io@@YGIXZ=__imp__RtlReleasePebLock@0")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtAllocateVirtualMemory@nt@win32@fast_io@@YGIPAXPAPAXIPAIII@Z=__imp__NtAllocateVirtualMemory@24")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwAllocateVirtualMemory@nt@win32@fast_io@@YGIPAXPAPAXIPAIII@Z=__imp__ZwAllocateVirtualMemory@24")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?NtQueryObject@nt@win32@fast_io@@YGIPAXW4object_information_class@123@0IPAI@Z=__imp__NtQueryObject@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ZwQueryObject@nt@win32@fast_io@@YGIPAXW4object_information_class@123@0IPAI@Z=__imp__ZwQueryObject@20")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlAcquireSRWLockExclusive@nt@win32@fast_io@@YGXPAX@Z=__imp__RtlAcquireSRWLockExclusive@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlTryAcquireSRWLockExclusive@nt@win32@fast_io@@YGIPAX@Z=__imp__RtlTryAcquireSRWLockExclusive@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?RtlReleaseSRWLockExclusive@nt@win32@fast_io@@YGXPAX@Z=__imp__RtlReleaseSRWLockExclusive@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?AcquireSRWLockExclusive@win32@fast_io@@YGXPAX@Z=__imp__AcquireSRWLockExclusive@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?TryAcquireSRWLockExclusive@win32@fast_io@@YGIPAX@Z=__imp__TryAcquireSRWLockExclusive@4")
#pragma comment( \
	linker,      \
	"/alternatename:__imp_?ReleaseSRWLockExclusive@win32@fast_io@@YGXPAX@Z=__imp__ReleaseSRWLockExclusive@4")


#if defined(_DLL)
#pragma comment(linker,"/alternatename:__imp_?msvc__RTtypeid@msvc@fast_io@@YAPAXPAX@Z=__imp____RTtypeid")
#else
#pragma comment(linker,"/alternatename:?msvc__RTtypeid@msvc@fast_io@@YAPAXPAX@Z=___RTtypeid")
#endif