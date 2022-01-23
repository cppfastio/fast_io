#include <cstdint>
#include <cstddef>
#include "stivale2.h"
#include "../../../include/fast_io.h"
#include "../../../include/fast_io_driver/limine/kernel_console.h"

namespace fast_io_kernel_demo
{
// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialised array in .bss.
std::byte kernel_stack[8192];

// stivale2 uses a linked list of tags for both communicating TO the
// bootloader, or receiving info FROM it. More information about these tags
// is found in the stivale2 specification.

// stivale2 offers a runtime terminal service which can be ditched at any
// time, but it provides an easy way to print out to graphical terminal,
// especially during early boot.

inline constexpr stivale2_header_tag_terminal create_terminal_hdr_tag() noexcept
{
	stivale2_header_tag_terminal v{};
	v.tag.identifier=STIVALE2_HEADER_TAG_TERMINAL_ID;
	v.tag.next={};
	return v;
}

static constinit stivale2_header_tag_terminal terminal_hdr_tag{create_terminal_hdr_tag()};

// We are now going to define a framebuffer header tag.
// This tag tells the bootloader that we want a graphical framebuffer instead
// of a CGA-compatible text mode. Omitting this tag will make the bootloader
// default to text mode, if available.
static stivale2_header_tag_framebuffer framebuffer_hdr_tag
{
	// Same as above.
	.tag={
		STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
		// Instead of 0, we now point to the previous header tag. The order in
		// which header tags are linked does not matter.
		(uint64_t)__builtin_addressof(terminal_hdr_tag)
	},
	.framebuffer_width=0,
	.framebuffer_height=0,
	.framebuffer_bpp=0,
	.unused={}
	// We set all the framebuffer specifics to 0 as we want the bootloader
	// to pick the best it can.
};

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
[[gnu::section(".stivale2hdr"),gnu::used]]
static struct stivale2_header stivale_hdr
{
	// The entry_point member is used to specify an alternative entry
	// point that the bootloader should jump to instead of the executable's
	// ELF entry point. We do not care about that so we leave it zeroed.
	.entry_point = 0,
	// Let's tell the bootloader where our stack is.
	// We need to add the sizeof(stack) since in x86(_64) the stack grows
	// downwards.
	.stack = (uintptr_t)kernel_stack + sizeof(kernel_stack),
	// Bit 1, if set, causes the bootloader to return to us pointers in the
	// higher half, which we likely want.
	// Bit 2, if set, tells the bootloader to enable protected memory ranges,
	// that is, to respect the ELF PHDR mandated permissions for the executable's
	// segments.
	.flags = (1 << 1) | (1 << 2),
	// This header structure is the root of the linked list of header tags and
	// points to the first one in the linked list.
	.tags = (uintptr_t)__builtin_addressof(framebuffer_hdr_tag)
};

// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).
inline void *stivale2_get_tag(stivale2_struct *stvl2, std::uint_least64_t id) noexcept {
	stivale2_tag *current_tag = __builtin_bit_cast(stivale2_tag*,stvl2->tags);
	for (;;) {
		// If the tag pointer is nullptr (end of linked list), we did not find
		// the tag. Return nullptr to signal this.
		if (current_tag == nullptr) {
			__builtin_trap();
		}

		// Check whether the identifier matches. If it does, return a pointer
		// to the matching tag.
		if (current_tag->identifier == id) {
			return current_tag;
		}

		// Get a pointer to the next tag in the linked list and repeat.
		current_tag = __builtin_bit_cast(stivale2_tag*,current_tag->next);
	}
}

template<typename T>
inline T& stivale2_get(stivale2_struct *stvl2, std::uint_least64_t id) noexcept
{
	return *reinterpret_cast<T*>(stivale2_get_tag(stvl2,id));
}

// The following will be our kernel's entry point.
extern "C"
[[gnu::used]]
void _start(struct stivale2_struct *stivale2_struct) noexcept
{
	// Let's get the terminal structure tag from the bootloader.
	auto& term{stivale2_get<stivale2_struct_tag_terminal>(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID)};

	// Let's get the address of the terminal write function.
	// Now, let's assign this pointer to a function pointer which
	// matches the prototype described in the stivale2 specification for
	// the stivale2_term_write function.
	using function_ptr = fast_io::limine::u8kernel_console::function_ptr_type;
	fast_io::limine::u8kernel_console console{__builtin_bit_cast(function_ptr,term.term_write)};


	auto& epoch{stivale2_get<stivale2_struct_tag_epoch>(stivale2_struct, STIVALE2_STRUCT_TAG_EPOCH_ID)};
	fast_io::unix_timestamp tsp{static_cast<std::int_least64_t>(epoch.epoch)};
	println(console,u8"Hello fast_io Kernel: stivale2_struct address:",stivale2_struct,u8"\n"
	u8"Unix Tiemstamp:",tsp,u8"\n"
	u8"UTC:",utc(tsp));

	// We're done, just hang...
	for (;;)
	{
		asm ("hlt");
	}
}

}
