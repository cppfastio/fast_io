#pragma once

#include <cstdint>
#include "../../../fast_io_core_impl/allocation/common.h"

namespace fast_io::win32::nt
{

union large_integer
{
	struct DUMMYSTRUCTNAMETYPE
	{
		::std::uint_least32_t LowPart;
		::std::int_least32_t HighPart;
	} DUMMYSTRUCTNAME;
	struct uTYPE
	{
		::std::uint_least32_t LowPart;
		::std::int_least32_t HighPart;
	} u;
	::std::int_least64_t QuadPart;
};

struct ansi_string
{
	::std::uint_least16_t Length;
	::std::uint_least16_t MaximumLength;
	char *Buffer;
};

struct unicode_string; // defined in nt_preliminary_definition.h

struct object_attributes
{
	::std::uint_least32_t Length;
	void *RootDirectory;
	unicode_string *ObjectName;
	::std::uint_least32_t Attributes;
	void *SecurityDescriptor;
	void *SecurityQualityOfService;
};

struct io_status_block
{
	union
	{
		::std::uint_least32_t Status;
		void *Pointer;
	};
	::std::size_t Information;
};

using pio_apc_routine = void (*)(void *, io_status_block *, ::std::uint_least32_t) noexcept;

struct rtlp_curdir_def
{
	::std::int_least32_t ref_count;
	void *handle;
};

struct rtl_relative_name_u
{
	unicode_string relative_name;
	void *containing_directory;
	rtlp_curdir_def cur_dir_ref;
};

/*
https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ne-wdm-_file_information_class
*/

enum class file_information_class
{
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation = 2,
	FileBothDirectoryInformation = 3,
	FileBasicInformation = 4,
	FileStandardInformation = 5,
	FileInternalInformation = 6,
	FileEaInformation = 7,
	FileAccessInformation = 8,
	FileNameInformation = 9,
	FileRenameInformation = 10,
	FileLinkInformation = 11,
	FileNamesInformation = 12,
	FileDispositionInformation = 13,
	FilePositionInformation = 14,
	FileFullEaInformation = 15,
	FileModeInformation = 16,
	FileAlignmentInformation = 17,
	FileAllInformation = 18,
	FileAllocationInformation = 19,
	FileEndOfFileInformation = 20,
	FileAlternateNameInformation = 21,
	FileStreamInformation = 22,
	FilePipeInformation = 23,
	FilePipeLocalInformation = 24,
	FilePipeRemoteInformation = 25,
	FileMailslotQueryInformation = 26,
	FileMailslotSetInformation = 27,
	FileCompressionInformation = 28,
	FileObjectIdInformation = 29,
	FileCompletionInformation = 30,
	FileMoveClusterInformation = 31,
	FileQuotaInformation = 32,
	FileReparsePointInformation = 33,
	FileNetworkOpenInformation = 34,
	FileAttributeTagInformation = 35,
	FileTrackingInformation = 36,
	FileIdBothDirectoryInformation = 37,
	FileIdFullDirectoryInformation = 38,
	FileValidDataLengthInformation = 39,
	FileShortNameInformation = 40,
	FileIoCompletionNotificationInformation = 41,
	FileIoStatusBlockRangeInformation = 42,
	FileIoPriorityHintInformation = 43,
	FileSfioReserveInformation = 44,
	FileSfioVolumeInformation = 45,
	FileHardLinkInformation = 46,
	FileProcessIdsUsingFileInformation = 47,
	FileNormalizedNameInformation = 48,
	FileNetworkPhysicalNameInformation = 49,
	FileIdGlobalTxDirectoryInformation = 50,
	FileIsRemoteDeviceInformation = 51,
	FileUnusedInformation = 52,
	FileNumaNodeInformation = 53,
	FileStandardLinkInformation = 54,
	FileRemoteProtocolInformation = 55,

	//
	//  These are special versions of these operations (defined earlier)
	//  which can be used by kernel mode drivers only to bypass security
	//  access checks for Rename and HardLink operations.  These operations
	//  are only recognized by the IOManager, a file system should never
	//  receive these.
	//

	FileRenameInformationBypassAccessCheck = 56,
	FileLinkInformationBypassAccessCheck = 57,

	//
	// End of special information classes reserved for IOManager.
	//

	FileVolumeNameInformation = 58,
	FileIdInformation = 59,
	FileIdExtdDirectoryInformation = 60,
	FileReplaceCompletionInformation = 61,
	FileHardLinkFullIdInformation = 62,
	FileIdExtdBothDirectoryInformation = 63,
	FileDispositionInformationEx = 64,
	FileRenameInformationEx = 65,
	FileRenameInformationExBypassAccessCheck = 66,
	FileDesiredStorageClassInformation = 67,
	FileStatInformation = 68,
	FileMemoryPartitionInformation = 69,
	FileStatLxInformation = 70,
	FileCaseSensitiveInformation = 71,
	FileLinkInformationEx = 72,
	FileLinkInformationExBypassAccessCheck = 73,
	FileStorageReserveIdInformation = 74,
	FileCaseSensitiveInformationForceAccessCheck = 75,
};

struct file_full_dir_information
{
	::std::uint_least32_t NextEntryOffset;
	::std::uint_least32_t FileIndex;
	::std::int_least64_t CreationTime;
	::std::int_least64_t LastAccessTime;
	::std::int_least64_t LastWriteTime;
	::std::int_least64_t ChangeTime;
	::std::int_least64_t EndOfFile;
	::std::int_least64_t AllocationSize;
	::std::uint_least32_t FileAttributes;
	::std::uint_least32_t FileNameLength;
	::std::uint_least32_t EaSize;
	char16_t FileName[1];
};

struct file_id_full_dir_information
{
	::std::uint_least32_t NextEntryOffset;
	::std::uint_least32_t FileIndex;
	::std::int_least64_t CreationTime;
	::std::int_least64_t LastAccessTime;
	::std::int_least64_t LastWriteTime;
	::std::int_least64_t ChangeTime;
	::std::int_least64_t EndOfFile;
	::std::int_least64_t AllocationSize;
	::std::uint_least32_t FileAttributes;
	::std::uint_least32_t FileNameLength;
	::std::uint_least32_t EaSize;
	::std::uint_least64_t FileId;
	char16_t FileName[1];
};

struct file_both_dir_information
{
	::std::uint_least32_t NextEntryOffset;
	::std::uint_least32_t FileIndex;
	::std::int_least64_t CreationTime;
	::std::int_least64_t LastAccessTime;
	::std::int_least64_t LastWriteTime;
	::std::int_least64_t ChangeTime;
	::std::int_least64_t EndOfFile;
	::std::int_least64_t AllocationSize;
	::std::uint_least32_t FileAttributes;
	::std::uint_least32_t FileNameLength;
	::std::uint_least32_t EaSize;
	char ShortNameLength;
	char16_t ShortName[12];
	char16_t FileName[1];
};

struct file_id_both_dir_information
{
	::std::uint_least32_t NextEntryOffset;
	::std::uint_least32_t FileIndex;
	::std::int_least64_t CreationTime;
	::std::int_least64_t LastAccessTime;
	::std::int_least64_t LastWriteTime;
	::std::int_least64_t ChangeTime;
	::std::int_least64_t EndOfFile;
	::std::int_least64_t AllocationSize;
	::std::uint_least32_t FileAttributes;
	::std::uint_least32_t FileNameLength;
	::std::uint_least32_t EaSize;
	char ShortNameLength;
	char16_t ShortName[12];
	::std::uint_least64_t FileId;
	char16_t FileName[1];
};

union dir_information
{
	void *DirInfo;
	file_full_dir_information *FullDirInfo;
	file_id_full_dir_information *IdFullDirInfo;
	file_both_dir_information *BothDirInfo;
	file_id_both_dir_information *IdBothDirInfo;
};

struct file_standard_information
{
	::std::uint_least64_t allocation_size;
	::std::uint_least64_t end_of_file;
	::std::uint_least32_t number_of_links;
	int delete_pending;
	int directory;
};

struct file_basic_information
{
	::std::uint_least64_t CreationTime;
	::std::uint_least64_t LastAccessTime;
	::std::uint_least64_t LastWriteTime;
	::std::uint_least64_t ChangeTime;
	::std::uint_least32_t FileAttributes;
};

struct file_internal_information
{
	::std::uint_least64_t IndexNumber;
};

enum class process_information_class
{
	ProcessBasicInformation,           // q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
	ProcessQuotaLimits,                // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
	ProcessIoCounters,                 // q: IO_COUNTERS
	ProcessVmCounters,                 // q: VM_COUNTERS, VM_COUNTERS_EX, VM_COUNTERS_EX2
	ProcessTimes,                      // q: KERNEL_USER_TIMES
	ProcessBasePriority,               // s: KPRIORITY
	ProcessRaisePriority,              // s: ULONG
	ProcessDebugPort,                  // q: HANDLE
	ProcessExceptionPort,              // s: PROCESS_EXCEPTION_PORT
	ProcessAccessToken,                // s: PROCESS_ACCESS_TOKEN
	ProcessLdtInformation,             // qs: PROCESS_LDT_INFORMATION // 10
	ProcessLdtSize,                    // s: PROCESS_LDT_SIZE
	ProcessDefaultHardErrorMode,       // qs: ULONG
	ProcessIoPortHandlers,             // (kernel-mode only) // PROCESS_IO_PORT_HANDLER_INFORMATION
	ProcessPooledUsageAndLimits,       // q: POOLED_USAGE_AND_LIMITS
	ProcessWorkingSetWatch,            // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
	ProcessUserModeIOPL,               // qs: ULONG (requires SeTcbPrivilege)
	ProcessEnableAlignmentFaultFixup,  // s: BOOLEAN
	ProcessPriorityClass,              // qs: PROCESS_PRIORITY_CLASS
	ProcessWx86Information,            // qs: ULONG (requires SeTcbPrivilege) (VdmAllowed)
	ProcessHandleCount,                // q: ULONG, PROCESS_HANDLE_INFORMATION // 20
	ProcessAffinityMask,               // s: KAFFINITY
	ProcessPriorityBoost,              // qs: ULONG
	ProcessDeviceMap,                  // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
	ProcessSessionInformation,         // q: PROCESS_SESSION_INFORMATION
	ProcessForegroundInformation,      // s: PROCESS_FOREGROUND_BACKGROUND
	ProcessWow64Information,           // q: ULONG_PTR
	ProcessImageFileName,              // q: UNICODE_STRING
	ProcessLUIDDeviceMapsEnabled,      // q: ULONG
	ProcessBreakOnTermination,         // qs: ULONG
	ProcessDebugObjectHandle,          // q: HANDLE // 30
	ProcessDebugFlags,                 // qs: ULONG
	ProcessHandleTracing,              // q: PROCESS_HANDLE_TRACING_QUERY; s: size 0 disables, otherwise enables
	ProcessIoPriority,                 // qs: IO_PRIORITY_HINT
	ProcessExecuteFlags,               // qs: ULONG
	ProcessResourceManagement,         // ProcessTlsInformation // PROCESS_TLS_INFORMATION
	ProcessCookie,                     // q: ULONG
	ProcessImageInformation,           // q: SECTION_IMAGE_INFORMATION
	ProcessCycleTime,                  // q: PROCESS_CYCLE_TIME_INFORMATION // since VISTA
	ProcessPagePriority,               // q: PAGE_PRIORITY_INFORMATION
	ProcessInstrumentationCallback,    // qs: PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION // 40
	ProcessThreadStackAllocation,      // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
	ProcessWorkingSetWatchEx,          // q: PROCESS_WS_WATCH_INFORMATION_EX[]
	ProcessImageFileNameWin32,         // q: UNICODE_STRING
	ProcessImageFileMapping,           // q: HANDLE (input)
	ProcessAffinityUpdateMode,         // qs: PROCESS_AFFINITY_UPDATE_MODE
	ProcessMemoryAllocationMode,       // qs: PROCESS_MEMORY_ALLOCATION_MODE
	ProcessGroupInformation,           // q: USHORT[]
	ProcessTokenVirtualizationEnabled, // s: ULONG
	ProcessConsoleHostProcess,         // q: ULONG_PTR // ProcessOwnerInformation
	ProcessWindowInformation,          // q: PROCESS_WINDOW_INFORMATION // 50
	ProcessHandleInformation,          // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
	ProcessMitigationPolicy,           // s: PROCESS_MITIGATION_POLICY_INFORMATION
	ProcessDynamicFunctionTableInformation,
	ProcessHandleCheckingMode, // qs: ULONG; s: 0 disables, otherwise enables
	ProcessKeepAliveCount,     // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
	ProcessRevokeFileHandles,  // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
	ProcessWorkingSetControl,  // s: PROCESS_WORKING_SET_CONTROL
	ProcessHandleTable,        // q: ULONG[] // since WINBLUE
	ProcessCheckStackExtentsMode,
	ProcessCommandLineInformation,   // q: UNICODE_STRING // 60
	ProcessProtectionInformation,    // q: PS_PROTECTION
	ProcessMemoryExhaustion,         // PROCESS_MEMORY_EXHAUSTION_INFO // since THRESHOLD
	ProcessFaultInformation,         // PROCESS_FAULT_INFORMATION
	ProcessTelemetryIdInformation,   // PROCESS_TELEMETRY_ID_INFORMATION
	ProcessCommitReleaseInformation, // PROCESS_COMMIT_RELEASE_INFORMATION
	ProcessDefaultCpuSetsInformation,
	ProcessAllowedCpuSetsInformation,
	ProcessSubsystemProcess,
	ProcessJobMemoryInformation,                 // PROCESS_JOB_MEMORY_INFO
	ProcessInPrivate,                            // since THRESHOLD2 // 70
	ProcessRaiseUMExceptionOnInvalidHandleClose, // qs: ULONG; s: 0 disables, otherwise enables
	ProcessIumChallengeResponse,
	ProcessChildProcessInformation, // PROCESS_CHILD_PROCESS_INFORMATION
	ProcessHighGraphicsPriorityInformation,
	ProcessSubsystemInformation,   // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
	ProcessEnergyValues,           // PROCESS_ENERGY_VALUES, PROCESS_EXTENDED_ENERGY_VALUES
	ProcessActivityThrottleState,  // PROCESS_ACTIVITY_THROTTLE_STATE
	ProcessActivityThrottlePolicy, // PROCESS_ACTIVITY_THROTTLE_POLICY
	ProcessWin32kSyscallFilterInformation,
	ProcessDisableSystemAllowedCpuSets,    // 80
	ProcessWakeInformation,                // PROCESS_WAKE_INFORMATION
	ProcessEnergyTrackingState,            // PROCESS_ENERGY_TRACKING_STATE
	ProcessManageWritesToExecutableMemory, // MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
	ProcessCaptureTrustletLiveDump,
	ProcessTelemetryCoverage,
	ProcessEnclaveInformation,
	ProcessEnableReadWriteVmLogging,          // PROCESS_READWRITEVM_LOGGING_INFORMATION
	ProcessUptimeInformation,                 // PROCESS_UPTIME_INFORMATION
	ProcessImageSection,                      // q: HANDLE
	ProcessDebugAuthInformation,              // since REDSTONE4 // 90
	ProcessSystemResourceManagement,          // PROCESS_SYSTEM_RESOURCE_MANAGEMENT
	ProcessSequenceNumber,                    // q: ULONGLONG
	ProcessLoaderDetour,                      // since REDSTONE5
	ProcessSecurityDomainInformation,         // PROCESS_SECURITY_DOMAIN_INFORMATION
	ProcessCombineSecurityDomainsInformation, // PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION
	ProcessEnableLogging,                     // PROCESS_LOGGING_INFORMATION
	ProcessLeapSecondInformation,             // PROCESS_LEAP_SECOND_INFORMATION
	ProcessFiberShadowStackAllocation,        // PROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION // since 19H1
	ProcessFreeFiberShadowStackAllocation,    // PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION
	ProcessAltSystemCallInformation,          // qs: BOOLEAN (kernel-mode only) // since 20H1 // 100
	ProcessDynamicEHContinuationTargets,      // PROCESS_DYNAMIC_EH_CONTINUATION_TARGETS_INFORMATION
	MaxProcessInfoClass
};

struct process_basic_information
{
	::std::uint_least32_t ExitStatus;
	void *PebBaseAddress;
	::std::size_t AffinityMask;
	::std::uint_least32_t BasePriority;
	void *UniqueProcessId;
	void *InheritedFromUniqueProcessId;
};

struct curdir
{
	unicode_string DosPath;
	void *Handle;
};

struct rtl_drive_letter_curdir
{
	::std::uint_least16_t Flags;
	::std::uint_least16_t Length;
	::std::uint_least32_t TimeStamp;
	unicode_string DosPath;
};

inline constexpr ::std::size_t rtl_max_drive_letters{32};

struct client_id; // defined in nt_preliminary_definition.h

enum class section_information_class
{
	SectionBasicInformation,
	SectionImageInformation,
	SectionRelocationInformation,
	MaxSectionInfoClass
};

struct rtl_critical_section
{
	void *debug_info;
	::std::int_least32_t lock_count;
	::std::int_least32_t recursion_count;
	void *owning_thread;
	void *lock_semaphore;
	::std::size_t spin_count;
};

namespace details
{
struct Detailed_SubSystemVersion_t
{
	::std::uint_least16_t SubSystemMinorVersion;
	::std::uint_least16_t SubSystemMajorVersion;
};

struct Detailed_OperatingSystemVersion_t
{
	::std::uint_least16_t MajorOperatingSystemVersion;
	::std::uint_least16_t MinorOperatingSystemVersion;
};

struct Detailed_ImageFlags_t
{
	::std::uint_least8_t ComPlusNativeReady : 1;
	::std::uint_least8_t ComPlusILOnly : 1;
	::std::uint_least8_t ImageDynamicallyRelocated : 1;
	::std::uint_least8_t ImageMappedFlat : 1;
	::std::uint_least8_t BaseBelow4gb : 1;
	::std::uint_least8_t ComPlusPrefer32bit : 1;
	::std::uint_least8_t Reserved : 2;
};
} // namespace details

struct section_image_information
{
	void *TransferAddress;
	::std::uint_least32_t ZeroBits;
	::std::size_t MaximumStackSize;
	::std::size_t CommittedStackSize;
	::std::uint_least32_t SubSystemType;
	union
	{
		details::Detailed_SubSystemVersion_t Detailed_SubSystemVersion;
		::std::uint_least32_t SubSystemVersion;
	};
	union
	{

		details::Detailed_OperatingSystemVersion_t Detailed_OperatingSystemVersion;
		::std::uint_least32_t OperatingSystemVersion;
	};
	::std::uint_least16_t ImageCharacteristics;
	::std::uint_least16_t DllCharacteristics;
	::std::uint_least16_t Machine;
	::std::uint_least8_t ImageContainsCode;
	union
	{
		::std::uint_least8_t ImageFlags;
		details::Detailed_ImageFlags_t Detailed_ImageFlags;
	};
	::std::uint_least32_t LoaderFlags;
	::std::uint_least32_t ImageFileSize;
	::std::uint_least32_t CheckSum;
};

struct rtl_user_process_parameters
{
	::std::uint_least32_t MaximumLength;
	::std::uint_least32_t Length;

	::std::uint_least32_t Flags;
	::std::uint_least32_t DebugFlags;

	void *ConsoleHandle;
	::std::uint_least32_t ConsoleFlags;
	void *StandardInput;
	void *StandardOutput;
	void *StandardError;

	curdir CurrentDirectory;
	unicode_string DllPath;
	unicode_string ImagePathName;
	unicode_string CommandLine;
	char16_t *Environment;

	::std::uint_least32_t StartingX;
	::std::uint_least32_t StartingY;
	::std::uint_least32_t CountX;
	::std::uint_least32_t CountY;
	::std::uint_least32_t CountCharsX;
	::std::uint_least32_t CountCharsY;
	::std::uint_least32_t FillAttribute;

	::std::uint_least32_t WindowFlags;
	::std::uint_least32_t ShowWindowFlags;
	unicode_string WindowTitle;
	unicode_string DesktopInfo;
	unicode_string ShellInfo;
	unicode_string RuntimeData;
	rtl_drive_letter_curdir CurrentDirectories[rtl_max_drive_letters];

	::std::uint_least32_t EnvironmentSize;
	::std::uint_least32_t EnvironmentVersion;
	void *PackageDependencyData;
	::std::uint_least32_t ProcessGroupId;
	::std::uint_least32_t LoaderThreads;
};
enum class ps_create_state
{
	PsCreateInitialState,
	PsCreateFailOnFileOpen,
	PsCreateFailOnSectionCreate,
	PsCreateFailExeFormat,
	PsCreateFailMachineMismatch,
	PsCreateFailExeName, // Debugger specified
	PsCreateSuccess,
	PsCreateMaximumStates
};

struct ps_create_info
{
	::std::size_t Size;
	ps_create_state State;
	union
	{
		// PsCreateInitialState
		struct
		{
			union u1union
			{
				::std::uint_least32_t InitFlags;
				struct
				{
					::std::uint_least8_t WriteOutputOnExit : 1;
					::std::uint_least8_t DetectManifest : 1;
					::std::uint_least8_t IFEOSkipDebugger : 1;
					::std::uint_least8_t IFEODoNotPropagateKeyState : 1;
					::std::uint_least8_t SpareBits1 : 4;
					::std::uint_least8_t SpareBits2 : 8;
					::std::uint_least16_t ProhibitedImageCharacteristics : 16;
				} s1;
			} u1;
			::std::uint_least32_t AdditionalFileAccess;
		} InitState;

		// PsCreateFailOnSectionCreate
		struct
		{
			void *FileHandle;
		} FailSection;

		// PsCreateFailExeFormat
		struct
		{
			::std::uint_least16_t DllCharacteristics;
		} ExeFormat;

		// PsCreateFailExeName
		struct
		{
			void *IFEOKey;
		} ExeName;

		// PsCreateSuccess
		struct
		{
			union u2union
			{
				::std::uint_least32_t OutputFlags;
				struct
				{
					::std::uint_least8_t ProtectedProcess : 1;
					::std::uint_least8_t AddressSpaceOverride : 1;
					::std::uint_least8_t DevOverrideEnabled : 1; // From Image File Execution Options
					::std::uint_least8_t ManifestDetected : 1;
					::std::uint_least8_t ProtectedProcessLight : 1;
					::std::uint_least8_t SpareBits1 : 3;
					::std::uint_least8_t SpareBits2 : 8;
					::std::uint_least16_t SpareBits3 : 16;
				} s2;
			} u2;
			void *FileHandle;
			void *SectionHandle;
			::std::uint_least64_t UserProcessParametersNative;
			::std::uint_least32_t UserProcessParametersWow64;
			::std::uint_least32_t CurrentParameterFlags;
			::std::uint_least64_t PebAddressNative;
			::std::uint_least32_t PebAddressWow64;
			::std::uint_least64_t ManifestAddress;
			::std::uint_least32_t ManifestSize;
		} SuccessState;
	} stateunion;
};

struct ps_attribute
{
	::std::size_t Attribute; // PROC_THREAD_ATTRIBUTE_XXX | PROC_THREAD_ATTRIBUTE_XXX modifiers, see
							 // ProcThreadAttributeValue macro and Windows Internals 6 (372)
	::std::size_t Size;      // Size of Value or *ValuePtr
	union
	{
		::std::size_t Value; // Reserve 8 bytes for data (such as a Handle or a data pointer)
		void *ValuePtr;      // data pointer
	};
	::std::size_t *ReturnLength; // Either 0 or specifies size of data returned to caller via "ValuePtr"
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	ps_attribute_list
{
	::std::size_t TotalLength;   // sizeof(PS_ATTRIBUTE_LIST)
	ps_attribute Attributes[32]; // Depends on how many attribute entries should be supplied to NtCreateUserProcess
};

template <::std::size_t n>
	requires(n != 0)
struct ps_attribute_list_array
{
	::std::size_t TotalLength{sizeof(ps_attribute_list_array<n>)}; // sizeof(PS_ATTRIBUTE_LIST)
	ps_attribute Attributes[n]{};                                  // Depends on how many attribute entries should be supplied to NtCreateUserProcess
	ps_attribute_list const *list_ptr() const noexcept
	{
		return reinterpret_cast<ps_attribute_list const *>(this);
	}
	ps_attribute_list *list_ptr() noexcept
	{
		return reinterpret_cast<ps_attribute_list *>(this);
	}
};

struct rtl_user_process_information
{
	::std::uint_least32_t Length;
	void *Process;
	void *Thread;
	client_id ClientId;
	section_image_information ImageInformation;
};

struct acl
{
	char unsigned AclRevision;
	char unsigned Sbz1;
	::std::uint_least16_t AclSize;
	::std::uint_least16_t AceCount;
	::std::uint_least16_t Sbz2;
};

struct security_descriptor
{
	char unsigned Revision;
	char unsigned Sbz1;
	::std::uint_least16_t Control;
	void *Owner;
	void *Group;
	acl *Sacl;
	acl *Dacl;
};

enum class object_information_class
{
	ObjectBasicInformation,         // q: OBJECT_BASIC_INFORMATION
	ObjectNameInformation,          // q: OBJECT_NAME_INFORMATION
	ObjectTypeInformation,          // q: OBJECT_TYPE_INFORMATION
	ObjectTypesInformation,         // q: OBJECT_TYPES_INFORMATION
	ObjectHandleFlagInformation,    // qs: OBJECT_HANDLE_FLAG_INFORMATION
	ObjectSessionInformation,       // s: void // change object session // (requires SeTcbPrivilege)
	ObjectSessionObjectInformation, // s: void // change object session // (requires SeTcbPrivilege)
	MaxObjectInfoClass
};

enum class section_inherit
{
	ViewShare = 1,
	ViewUnmap = 2
};

enum priority_class : ::std::uint_least8_t
{
	Undefined,
	Idle,
	Normal,
	High,
	Realtime,
	BelowNormal,
	AboveNormal
};

struct process_priority_class
{
	bool Foreground;
	priority_class PriorityClass;
};

struct object_name_information
{
	unicode_string Name;
};

struct rtl_buffer
{
	::std::uint_least8_t *Buffer;
	::std::uint_least8_t *StaticBuffer;
	::std::size_t Size;
	::std::size_t StaticSize;
	::std::size_t ReservedForAllocatedSize;
	void *ReservedForIMalloc;
};

struct rtl_unicode_string_buffer
{
	unicode_string String;
	rtl_buffer ByteBuffer;
	char16_t MinimumStaticBufferForTerminalNul;
};

enum class system_information_class
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemNextEventIdInformation,
	SystemEventIdsInformation,
	SystemCrashDumpInformation,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemPlugPlayBusInformation,
	SystemDockInformation,
	SystemPowerInformation,
	SystemProcessorSpeedInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation
};

struct system_basic_information
{
	::std::uint_least32_t Reserved;
	::std::uint_least32_t TimerResolution;
	::std::uint_least32_t PageSize;
	::std::uint_least32_t NumberOfPhysicalPages;
	::std::uint_least32_t LowestPhysicalPageNumber;
	::std::uint_least32_t HighestPhysicalPageNumber;
	::std::uint_least32_t AllocationGranularity;
	::std::size_t MinimumUserModeAddress;
	::std::size_t MaximumUserModeAddress;
	::std::size_t ActiveProcessorsAffinityMask;
	char NumberOfProcessors;
};

struct rtl_srwlock
{
	void *Ptr;
};

namespace details
{
struct Detailed_flag_t
{
	::std::uint_least32_t StdHandleState : 2;   // PS_STD_HANDLE_STATE
	::std::uint_least32_t PseudoHandleMask : 3; // PS_STD_*
};
} // namespace details

struct ps_std_handle_info
{
	union
	{
		::std::uint_least32_t Flags; // 0x121 = 100100001
		details::Detailed_flag_t detailed_flags;
	};
	::std::uint_least32_t StdHandleSubsystemType;
};

enum class fs_information_class
{
	FileFsVolumeInformation = 1,
	FileFsLabelInformation,
	FileFsSizeInformation,
	FileFsDeviceInformation,
	FileFsAttributeInformation,
	FileFsControlInformation,
	FileFsFullSizeInformation,
	FileFsObjectIdInformation,
	FileFsDriverPathInformation,
	FileFsVolumeFlagsInformation,
	FileFsSectorSizeInformation,
	FileFsDataCopyInformation,
	FileFsMetadataSizeInformation,
	FileFsFullSizeInformationEx,
	FileFsMaximumInformation
};

struct file_fs_device_type
{
	::std::uint_least32_t DeviceType;
	::std::uint_least32_t Characteristics;
};

struct file_fs_volume_information
{
	::std::uint_least64_t VolumeCreationTime;
	::std::uint_least32_t VolumeSerialNumber;
	::std::uint_least32_t VolumeLabelLength;
	::std::uint_least8_t SupportsObjects;
	char16_t VolumeLabel[1];
};

struct file_link_information
{
	::std::uint_least8_t ReplaceIfExists;
	void *RootDirectory;
	::std::uint_least32_t FileNameLength;
	char16_t FileName[1];
};

struct file_rename_information
{
	::std::uint_least8_t ReplaceIfExists;
	void *RootDirectory;
	::std::uint_least32_t FileNameLength;
	char16_t FileName[1];
};

struct reparse_data_buffer
{
	::std::uint_least32_t ReparseTag;
	::std::uint_least16_t ReparseDataLength;
	::std::uint_least16_t Reserved;
	union BuffersU
	{
		struct SymbolicLinkReparseBufferT
		{
			::std::uint_least16_t SubstituteNameOffset;
			::std::uint_least16_t SubstituteNameLength;
			::std::uint_least16_t PrintNameOffset;
			::std::uint_least16_t PrintNameLength;
			::std::uint_least32_t Flags;
			char16_t PathBuffer[1];
		} SymbolicLinkReparseBuffer;
		struct MountPointReparseBufferT
		{
			::std::uint_least16_t SubstituteNameOffset;
			::std::uint_least16_t SubstituteNameLength;
			::std::uint_least16_t PrintNameOffset;
			::std::uint_least16_t PrintNameLength;
			char16_t PathBuffer[1];
		} MountPointReparseBuffer;
		struct GenericReparseBufferT
		{
			::std::uint_least8_t DataBuffer[1];
		} GenericReparseBuffer;
	} u;
};

struct file_disposition_information
{
	::std::uint_least8_t DeleteFile;
};

struct kernel_user_times
{
	::std::int_least64_t CreateTime;
	::std::int_least64_t ExitTime;
	::std::int_least64_t KernelTime;
	::std::int_least64_t UserTime;
};

enum class thread_information_class : ::std::uint_least32_t
{
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger,
	ThreadBreakOnTermination,
	ThreadSwitchLegacyState,
	ThreadIsTerminated,
	ThreadLastSystemCall,
	ThreadIoPriority,
	ThreadCycleTime,
	ThreadPagePriority,
	ThreadActualBasePriority,
	ThreadTebInformation,
	ThreadCSwitchMon,
	MaxThreadInfoClass
};

struct object_handle_attribute_information
{
	::std::uint_least8_t Inherit;
	::std::uint_least8_t ProtectFromClose;
};

enum class rtl_path_type
{
	RtlPathTypeUnknown,
	RtlPathTypeUncAbsolute,
	RtlPathTypeDriveAbsolute,
	RtlPathTypeDriveRelative,
	RtlPathTypeRooted,
	RtlPathTypeRelative,
	RtlPathTypeLocalDevice,
	RtlPathTypeRootLocalDevice,
};

// ALPC nt 6x
// Based on https://github.com/googleprojectzero/sandbox-attacksurface-analysis-tools/blob/main/NtApiDotNet/NtAlpcNative.cs#L82-L97
enum class ALPC_MESSAGE_TYPE
{
	None = 0,
	Request = 1,
	Reply = 2,
	Datagram = 3,
	LostReply = 4,
	PortClosed = 5,
	ClientDied = 6,
	Exception = 7,
	DebugEvent = 8,
	ErrorEvent = 9,
	ConnectionRequest = 10,
	ConnectionReply = 11,
	UNKNOWN12 = 12,       // Seems to be something like an empty reply, PID and TID pointing to own thread
	PortDisconnected = 13 // Used by the kernel when disconnecting an exception port.
};
// End Based  ON

enum class security_impersonation_level
{
	SecurityAnonymous,
	SecurityIdentification,
	SecurityImpersonation,
	SecurityDelegation
};

struct security_quality_of_service
{
	::std::uint_least32_t Length;
	security_impersonation_level ImpersonationLevel;
	::std::uint_least8_t ContextTrackingMode;
	::std::uint_least8_t EffectiveOnly;
};

struct alpc_port_attributes
{
	::std::uint_least32_t Flags;
	security_quality_of_service SecurityQos;
	::std::uint_least64_t MaxMessageLength;
	::std::uint_least64_t MemoryBandwidth;
	::std::uint_least64_t MaxPoolUsage;
	::std::uint_least64_t MaxSectionSize;
	::std::uint_least64_t MaxViewSize;
	::std::uint_least64_t MaxTotalSectionSize;
	::std::uint_least32_t DupObjectTypes;
#ifdef _WIN64
	::std::uint_least32_t Reserved;
#endif
};

struct alpc_message_attributes
{
	::std::uint_least32_t AllocatedAttributes;
	::std::uint_least32_t ValidAttributes;
};

namespace details
{
struct Details_length
{
	::std::uint_least16_t DataLength;
	::std::uint_least16_t TotalLength;
};

struct Details_ZeroInit
{
	::std::uint_least16_t Type;
	::std::uint_least16_t DataInfoOffset;
};
} // namespace details

struct port_message
{
	union
	{
		details::Details_length s1;
		::std::uint_least32_t Length;
	} u1;
	union
	{
		details::Details_ZeroInit s2;
		::std::uint_least32_t ZeroInit;
	} u2;
	union
	{
		client_id ClientId;
		double DoNotUseThisField;
	};
	::std::uint_least32_t MessageId;
	union
	{
		::std::size_t ClientViewSize;     // only valid for LPC_CONNECTION_REQUEST messages
		::std::uint_least32_t CallbackId; // only valid for LPC_REQUEST messages
	};
};


// FROM https://docs.rs/ntapi/0.3.6/ntapi/ntlpcapi/struct.ALPC_HANDLE_ATTR.html
struct alpc_data_view_attr
{
	::std::uint_least32_t Flags;
	void *SectionHandle;
	void *ViewBase;
	::std::size_t ViewSize;
};

struct alpc_token_attr
{
	::std::uint_least64_t TokenId;
	::std::uint_least64_t AuthenticationId;
	::std::uint_least64_t ModifiedId;
};

struct alpc_port_context
{
	void *Handle;
};

struct alpc_context_attr
{
	void *PortContext;
	void *MessageContext;
	::std::uint_least32_t Sequence;
	::std::uint_least32_t MessageId;
	::std::uint_least32_t CallbackId;
};

struct alpc_security_attr
{
	::std::uint_least32_t Flags;
	security_quality_of_service *pQOS;
	void *ContextHandle;
};

struct token_source
{
	char SourceName[8];
	::std::int_least64_t SourceIdentifier;
};

struct token_control
{
	::std::int_least64_t TokenId;
	::std::int_least64_t AuthenticationId;
	::std::int_least64_t ModifiedId;
	token_source TokenSource;
};

struct security_client_context
{
	security_quality_of_service SecurityQos; //_KALPC_SECURITY_DATA
	void *ClientToken;
	::std::uint_least8_t DirectlyAccessClientToken;
	::std::uint_least8_t DirectAccessEffectiveOnly;
	::std::uint_least8_t ServerIsRemote;
	token_control ClientTokenControl;
};

// From https://github.com/hakril/PythonForWindows/blob/3149961634cda0029a288e6ffa7779e4492adb13/ctypes_generation/definitions/structures/alpc.txt
struct alpc_direct_attr
{
	void *Event;
};

struct alpc_work_on_behalf_attr
{
	::std::uint_least64_t Ticket;
};

struct alpc_handle_attr
{
	::std::uint_least32_t Flags;
	void *Handle;
	::std::uint_least32_t ObjectType;
	::std::uint_least32_t DesiredAccess;
};

struct alpc_handle_entry
{
	void *Object;
};

struct port_view
{
	::std::uint_least32_t Length;
	void *SectionHandle;
	::std::uint_least32_t SectionOffset;
	::std::size_t ViewSize;
	void *ViewBase;
	void *ViewRemoteBase;
};

enum class alpc_port_information_class
{
	AlpcBasicInformation,                                // q: out ALPC_BASIC_INFORMATION
	AlpcPortInformation,                                 // s: in ALPC_PORT_ATTRIBUTES
	AlpcAssociateCompletionPortInformation,              // s: in ALPC_PORT_ASSOCIATE_COMPLETION_PORT
	AlpcConnectedSIDInformation,                         // q: in SID
	AlpcServerInformation,                               // q: inout ALPC_SERVER_INFORMATION
	AlpcMessageZoneInformation,                          // s: in ALPC_PORT_MESSAGE_ZONE_INFORMATION
	AlpcRegisterCompletionListInformation,               // s: in ALPC_PORT_COMPLETION_LIST_INFORMATION
	AlpcUnregisterCompletionListInformation,             // s: VOID
	AlpcAdjustCompletionListConcurrencyCountInformation, // s: in ULONG
	AlpcRegisterCallbackInformation,                     // kernel-mode only
	AlpcCompletionListRundownInformation,                // s: VOID
	AlpcWaitForPortReferences
};

namespace details
{
struct Details_in
{
	void *ThreadHandle;
};

struct Details_out
{
	::std::uint_least8_t ThreadBlocked;
	void *ConnectedProcessId;
	unicode_string ConnectionPortName;
};

} // namespace details

struct alpc_server_information
{
	union
	{
		details::Details_in In;
		details::Details_out Out;
	};
};

struct alpc_basic_information
{
	::std::uint_least32_t Flags;
	::std::uint_least32_t SequenceNo;
	void *PortContext;
};

enum class alpc_message_information_class
{
	AlpcMessageSidInformation,             // out PSID
	AlpcMessageTokenModifiedIdInformation, // q: out LUID
	AlpcMessageDirectStatusInformation,
	AlpcMessageHandleInformation, // ALPC_MESSAGE_HANDLE_INFORMATION
	MaxAlpcMessageInfoClass
};

struct alpc_message_handle_information
{
	::std::uint_least32_t Index;
	::std::uint_least32_t Flags;
	::std::uint_least32_t Handle;
	::std::uint_least32_t ObjectType;
	::std::uint_least32_t GrantedAccess;
};

struct cs_port_context
{
	::std::uint_least32_t PID;
	::std::uint_least32_t TID;
	::std::uint_least32_t ID;
};


struct alpc_message
{
	port_message PortHeader;
	::std::uint_least8_t PortMessage[1];
};

namespace details
{
struct Details_ex_push_lock
{
	::std::uint_least32_t Locked : 1;         // 0x0
	::std::uint_least32_t Waiting : 1;        // 0x0
	::std::uint_least32_t Waking : 1;         // 0x0
	::std::uint_least32_t MultipleShared : 1; // 0x0
	::std::uint_least32_t Shared : 28;        // 0x0
};
} // namespace details

struct ex_push_lock
{
	union
	{
		details::Details_ex_push_lock detail_Value;
		::std::uint_least32_t Value; // 0x0
		void *Ptr;                   // 0x0
	};
};

struct alpc_handle_table
{
	alpc_handle_entry Handles;
	::std::uint_least32_t TotalHandles;
	::std::uint_least32_t Flags;
	ex_push_lock Lock;
};


// typedef struct _EPROCESS* PEPROCESS;
struct kalpc_security_data
{
	alpc_handle_table *HandleTable; //_KALPC_SECURITY_DATA
	void *ContextHandle;
	void *OwningProcess;
	void *OwnerPort;
	security_client_context DynamicSecurity;
};

struct mounter_target_name
{
	::std::uint_least16_t DeviceNameLength;
	char16_t DeviceName[1];
};

struct mountmgr_volume_paths
{
	::std::uint_least32_t MultiSzLength;
	char16_t MultiSz[1];
};

/**
 * @brief minimum and maximum privilege number in `enum class privileges`.
 *  	  this is one of the macro api in WDK wdm.h
 */
inline constexpr ::std::uint_least32_t SE_MIN_WELL_KNOWN_PRIVILEGE{2};
inline constexpr ::std::uint_least32_t SE_MAX_WELL_KNOWN_PRIVILEGE{36};

/**
 * @brief WDK macro apis for privilege (supplied by wdm.h).
 *        equals to luid part values that LookupPrivilegeValue returned
 */
enum class privileges : ::std::uint_least64_t
{
	SE_CREATE_TOKEN_PRIVILEGE = 2,
	SE_ASSIGNPRIMARYTOKEN_PRIVILEGE = 3,
	SE_LOCK_MEMORY_PRIVILEGE = 4,
	SE_INCREASE_QUOTA_PRIVILEGE = 5,
	SE_MACHINE_ACCOUNT_PRIVILEGE = 6,
	SE_TCB_PRIVILEGE = 7,
	SE_SECURITY_PRIVILEGE = 8,
	SE_TAKE_OWNERSHIP_PRIVILEGE = 9,
	SE_LOAD_DRIVER_PRIVILEGE = 10,
	SE_SYSTEM_PROFILE_PRIVILEGE = 11,
	SE_SYSTEMTIME_PRIVILEGE = 12,
	SE_PROF_SINGLE_PROCESS_PRIVILEGE = 13,
	SE_INC_BASE_PRIORITY_PRIVILEGE = 14,
	SE_CREATE_PAGEFILE_PRIVILEGE = 15,
	SE_CREATE_PERMANENT_PRIVILEGE = 16,
	SE_BACKUP_PRIVILEGE = 17,
	SE_RESTORE_PRIVILEGE = 18,
	SE_SHUTDOWN_PRIVILEGE = 19,
	SE_DEBUG_PRIVILEGE = 20,
	SE_AUDIT_PRIVILEGE = 21,
	SE_SYSTEM_ENVIRONMENT_PRIVILEGE = 22,
	SE_CHANGE_NOTIFY_PRIVILEGE = 23,
	SE_REMOTE_SHUTDOWN_PRIVILEGE = 24,
	SE_UNDOCK_PRIVILEGE = 25,
	SE_SYNC_AGENT_PRIVILEGE = 26,
	SE_ENABLE_DELEGATION_PRIVILEGE = 27,
	SE_MANAGE_VOLUME_PRIVILEGE = 28,
	SE_IMPERSONATE_PRIVILEGE = 29,
	SE_CREATE_GLOBAL_PRIVILEGE = 30,
	SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE = 31,
	SE_RELABEL_PRIVILEGE = 32,
	SE_INC_WORKING_SET_PRIVILEGE = 33,
	SE_TIME_ZONE_PRIVILEGE = 34,
	SE_CREATE_SYMBOLIC_LINK_PRIVILEGE = 35,
	SE_DELEGATE_SESSION_USER_IMPERSONATE_PRIVILEGE = 36,
};

// Console
enum class consrv_api_number
{
	ConsolepOpenConsole = 512,
	ConsolepGetConsoleInput,
	ConsolepWriteConsoleInput,
	ConsolepReadConsoleOutput,
	ConsolepWriteConsoleOutput,
	ConsolepReadConsoleOutputString,
	ConsolepWriteConsoleOutputString,
	ConsolepFillConsoleOutput,
	ConsolepGetMode,
	ConsolepGetNumberOfFonts,
	ConsolepGetNumberOfInputEvents,
	ConsolepGetScreenBufferInfo,
	ConsolepGetCursorInfo,
	ConsolepGetMouseInfo,
	ConsolepGetFontInfo,
	ConsolepGetFontSize,
	ConsolepGetCurrentFont,
	ConsolepSetMode,
	ConsolepSetActiveScreenBuffer,
	ConsolepFlushInputBuffer,
	ConsolepGetLargestWindowSize,
	ConsolepSetScreenBufferSize,
	ConsolepSetCursorPosition,
	ConsolepSetCursorInfo,
	ConsolepSetWindowInfo,
	ConsolepScrollScreenBuffer,
	ConsolepSetTextAttribute,
	ConsolepSetFont,
	ConsolepSetIcon,
	ConsolepReadConsole,
	ConsolepWriteConsole,
	ConsolepDuplicateHandle,
	ConsolepGetHandleInformation,
	ConsolepSetHandleInformation,
	ConsolepCloseHandle,
	ConsolepVerifyIoHandle,
	ConsolepAlloc, // Not present in Win7
	ConsolepFree,  // Not present in Win7
	ConsolepGetTitle,
	ConsolepSetTitle,
	ConsolepCreateScreenBuffer,
	ConsolepInvalidateBitMapRect,
	ConsolepVDMOperation,
	ConsolepSetCursor,
	ConsolepShowCursor,
	ConsolepMenuControl,
	ConsolepSetPalette,
	ConsolepSetDisplayMode,
	ConsolepRegisterVDM,
	ConsolepGetHardwareState,
	ConsolepSetHardwareState,
	ConsolepGetDisplayMode,
	ConsolepAddAlias,
	ConsolepGetAlias,
	ConsolepGetAliasesLength,
	ConsolepGetAliasExesLength,
	ConsolepGetAliases,
	ConsolepGetAliasExes,
	ConsolepExpungeCommandHistory,
	ConsolepSetNumberOfCommands,
	ConsolepGetCommandHistoryLength,
	ConsolepGetCommandHistory,
	ConsolepSetCommandHistoryMode, // Not present in Vista+
	ConsolepGetCP,
	ConsolepSetCP,
	ConsolepSetKeyShortcuts,
	ConsolepSetMenuClose,
	ConsolepNotifyLastClose,
	ConsolepGenerateCtrlEvent,
	ConsolepGetKeyboardLayoutName,
	ConsolepGetConsoleWindow,
	ConsolepCharType,
	ConsolepSetLocalEUDC,
	ConsolepSetCursorMode,
	ConsolepGetCursorMode,
	ConsolepRegisterOS2,
	ConsolepSetOS2OemFormat,
	ConsolepGetNlsMode,
	ConsolepSetNlsMode,
	ConsolepRegisterConsoleIME,   // Not present in Win7
	ConsolepUnregisterConsoleIME, // Not present in Win7
	// ConsolepQueryConsoleIME,                // Added only in Vista and Win2k8, not present in Win7
	ConsolepGetLangId,
	ConsolepAttach, // Not present in Win7
	ConsolepGetSelectionInfo,
	ConsolepGetProcessList,

	ConsolepGetHistory, // Added in Vista+
	ConsolepSetHistory, // Added in Vista+
	// ConsolepSetCurrentFont,                 // Added in Vista+
	// ConsolepSetScreenBufferInfo,            // Added in Vista+
	// ConsolepClientConnect,                  // Added in Win7

	ConsolepMaxApiNumber
};

//
// See https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/ns-shlobj_core-nt_console_props
//

struct coord
{
	::std::int_least16_t X;
	::std::int_least16_t Y;
};

struct console_properties
{
	int IconIndex;
	void *hIcon;
	void *hIconSm;
	::std::uint_least32_t dwHotKey;
	::std::uint_least32_t dwStartupFlags;

	// NT_CONSOLE_PROPS
	::std::int_least32_t wFillAttribute;
	::std::int_least32_t wPopupFillAttribute;

	//
	// Not on MSDN, but show up in binary
	//
	::std::int_least32_t wShowWindow;
	::std::int_least32_t wUnknown;

	coord dwScreenBufferSize;
	coord dwWindowSize;
	coord dwWindowOrigin;
	::std::uint_least32_t nFont;
	::std::uint_least32_t nInputBufferSize;
	coord dwFontSize;
	::std::int_least32_t uFontFamily;
	::std::int_least32_t uFontWeight;
	char16_t FaceName[32];
	::std::int_least32_t uCursorSize;
	int bFullScreen;
	int bQuickEdit;
	int bInsertMode;
	int bAutoPosition;
	::std::int_least32_t uHistoryBufferSize;
	::std::int_least32_t uNumberOfHistoryBuffers;
	int bHistoryNoDup;
	::std::int_least32_t ColorTable[16];

	// NT_FE_CONSOLE_PROPS
	::std::int_least32_t uCodePage;
};

struct console_start_info
{
	void *ConsoleHandle;
	void *InputWaitHandle;
	void *InputHandle;
	void *OutputHandle;
	void *ErrorHandle;
	void *InitEvents[2];

	console_properties cp;
};

using pthread_start_routine = ::std::uint_least32_t(__stdcall *)(void *);

struct consrv_api_connectinfo
{
	console_start_info ConsoleStartInfo;

	::std::uint_least8_t IsConsoleApp;
	::std::uint_least8_t IsWindowVisible;

	// ::std::uint_least16_t Padding;

	pthread_start_routine CtrlRoutine;
	pthread_start_routine PropRoutine;
	pthread_start_routine ImeRoutine;

	::std::uint_least32_t TitleLength;
	char16_t ConsoleTitle[260 + 1]; // Console title or full path to the startup shortcut
	::std::uint_least32_t DesktopLength;
	char16_t *Desktop; // Contrary to the case of console_allocconsole, the
	// desktop string is allocated in the process' heap,
	// and CSR will read it via NtReadVirtualMemory.
	::std::uint_least32_t AppNameLength;
	char16_t AppName[128]; // Full path of the launched app
	::std::uint_least32_t CurDirLength;
	char16_t CurDir[260 + 1];
};

struct console_getprocesslist
{
	void *ConsoleHandle;
	::std::uint_least32_t ProcessCount;
	::std::uint_least32_t ProcessIdsList;
};

struct console_generatectrlevent
{
	void *ConsoleHandle;
	::std::uint_least32_t CtrlEvent;
	::std::uint_least32_t ProcessGroupId;
};

struct console_notifylastclose
{
	void *ConsoleHandle;
};

struct console_writeconsole
{
	void *ConsoleHandle;
	void *OutputHandle;

	char StaticBuffer[80];
	void *Buffer; // BufPtr
	::std::uint_least32_t NumBytes;

	// On Windows, the client never uses this member
	::std::uint_least32_t Reserved1;

	::std::uint_least8_t UsingStaticBuffer;
	::std::uint_least8_t Unicode;

	// On Windows, the client never uses this member
	char Reserved2[6];
};

struct console_readconsole
{
	void *ConsoleHandle;
	void *InputHandle;

	::std::uint_least16_t ExeLength;

	char StaticBuffer[80];
	void *Buffer; // BufPtr
	::std::uint_least32_t NumBytes;

	::std::uint_least32_t CaptureBufferSize;

	::std::uint_least32_t InitialNumBytes;
	::std::uint_least32_t CtrlWakeupMask;
	::std::uint_least32_t ControlKeyState;
	::std::uint_least8_t Unicode;
};

struct console_allocconsole
{
	console_start_info *ConsoleStartInfo;

	::std::uint_least32_t TitleLength;
	char16_t *ConsoleTitle; // Console title or full path to the startup shortcut
	::std::uint_least32_t DesktopLength;
	char16_t *Desktop;
	::std::uint_least32_t AppNameLength;
	char16_t *AppName; // Full path of the launched app
	::std::uint_least32_t CurDirLength;
	char16_t *CurDir;

	pthread_start_routine CtrlRoutine;
	pthread_start_routine PropRoutine;
};

struct console_attachconsole
{
	/*
	 * If ProcessId == ATTACH_PARENT_PROCESS == -1, then attach
	 * the current process to its parent process console.
	 */
	::std::uint_least32_t ProcessId;

	console_start_info *ConsoleStartInfo;

	pthread_start_routine CtrlRoutine;
	pthread_start_routine PropRoutine;
};

struct console_freeconsole
{
	void *ConsoleHandle;
};

struct console_getscreenbufferinfo
{
	void *ConsoleHandle;
	void *OutputHandle;
	coord ScreenBufferSize;
	coord CursorPosition;
	coord ViewOrigin;
	::std::int_least32_t Attributes;
	coord ViewSize;
	coord MaximumViewSize;
};

struct console_setcursorposition
{
	void *ConsoleHandle;
	void *OutputHandle;
	coord Position;
};

struct console_showcursor
{
	void *ConsoleHandle;
	void *OutputHandle;
	int Show;
	int RefCount;
};

struct console_setcursor
{
	void *ConsoleHandle;
	void *OutputHandle;
	void *CursorHandle;
};

struct console_cursor_info
{
	::std::uint_least32_t dwSize;
	int bVisible;
};

struct console_getsetcursorinfo
{
	void *ConsoleHandle;
	void *OutputHandle;
	console_cursor_info Info;
	/*
		::std::uint_least32_t   Size;
		::std::uint_least8_t Visible;
	*/
};

struct console_getmouseinfo
{
	void *ConsoleHandle;
	::std::uint_least32_t NumButtons;
};

struct console_settextattrib
{
	void *ConsoleHandle;
	void *OutputHandle;
	::std::int_least32_t Attributes;
};

struct console_getsetconsolemode
{
	void *ConsoleHandle;
	void *Handle;
	::std::uint_least32_t Mode;
};

struct console_getdisplaymode
{
	void *ConsoleHandle;
	::std::uint_least32_t DisplayMode; // ModeFlags
};

struct console_setdisplaymode
{
	void *ConsoleHandle;
	void *OutputHandle;
	::std::uint_least32_t DisplayMode; // ModeFlags
	coord NewSBDim;
	void *EventHandle;
};

/*
 * Console hardware states.
 */

struct console_getsethwstate
{
	void *ConsoleHandle;
	void *OutputHandle;
	::std::uint_least32_t Flags;
	::std::uint_least32_t State;
};

struct console_getnumfonts
{
	void *ConsoleHandle;
	::std::uint_least32_t NumFonts;
};

struct console_font_info
{
	::std::uint_least32_t nFont;
	coord dwFontSize;
};

struct console_getfontinfo
{
	void *ConsoleHandle;
	void *OutputHandle;
	::std::uint_least8_t MaximumWindow;
	console_font_info *FontInfo;
	::std::uint_least32_t NumFonts;
};

struct console_getfontsize
{
	void *ConsoleHandle;
	void *OutputHandle;
	::std::uint_least32_t FontIndex;
	coord FontSize;
};

struct console_getcurrentfont
{
	void *ConsoleHandle;
	void *OutputHandle;
	::std::uint_least8_t MaximumWindow;
	::std::uint_least32_t FontIndex;
	coord FontSize;
};

struct console_setfont
{
	void *ConsoleHandle;
	void *OutputHandle;
	::std::uint_least32_t FontIndex;
};

struct console_graphics_buffer_info
{
	::std::uint_least32_t dwBitMapInfoLength;
	void *lpBitMapInfo;
	::std::uint_least32_t dwUsage; // DIB_PAL_COLORS or DIB_RGB_COLORS
	void *hMutex;
	void *lpBitMap;
};

struct console_createscreenbuffer
{
	void *ConsoleHandle;
	::std::uint_least32_t DesiredAccess; // ACCESS_MASK
	int InheritHandle;
	::std::uint_least32_t ShareMode;
	/* Type of the screen buffer: CONSOLE_TEXTMODE_BUFFER or CONSOLE_GRAPHICS_BUFFER */
	::std::uint_least32_t ScreenBufferType;
	/*
	 * This structure holds the initialization information
	 * for graphics screen buffers.
	 */
	console_graphics_buffer_info GraphicsBufferInfo;
	void *hMutex;
	void *lpBitMap;
	void *OutputHandle; /* Handle to newly created screen buffer */
};

struct console_setactivescreenbuffer
{
	void *ConsoleHandle;
	void *OutputHandle; /* Handle to screen buffer to switch to */
};

struct small_rect
{
	::std::int_least16_t Left;
	::std::int_least16_t Top;
	::std::int_least16_t Right;
	::std::int_least16_t Bottom;
};

typedef struct console_invalidatedibits
{
	void *ConsoleHandle;
	void *OutputHandle;
	small_rect Region;
};

struct console_setpalette
{
	void *ConsoleHandle;
	void *OutputHandle;
	void *PaletteHandle;
	::std::int_least32_t Usage;
};

struct console_getsetconsoletitle
{
	void *ConsoleHandle;
	::std::uint_least32_t Length;
	void *Title;
	::std::uint_least8_t Unicode;
};

struct console_flushinputbuffer
{
	void *ConsoleHandle;
	void *InputHandle;
};

struct char_info
{
	union
	{
		char16_t UnicodeChar;
		char AsciiChar;
	} Char;
	::std::uint_least16_t Attributes;
};

struct console_scrollscreenbuffer
{
	void *ConsoleHandle;
	void *OutputHandle;
	small_rect ScrollRectangle;
	small_rect ClipRectangle;
	int UseClipRectangle;
	coord DestinationOrigin;
	char_info Fill;
	::std::uint_least8_t Unicode;
};


/*
 * An attribute or a character are instances of the same entity, namely
 * a "code" (what would be called an (ANSI) escape sequence). Therefore
 * encode them inside the same structure.
 */
enum class code_type
{
	CODE_ASCII = 0x01,
	CODE_UNICODE = 0x02,
	CODE_ATTRIBUTE = 0x03
};

union code_element
{
	char AsciiChar;
	char16_t UnicodeChar;
	::std::int_least32_t Attribute;
};

struct console_outputcode
{
	void *ConsoleHandle;
	void *OutputHandle;
	coord coord;

	code_type CodeType;
	char CodeStaticBuffer[80]; // == 40 * sizeof(code_element)
	void *pCode;               // Either a pointer to a character or to an attribute.
	// union
	// {
	// void*  pCode;
	// PCHAR  AsciiChar;
	// char16_t* UnicodeChar;
	// P::std::int_least32_t  Attribute;
	// } pCode;    // Either a pointer to a character or to an attribute.

	::std::uint_least32_t NumCodes;
};

struct console_filloutputcode
{
	void *ConsoleHandle;
	void *OutputHandle;
	coord Writecoord;

	code_type CodeType;
	code_element Code; // Either a character or an attribute.

	::std::uint_least32_t NumCodes;
};

struct key_event_record
{
	int bKeyDown;
	::std::uint_least16_t wRepeatCount;
	::std::uint_least16_t wVirtualKeyCode;
	::std::uint_least16_t wVirtualScanCode;
	union
	{
		char16_t UnicodeChar;
		char AsciiChar;
	} uChar;
	::std::uint_least32_t dwControlKeyState;
};

struct mouse_event_record
{
	coord dwMousePosition;
	::std::uint_least32_t dwButtonState;
	::std::uint_least32_t dwControlKeyState;
	::std::uint_least32_t dwEventFlags;
};

struct window_buffer_size_record
{
	coord dwSize;
};

struct menu_event_record
{
	::std::uint_least32_t dwCommandId;
};

struct focus_event_record
{
	int bSetFocus;
};

struct input_record
{
	::std::uint_least16_t EventType;
	union
	{
		key_event_record KeyEvent;
		mouse_event_record MouseEvent;
		window_buffer_size_record WindowBufferSizeEvent;
		menu_event_record MenuEvent;
		focus_event_record FocusEvent;
	} Event;
};

struct console_getinput
{
	void *ConsoleHandle;
	void *InputHandle;
	input_record RecordStaticBuffer[5];
	input_record *RecordBufPtr;
	::std::uint_least32_t NumRecords;
	::std::int_least32_t Flags;
	::std::uint_least8_t Unicode;
};

struct console_writeinput
{
	void *ConsoleHandle;
	void *InputHandle;
	input_record RecordStaticBuffer[5];
	input_record *RecordBufPtr;
	::std::uint_least32_t NumRecords;
	::std::uint_least8_t Unicode;
	::std::uint_least8_t AppendToEnd;
};

struct console_readoutput
{
	void *ConsoleHandle;
	void *OutputHandle;

	char_info StaticBuffer;
	char_info *CharInfo;

	small_rect ReadRegion;
	::std::uint_least8_t Unicode;
};

struct console_writeoutput
{
	void *ConsoleHandle;
	void *OutputHandle;

	char_info StaticBuffer;
	char_info *CharInfo;

	small_rect WriteRegion;
	::std::uint_least8_t Unicode;

	/*
	 * If we are going to write too large (>= 64 kB, size of the CSR heap)
	 * data buffers, we allocate a heap buffer in the process' memory, and
	 * CSR will read it via NtReadVirtualMemory.
	 */
	::std::uint_least8_t UseVirtualMemory;
};

struct console_getnuminputevents
{
	void *ConsoleHandle;
	void *InputHandle;
	::std::uint_least32_t NumberOfEvents;
};


struct console_closehandle
{
	void *ConsoleHandle;
	void *Handle;
};

struct console_verifyhandle
{
	int IsValid;
	void *ConsoleHandle;
	void *Handle;
};

struct console_duplicatehandle
{
	void *ConsoleHandle;
	void *SourceHandle;
	::std::uint_least32_t DesiredAccess; // ACCESS_MASK
	::std::uint_least8_t InheritHandle;
	::std::uint_least32_t Options;
	void *TargetHandle;
};

struct console_gethandleinfo
{
	void *ConsoleHandle;
	void *Handle;
	::std::uint_least32_t Flags;
};

struct console_sethandleinfo
{
	void *ConsoleHandle;
	void *Handle;
	::std::uint_least32_t Mask;
	::std::uint_least32_t Flags;
};

/*
 * Type of handles.
 */
enum class console_handle_type
{
	HANDLE_INPUT = 0x01,
	HANDLE_OUTPUT = 0x02
};

struct console_openconsole
{
	void *ConsoleHandle;
	console_handle_type HandleType;
	::std::uint_least32_t DesiredAccess; // ACCESS_MASK
	int InheritHandle;
	::std::uint_least32_t ShareMode;
	void *Handle;
};


struct console_getlargestwindowsize
{
	void *ConsoleHandle;
	void *OutputHandle;
	coord Size;
};

struct console_menucontrol
{
	void *ConsoleHandle;
	void *OutputHandle;
	::std::uint_least32_t CmdIdLow;
	::std::uint_least32_t CmdIdHigh;
	void *MenuHandle;
};

struct console_setmenuclose
{
	void *ConsoleHandle;
	int Enable;
};

struct console_setwindowinfo
{
	void *ConsoleHandle;
	void *OutputHandle;
	int Absolute;
	small_rect WindowRect; // New console window position in the screen-buffer frame (Absolute == TRUE)
						   // or in the old window position frame (Absolute == FALSE).
};

struct console_getwindow
{
	void *ConsoleHandle;
	void *WindowHandle;
};

struct console_seticon
{
	void *ConsoleHandle;
	void *IconHandle;
};


struct console_addgetalias
{
	void *ConsoleHandle;
	::std::uint_least16_t SourceLength;
	::std::uint_least16_t TargetLength; // Also used for storing the number of bytes written.
	::std::uint_least16_t ExeLength;
	void *Source;
	void *Target;
	void *ExeName;
	::std::uint_least8_t Unicode;
	::std::uint_least8_t Unicode2;
};

struct console_getallaliases
{
	void *ConsoleHandle;
	::std::uint_least16_t ExeLength;
	void *ExeName;
	::std::uint_least8_t Unicode;
	::std::uint_least8_t Unicode2;
	::std::uint_least32_t AliasesBufferLength;
	void *AliasesBuffer;
};

struct console_getallaliaseslength
{
	void *ConsoleHandle;
	::std::uint_least16_t ExeLength;
	void *ExeName;
	::std::uint_least32_t Length;
	::std::uint_least8_t Unicode;
	::std::uint_least8_t Unicode2;
};

struct console_getaliasesexes
{
	void *ConsoleHandle;
	::std::uint_least32_t Length; // ExeLength; // ExesLength
	void *ExeNames;
	::std::uint_least8_t Unicode;
};

struct console_getaliasesexeslength
{
	void *ConsoleHandle;
	::std::uint_least32_t Length;
	::std::uint_least8_t Unicode;
};

struct console_getcommandhistory
{
	void *ConsoleHandle;
	::std::uint_least32_t HistoryLength;
	void *History;
	::std::uint_least16_t ExeLength;
	void *ExeName;
	::std::uint_least8_t Unicode;
	::std::uint_least8_t Unicode2;
};

struct console_getcommandhistorylength
{
	void *ConsoleHandle;
	::std::uint_least32_t HistoryLength;
	::std::uint_least16_t ExeLength;
	void *ExeName;
	::std::uint_least8_t Unicode;
	::std::uint_least8_t Unicode2;
};

struct console_expungecommandhistory
{
	void *ConsoleHandle;
	::std::uint_least16_t ExeLength;
	void *ExeName;
	::std::uint_least8_t Unicode;
	::std::uint_least8_t Unicode2;
};

struct console_getsethistoryinfo
{
	::std::int_least32_t HistoryBufferSize;
	::std::int_least32_t NumberOfHistoryBuffers;
	::std::uint_least32_t dwFlags;
};

struct console_sethistorynumbercommands
{
	void *ConsoleHandle;
	::std::uint_least32_t NumCommands;
	::std::uint_least16_t ExeLength;
	void *ExeName;
	::std::uint_least8_t Unicode;
	::std::uint_least8_t Unicode2;
};

struct console_srthistorymode
{
	void *ConsoleHandle;
	::std::uint_least32_t Mode;
};

struct console_setscreenbuffersize
{
	void *ConsoleHandle;
	void *OutputHandle;
	coord Size;
};

struct console_selection_info
{
	::std::uint_least32_t dwFlags;
	coord dwSelectionAnchor;
	small_rect srSelection;
};

struct console_getselectioninfo
{
	void *ConsoleHandle;
	console_selection_info Info;
};

struct console_getinputoutputcp
{
	void *ConsoleHandle;
	::std::int_least32_t CodePage;
	int OutputCP; // TRUE : Output Code Page ; FALSE : Input Code Page
};

struct console_setinputoutputcp
{
	void *ConsoleHandle;
	::std::int_least32_t CodePage;
	int OutputCP; // TRUE : Output Code Page ; FALSE : Input Code Page
	void *EventHandle;
};

struct console_getlangid
{
	void *ConsoleHandle;
	::std::int_least16_t LangId;
};

struct console_getkbdlayoutname
{
	void *ConsoleHandle;
	char LayoutBuffer[9 * sizeof(char16_t)]; // Can hold up to 9 wchars
	int Ansi;
};

struct console_registervdm
{
	void *ConsoleHandle;
	::std::uint_least32_t RegisterFlags;
	void *StartHardwareEvent;
	void *EndHardwareEvent;
	void *ErrorHardwareEvent;

	/* Unused member */
	::std::uint_least32_t UnusedVar;

	::std::uint_least32_t VideoStateLength;
	void *VideoState; // PVIDEO_HARDWARE_STATE_HEADER

	/* Unused members */
	void *UnusedBuffer;
	::std::uint_least32_t UnusedBufferLength;

	coord VDMBufferSize;
	void *VDMBuffer;
};

struct console_registerconsoleime
{
	void *ConsoleHandle;
	void *hWnd;
	::std::uint_least32_t dwThreadId;
	::std::uint_least32_t cbDesktop;
	char16_t *pDesktop;
	::std::uint_least32_t dwAttachToThreadId;
};

struct console_unregisterconsoleime
{
	void *ConsoleHandle;
	::std::uint_least32_t dwThreadId;
};

struct csr_capture_buffer
{
	::std::uint_least32_t Size;
	csr_capture_buffer *PreviousCaptureBuffer;
	::std::uint_least32_t PointerCount;
	void *BufferEnd;
	::std::size_t PointerOffsetsArray[1];
};

struct console_api_message
{
	port_message Header;

	csr_capture_buffer *CsrCaptureData;
	::std::uint_least32_t ApiNumber;
	::std::int_least32_t Status;
	::std::uint_least32_t Reserved;
	union
	{
		/* Console initialization and uninitialization */
		console_allocconsole AllocConsoleRequest;
		console_attachconsole AttachConsoleRequest;
		console_freeconsole FreeConsoleRequest;

		/* Processes */
		console_getprocesslist GetProcessListRequest;
		console_generatectrlevent GenerateCtrlEventRequest;
		console_notifylastclose NotifyLastCloseRequest;

		/* Handles */
		console_openconsole OpenConsoleRequest;
		console_closehandle CloseHandleRequest;
		console_verifyhandle VerifyHandleRequest;
		console_duplicatehandle DuplicateHandleRequest;
		console_gethandleinfo GetHandleInfoRequest;
		console_sethandleinfo SetHandleInfoRequest;

		/* Cursor & Mouse */
		console_showcursor ShowCursorRequest;
		console_setcursor SetCursorRequest;
		console_getsetcursorinfo CursorInfoRequest;
		console_setcursorposition SetCursorPositionRequest;
		console_getmouseinfo GetMouseInfoRequest;

		/* Screen-buffer */
		console_createscreenbuffer CreateScreenBufferRequest;
		console_setactivescreenbuffer SetScreenBufferRequest;
		console_getscreenbufferinfo ScreenBufferInfoRequest;
		console_setscreenbuffersize SetScreenBufferSizeRequest;
		console_scrollscreenbuffer ScrollScreenBufferRequest;

		console_getselectioninfo GetSelectionInfoRequest;
		console_flushinputbuffer FlushInputBufferRequest;

		/* Console mode */
		console_getsetconsolemode ConsoleModeRequest;
		console_getdisplaymode GetDisplayModeRequest;
		console_setdisplaymode SetDisplayModeRequest;
		console_getsethwstate HardwareStateRequest;

		/* Console fonts */
		console_getnumfonts GetNumFontsRequest;
		console_getfontinfo GetFontInfoRequest;
		console_getfontsize GetFontSizeRequest;
		console_getcurrentfont GetCurrentFontRequest;
		console_setfont SetFontRequest;

		/* Console window */
		console_invalidatedibits InvalidateDIBitsRequest;
		console_setpalette SetPaletteRequest;
		console_getsetconsoletitle TitleRequest;
		console_getlargestwindowsize GetLargestWindowSizeRequest;
		console_menucontrol MenuControlRequest;
		console_setmenuclose SetMenuCloseRequest;
		console_setwindowinfo SetWindowInfoRequest;
		console_getwindow GetWindowRequest;
		console_seticon SetIconRequest;

		/* Read */
		console_readconsole ReadConsoleRequest;   // SrvReadConsole / ReadConsole
		console_getinput GetInputRequest;         // SrvGetConsoleInput / PeekConsoleInput & ReadConsoleInput
		console_readoutput ReadOutputRequest;     // SrvReadConsoleOutput / ReadConsoleOutput
		console_outputcode ReadOutputCodeRequest; // SrvReadConsoleOutputString / ReadConsoleOutputAttribute & ReadConsoleOutputCharacter
		console_getnuminputevents GetNumInputEventsRequest;

		/* Write */
		console_writeconsole WriteConsoleRequest; // SrvWriteConsole / WriteConsole
		console_writeinput WriteInputRequest;
		console_writeoutput WriteOutputRequest;
		console_outputcode WriteOutputCodeRequest;

		console_filloutputcode FillOutputRequest;
		console_settextattrib SetTextAttribRequest;

		/* Aliases */
		console_addgetalias ConsoleAliasRequest;
		console_getallaliases GetAllAliasesRequest;
		console_getallaliaseslength GetAllAliasesLengthRequest;
		console_getaliasesexes GetAliasesExesRequest;
		console_getaliasesexeslength GetAliasesExesLengthRequest;

		/* History */
		console_getcommandhistory GetCommandHistoryRequest;
		console_getcommandhistorylength GetCommandHistoryLengthRequest;
		console_expungecommandhistory ExpungeCommandHistoryRequest;
		console_getsethistoryinfo HistoryInfoRequest;
		console_sethistorynumbercommands SetHistoryNumberCommandsRequest;
		console_srthistorymode SetHistoryModeRequest;

		/* Input and Output Code Pages; keyboard */
		console_getinputoutputcp GetConsoleCPRequest;
		console_setinputoutputcp SetConsoleCPRequest;
		console_getlangid LangIdRequest;
		console_getkbdlayoutname GetKbdLayoutNameRequest;

		/* Virtual DOS Machine */
		console_registervdm RegisterVDMRequest;

		/* Console IME */
		console_registerconsoleime RegisterConsoleIME;
		console_unregisterconsoleime UnregisterConsoleIME;
	} Data;
};

} // namespace fast_io::win32::nt
