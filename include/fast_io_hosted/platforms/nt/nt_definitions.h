#pragma once

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

struct unicode_string
{
	::std::uint_least16_t Length;
	::std::uint_least16_t MaximumLength;
	char16_t *Buffer;
};

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
	union dummyunion
	{
		::std::uint_least32_t Status;
		void *Pointer;
	} DUMMYUNIONNAME;
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
	FileFullDirectoryInformation,            // 2
	FileBothDirectoryInformation,            // 3
	FileBasicInformation,                    // 4
	FileStandardInformation,                 // 5
	FileInternalInformation,                 // 6
	FileEaInformation,                       // 7
	FileAccessInformation,                   // 8
	FileNameInformation,                     // 9
	FileRenameInformation,                   // 10
	FileLinkInformation,                     // 11
	FileNamesInformation,                    // 12
	FileDispositionInformation,              // 13
	FilePositionInformation,                 // 14
	FileFullEaInformation,                   // 15
	FileModeInformation,                     // 16
	FileAlignmentInformation,                // 17
	FileAllInformation,                      // 18
	FileAllocationInformation,               // 19
	FileEndOfFileInformation,                // 20
	FileAlternateNameInformation,            // 21
	FileStreamInformation,                   // 22
	FilePipeInformation,                     // 23
	FilePipeLocalInformation,                // 24
	FilePipeRemoteInformation,               // 25
	FileMailslotQueryInformation,            // 26
	FileMailslotSetInformation,              // 27
	FileCompressionInformation,              // 28
	FileObjectIdInformation,                 // 29
	FileCompletionInformation,               // 30
	FileMoveClusterInformation,              // 31
	FileQuotaInformation,                    // 32
	FileReparsePointInformation,             // 33
	FileNetworkOpenInformation,              // 34
	FileAttributeTagInformation,             // 35
	FileTrackingInformation,                 // 36
	FileIdBothDirectoryInformation,          // 37
	FileIdFullDirectoryInformation,          // 38
	FileValidDataLengthInformation,          // 39
	FileShortNameInformation,                // 40
	FileIoCompletionNotificationInformation, // 41
	FileIoStatusBlockRangeInformation,       // 42
	FileIoPriorityHintInformation,           // 43
	FileSfioReserveInformation,              // 44
	FileSfioVolumeInformation,               // 45
	FileHardLinkInformation,                 // 46
	FileProcessIdsUsingFileInformation,      // 47
	FileNormalizedNameInformation,           // 48
	FileNetworkPhysicalNameInformation,      // 49
	FileIdGlobalTxDirectoryInformation,      // 50
	FileIsRemoteDeviceInformation,           // 51
	FileUnusedInformation,                   // 52
	FileNumaNodeInformation,                 // 53
	FileStandardLinkInformation,             // 54
	FileRemoteProtocolInformation,           // 55

	//
	//  These are special versions of these operations (defined earlier)
	//  which can be used by kernel mode drivers only to bypass security
	//  access checks for Rename and HardLink operations.  These operations
	//  are only recognized by the IOManager, a file system should never
	//  receive these.
	//

	FileRenameInformationBypassAccessCheck, // 56
	FileLinkInformationBypassAccessCheck,   // 57

	//
	// End of special information classes reserved for IOManager.
	//

	FileVolumeNameInformation,                    // 58
	FileIdInformation,                            // 59
	FileIdExtdDirectoryInformation,               // 60
	FileReplaceCompletionInformation,             // 61
	FileHardLinkFullIdInformation,                // 62
	FileIdExtdBothDirectoryInformation,           // 63
	FileDispositionInformationEx,                 // 64
	FileRenameInformationEx,                      // 65
	FileRenameInformationExBypassAccessCheck,     // 66
	FileDesiredStorageClassInformation,           // 67
	FileStatInformation,                          // 68
	FileMemoryPartitionInformation,               // 69
	FileStatLxInformation,                        // 70
	FileCaseSensitiveInformation,                 // 71
	FileLinkInformationEx,                        // 72
	FileLinkInformationExBypassAccessCheck,       // 73
	FileStorageReserveIdInformation,              // 74
	FileCaseSensitiveInformationForceAccessCheck, // 75
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

struct client_id
{
	void *hprocess;
	void *hthread;
};

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

struct section_image_information
{
	void *TransferAddress;
	::std::uint_least32_t ZeroBits;
	::std::size_t MaximumStackSize;
	::std::size_t CommittedStackSize;
	::std::uint_least32_t SubSystemType;
	union
	{
		struct
		{
			::std::uint_least16_t SubSystemMinorVersion;
			::std::uint_least16_t SubSystemMajorVersion;
		};
		::std::uint_least32_t SubSystemVersion;
	};
	union
	{
		struct
		{
			::std::uint_least16_t MajorOperatingSystemVersion;
			::std::uint_least16_t MinorOperatingSystemVersion;
		};
		::std::uint_least32_t OperatingSystemVersion;
	};
	::std::uint_least16_t ImageCharacteristics;
	::std::uint_least16_t DllCharacteristics;
	::std::uint_least16_t Machine;
	::std::uint_least8_t ImageContainsCode;
	union
	{
		::std::uint_least8_t ImageFlags;
		struct
		{
			::std::uint_least8_t ComPlusNativeReady : 1;
			::std::uint_least8_t ComPlusILOnly : 1;
			::std::uint_least8_t ImageDynamicallyRelocated : 1;
			::std::uint_least8_t ImageMappedFlat : 1;
			::std::uint_least8_t BaseBelow4gb : 1;
			::std::uint_least8_t ComPlusPrefer32bit : 1;
			::std::uint_least8_t Reserved : 2;
		};
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
	ObjectBasicInformation = 0,
	ObjectNameInformation = 1,
	ObjectTypeInformation = 2,
	ObjectAllTypesInformation = 3,
	ObjectHandleInformation = 4
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

struct ps_std_handle_info
{
	union
	{
		::std::uint_least32_t Flags; // 0x121 = 100100001
		struct
		{
			::std::uint_least32_t StdHandleState : 2;   // PS_STD_HANDLE_STATE
			::std::uint_least32_t PseudoHandleMask : 3; // PS_STD_*
		};
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
	void* RootDirectory;
	::std::uint_least32_t FileNameLength;
	char16_t FileName[1];
};

struct file_rename_information
{
	::std::uint_least8_t ReplaceIfExists;
	void* RootDirectory;
	::std::uint_least32_t FileNameLength;
	char16_t FileName[1];
};

struct reparse_data_buffer
{
	::std::uint_least32_t ReparseTag;
	::std::uint_least16_t ReparseDataLength;
	::std::uint_least16_t Reserved;
	union
	{
		struct
		{
			::std::uint_least16_t SubstituteNameOffset;
			::std::uint_least16_t SubstituteNameLength;
			::std::uint_least16_t PrintNameOffset;
			::std::uint_least16_t PrintNameLength;
			::std::uint_least32_t Flags;
			char16_t PathBuffer[1];
		} SymbolicLinkReparseBuffer;
		struct
		{
			::std::uint_least16_t SubstituteNameOffset;
			::std::uint_least16_t SubstituteNameLength;
			::std::uint_least16_t PrintNameOffset;
			::std::uint_least16_t PrintNameLength;
			char16_t PathBuffer[1];
		} MountPointReparseBuffer;
		struct
		{
			::std::uint_least8_t DataBuffer[1];
		} GenericReparseBuffer;
	};
};

struct file_disposition_information
{
	::std::uint_least8_t DeleteFile;
};
} // namespace fast_io::win32::nt
