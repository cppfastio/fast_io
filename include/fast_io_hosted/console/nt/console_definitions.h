#pragma once

namespace fast_io::win32::nt
{

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
	::std::uint_least32_t* ProcessIdsList; // or ::std::uint_least32_t?
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

struct console_invalidatedibits
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
	::std::uint_least32_t Status;
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
