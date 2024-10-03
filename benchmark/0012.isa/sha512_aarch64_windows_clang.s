	.text
	.def	"@feat.00";
	.scl	3;
	.type	0;
	.endef
	.globl	"@feat.00"
.set "@feat.00", 0
	.file	"sha512.cc"
	.def	"?sha512_invoker@@YAXPEA_KPEBW4byte@std@@1@Z";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sha512_invoker@@YAXPEA_KPEBW4byte@std@@1@Z" // -- Begin function ?sha512_invoker@@YAXPEA_KPEBW4byte@std@@1@Z
	.p2align	2
"?sha512_invoker@@YAXPEA_KPEBW4byte@std@@1@Z": // @"?sha512_invoker@@YAXPEA_KPEBW4byte@std@@1@Z"
.seh_proc "?sha512_invoker@@YAXPEA_KPEBW4byte@std@@1@Z"
// %bb.0:
	sub	sp, sp, #272
	.seh_stackalloc	272
	stp	x19, x20, [sp, #176]            // 16-byte Folded Spill
	.seh_save_regp	x19, 176
	stp	x21, x22, [sp, #192]            // 16-byte Folded Spill
	.seh_save_regp	x21, 192
	stp	x23, x24, [sp, #208]            // 16-byte Folded Spill
	.seh_save_regp	x23, 208
	stp	x25, x26, [sp, #224]            // 16-byte Folded Spill
	.seh_save_regp	x25, 224
	stp	x27, x28, [sp, #240]            // 16-byte Folded Spill
	.seh_save_regp	x27, 240
	stp	x29, x30, [sp, #256]            // 16-byte Folded Spill
	.seh_save_fplr	256
	.seh_endprologue
	cmp	x1, x2
	str	x2, [sp, #16]                   // 8-byte Folded Spill
	b.eq	.LBB0_6
// %bb.1:
	mov	x27, x1
	ldp	x17, x15, [x0, #48]
	ldp	x3, x1, [x0, #32]
	str	x0, [sp, #8]                    // 8-byte Folded Spill
	ldp	x4, x2, [x0, #16]
	ldp	x6, x5, [x0]
.LBB0_2:                                // =>This Loop Header: Depth=1
                                        //     Child Loop BB0_3 Depth 2
	bic	x9, x17, x3
	and	x10, x1, x3
	ror	x8, x3, #14
	ldp	x12, x14, [x27]
	orr	x9, x10, x9
	ror	x11, x6, #28
	str	x15, [sp, #88]                  // 8-byte Folded Spill
	add	x9, x15, x9
	mov	x15, #44578                     // =0xae22
	eor	x8, x8, x3, ror #18
	eor	x16, x5, x6
	movk	x15, #55080, lsl #16
	rev	x13, x12
	eor	x10, x11, x6, ror #34
	movk	x15, #12184, lsl #32
	add	x9, x9, x13
	eor	x8, x8, x3, ror #41
	movk	x15, #17034, lsl #48
	bic	x11, x5, x16
	and	x12, x4, x16
	add	x9, x9, x15
	eor	x10, x10, x6, ror #39
	orr	x11, x12, x11
	add	x8, x8, x9
	rev	x15, x14
	str	x2, [sp, #48]                   // 8-byte Folded Spill
	add	x9, x11, x10
	add	x12, x8, x2
	str	x15, [sp, #128]                 // 8-byte Folded Spill
	add	x8, x9, x8
	bic	x11, x1, x12
	and	x14, x3, x12
	ror	x9, x12, #14
	ror	x10, x8, #28
	add	x15, x17, x15
	orr	x11, x14, x11
	eor	x14, x8, x6
	stp	x1, x17, [sp, #64]              // 16-byte Folded Spill
	add	x11, x15, x11
	and	x15, x14, x16
	mov	x16, #26061                     // =0x65cd
	eor	x9, x9, x12, ror #18
	eor	x10, x10, x8, ror #34
	movk	x16, #9199, lsl #16
	movk	x16, #17553, lsl #32
	eor	x15, x15, x6
	eor	x9, x9, x12, ror #41
	movk	x16, #28983, lsl #48
	eor	x10, x10, x8, ror #39
	add	x11, x11, x16
	str	x3, [sp, #56]                   // 8-byte Folded Spill
	adrp	x28, "?k512scalar@sha512@details@fast_io@@3U?$array@_K$0EA@@freestanding@3@B"+64
	add	x28, x28, :lo12:"?k512scalar@sha512@details@fast_io@@3U?$array@_K$0EA@@freestanding@3@B"+64
	add	x9, x11, x9
	add	x10, x10, x15
	ldp	x11, x15, [x27, #16]
	add	x0, x9, x4
	add	x10, x10, x9
	bic	x16, x3, x0
	and	x17, x12, x0
	ror	x9, x0, #14
	orr	x16, x17, x16
	eor	x17, x10, x8
	rev	x2, x11
	ror	x11, x10, #28
	and	x14, x17, x14
	add	x1, x1, x2
	eor	x9, x9, x0, ror #18
	eor	x14, x14, x8
	add	x16, x1, x16
	mov	x1, #15151                      // =0x3b2f
	eor	x11, x11, x10, ror #34
	movk	x1, #60493, lsl #16
	eor	x9, x9, x0, ror #41
	rev	x15, x15
	movk	x1, #64463, lsl #32
	eor	x11, x11, x10, ror #39
	stp	x15, x2, [sp, #160]             // 16-byte Folded Spill
	movk	x1, #46528, lsl #48
	add	x3, x3, x15
	add	x16, x16, x1
	add	x11, x11, x14
	mov	x15, #56252                     // =0xdbbc
	add	x9, x16, x9
	movk	x15, #33161, lsl #16
	stp	x5, x4, [sp, #32]               // 16-byte Folded Spill
	add	x14, x9, x5
	add	x9, x11, x9
	movk	x15, #56229, lsl #32
	ror	x16, x9, #28
	ror	x11, x14, #14
	bic	x1, x12, x14
	and	x2, x0, x14
	movk	x15, #59829, lsl #48
	eor	x16, x16, x9, ror #34
	eor	x11, x11, x14, ror #18
	orr	x1, x2, x1
	eor	x2, x9, x10
	add	x1, x3, x1
	str	x6, [sp, #24]                   // 8-byte Folded Spill
	and	x17, x2, x17
	eor	x16, x16, x9, ror #39
	eor	x11, x11, x14, ror #41
	eor	x17, x17, x10
	add	x1, x1, x15
	str	x27, [sp, #80]                  // 8-byte Folded Spill
	add	x16, x16, x17
	ldp	x17, x3, [x27, #32]
	add	x11, x1, x11
	add	x1, x11, x6
	add	x11, x16, x11
	ror	x16, x1, #14
	rev	x15, x17
	ror	x4, x11, #28
	str	x15, [sp, #152]                 // 8-byte Folded Spill
	add	x12, x12, x15
	mov	x15, #46392                     // =0xb538
	bic	x5, x0, x1
	and	x6, x14, x1
	eor	x16, x16, x1, ror #18
	eor	x4, x4, x11, ror #34
	movk	x15, #62280, lsl #16
	orr	x5, x6, x5
	eor	x6, x11, x9
	movk	x15, #49755, lsl #32
	add	x12, x12, x5
	eor	x16, x16, x1, ror #41
	and	x2, x6, x2
	movk	x15, #14678, lsl #48
	eor	x4, x4, x11, ror #39
	add	x12, x12, x15
	eor	x2, x2, x9
	add	x12, x12, x16
	mov	x15, #53273                     // =0xd019
	add	x2, x4, x2
	add	x16, x12, x8
	rev	x4, x3
	add	x8, x2, x12
	ror	x12, x16, #14
	bic	x3, x14, x16
	ror	x2, x8, #28
	and	x5, x1, x16
	movk	x15, #46597, lsl #16
	eor	x12, x12, x16, ror #18
	add	x0, x4, x0
	orr	x3, x5, x3
	eor	x2, x2, x8, ror #34
	eor	x7, x8, x11
	movk	x15, #4593, lsl #32
	add	x0, x0, x3
	eor	x12, x12, x16, ror #41
	and	x3, x7, x6
	movk	x15, #23025, lsl #48
	eor	x2, x2, x8, ror #39
	eor	x3, x3, x11
	add	x0, x0, x15
	add	x12, x0, x12
	add	x0, x2, x3
	add	x2, x12, x10
	add	x10, x0, x12
	ldp	x3, x0, [x27, #48]
	ror	x12, x2, #14
	bic	x6, x1, x2
	and	x19, x16, x2
	eor	x12, x12, x2, ror #18
	orr	x6, x19, x6
	eor	x19, x10, x8
	rev	x15, x3
	ror	x3, x10, #28
	str	x15, [sp, #144]                 // 8-byte Folded Spill
	add	x14, x15, x14
	mov	x15, #20379                     // =0x4f9b
	movk	x15, #44825, lsl #16
	eor	x3, x3, x10, ror #34
	add	x14, x14, x6
	movk	x15, #33444, lsl #32
	eor	x12, x12, x2, ror #41
	and	x6, x19, x7
	movk	x15, #37439, lsl #48
	eor	x3, x3, x10, ror #39
	eor	x6, x6, x8
	add	x14, x14, x15
	rev	x15, x0
	add	x12, x14, x12
	add	x3, x3, x6
	str	x15, [sp, #136]                 // 8-byte Folded Spill
	add	x14, x12, x9
	add	x9, x3, x12
	add	x1, x15, x1
	ror	x12, x14, #14
	ror	x0, x9, #28
	mov	x15, #33048                     // =0x8118
	bic	x3, x16, x14
	and	x7, x2, x14
	movk	x15, #55917, lsl #16
	eor	x12, x12, x14, ror #18
	orr	x3, x7, x3
	eor	x0, x0, x9, ror #34
	movk	x15, #24277, lsl #32
	eor	x20, x9, x10
	add	x1, x1, x3
	eor	x12, x12, x14, ror #41
	movk	x15, #43804, lsl #48
	and	x3, x20, x19
	add	x1, x1, x15
	eor	x0, x0, x9, ror #39
	eor	x3, x3, x10
	add	x12, x1, x12
	mov	x15, #578                       // =0x242
	add	x0, x0, x3
	add	x3, x12, x11
	movk	x15, #41731, lsl #16
	ldp	x11, x1, [x27, #64]
	add	x12, x0, x12
	ror	x0, x3, #14
	bic	x19, x2, x3
	and	x21, x14, x3
	orr	x19, x21, x19
	movk	x15, #43672, lsl #32
	eor	x21, x12, x9
	rev	x30, x11
	ror	x11, x12, #28
	eor	x0, x0, x3, ror #18
	add	x16, x30, x16
	movk	x15, #55303, lsl #48
	rev	x5, x1
	eor	x11, x11, x12, ror #34
	add	x16, x16, x19
	eor	x0, x0, x3, ror #41
	and	x19, x21, x20
	add	x16, x16, x15
	mov	x15, #28606                     // =0x6fbe
	eor	x11, x11, x12, ror #39
	eor	x19, x19, x9
	add	x16, x16, x0
	add	x0, x16, x8
	movk	x15, #17776, lsl #16
	add	x2, x5, x2
	add	x11, x11, x19
	bic	x1, x14, x0
	and	x20, x3, x0
	add	x8, x11, x16
	ror	x11, x0, #14
	orr	x1, x20, x1
	ror	x16, x8, #28
	movk	x15, #23297, lsl #32
	eor	x22, x8, x12
	eor	x11, x11, x0, ror #18
	add	x1, x2, x1
	movk	x15, #4739, lsl #48
	eor	x16, x16, x8, ror #34
	and	x2, x22, x21
	add	x1, x1, x15
	eor	x11, x11, x0, ror #41
	eor	x2, x2, x12
	mov	x15, #45708                     // =0xb28c
	eor	x16, x16, x8, ror #39
	movk	x15, #20196, lsl #16
	add	x11, x1, x11
	movk	x15, #34238, lsl #32
	add	x16, x16, x2
	add	x1, x11, x10
	movk	x15, #9265, lsl #48
	ldp	x10, x2, [x27, #80]
	add	x11, x16, x11
	ror	x16, x1, #14
	bic	x21, x3, x1
	and	x23, x0, x1
	orr	x21, x23, x21
	eor	x23, x11, x8
	rev	x6, x10
	ror	x10, x11, #28
	eor	x16, x16, x1, ror #18
	add	x14, x6, x14
	rev	x7, x2
	eor	x10, x10, x11, ror #34
	add	x14, x14, x21
	eor	x16, x16, x1, ror #41
	and	x21, x23, x22
	add	x14, x14, x15
	mov	x15, #46306                     // =0xb4e2
	eor	x10, x10, x11, ror #39
	eor	x21, x21, x8
	add	x16, x14, x16
	add	x14, x16, x9
	movk	x15, #54783, lsl #16
	add	x3, x7, x3
	add	x10, x10, x21
	bic	x2, x0, x14
	and	x22, x1, x14
	add	x9, x10, x16
	ror	x10, x14, #14
	orr	x2, x22, x2
	ror	x16, x9, #28
	movk	x15, #32195, lsl #32
	eor	x24, x9, x11
	eor	x10, x10, x14, ror #18
	add	x2, x3, x2
	movk	x15, #21772, lsl #48
	eor	x16, x16, x9, ror #34
	and	x3, x24, x23
	add	x2, x2, x15
	eor	x10, x10, x14, ror #41
	eor	x3, x3, x11
	mov	x15, #35183                     // =0x896f
	eor	x16, x16, x9, ror #39
	movk	x15, #62075, lsl #16
	add	x10, x2, x10
	movk	x15, #23924, lsl #32
	add	x2, x16, x3
	add	x16, x10, x12
	movk	x15, #29374, lsl #48
	ldp	x12, x3, [x27, #96]
	add	x10, x2, x10
	ror	x2, x16, #14
	bic	x23, x1, x16
	and	x25, x14, x16
	orr	x23, x25, x23
	eor	x25, x10, x9
	rev	x22, x12
	ror	x12, x10, #28
	eor	x2, x2, x16, ror #18
	add	x0, x22, x0
	eor	x12, x12, x10, ror #34
	add	x0, x0, x23
	eor	x2, x2, x16, ror #41
	and	x23, x25, x24
	add	x0, x0, x15
	mov	x15, #38577                     // =0x96b1
	eor	x12, x12, x10, ror #39
	eor	x23, x23, x9
	add	x0, x0, x2
	add	x8, x0, x8
	movk	x15, #15126, lsl #16
	add	x12, x12, x23
	rev	x23, x3
	bic	x2, x14, x8
	add	x26, x12, x0
	ror	x12, x8, #14
	and	x3, x16, x8
	ror	x0, x26, #28
	add	x1, x23, x1
	orr	x2, x3, x2
	eor	x12, x12, x8, ror #18
	eor	x3, x26, x10
	movk	x15, #45566, lsl #32
	eor	x0, x0, x26, ror #34
	add	x1, x1, x2
	and	x2, x3, x25
	eor	x12, x12, x8, ror #41
	movk	x15, #32990, lsl #48
	eor	x2, x2, x10
	eor	x0, x0, x26, ror #39
	add	x1, x1, x15
	mov	x15, #4661                      // =0x1235
	add	x12, x1, x12
	movk	x15, #9671, lsl #16
	add	x1, x0, x2
	add	x0, x12, x11
	movk	x15, #1703, lsl #32
	add	x2, x1, x12
	ldp	x11, x1, [x27, #112]
	ror	x12, x0, #14
	bic	x25, x16, x0
	and	x27, x8, x0
	orr	x25, x27, x25
	eor	x27, x2, x26
	movk	x15, #39900, lsl #48
	rev	x24, x11
	ror	x11, x2, #28
	eor	x12, x12, x0, ror #18
	add	x14, x24, x14
	and	x3, x27, x3
	eor	x11, x11, x2, ror #34
	add	x14, x14, x25
	eor	x12, x12, x0, ror #41
	add	x14, x14, x15
	eor	x3, x3, x26
	mov	x15, #9876                      // =0x2694
	eor	x11, x11, x2, ror #39
	add	x12, x14, x12
	rev	x25, x1
	add	x9, x12, x9
	movk	x15, #53097, lsl #16
	add	x16, x25, x16
	add	x11, x11, x3
	bic	x1, x8, x9
	and	x3, x0, x9
	add	x12, x11, x12
	ror	x11, x9, #14
	orr	x1, x3, x1
	ror	x14, x12, #28
	movk	x15, #61812, lsl #32
	add	x1, x16, x1
	eor	x11, x11, x9, ror #18
	eor	x16, x12, x2
	movk	x15, #49563, lsl #48
	eor	x14, x14, x12, ror #34
	and	x3, x16, x27
	add	x1, x1, x15
	eor	x11, x11, x9, ror #41
	eor	x14, x14, x12, ror #39
	add	x11, x1, x11
	eor	x1, x3, x2
	add	x14, x14, x1
	add	x10, x11, x10
	mov	w1, #512                        // =0x200
	add	x14, x14, x11
.LBB0_3:                                //   Parent Loop BB0_2 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	ldr	x17, [sp, #128]                 // 8-byte Folded Reload
	ror	x27, x10, #14
	ror	x3, x24, #19
	stp	x5, x1, [sp, #112]              // 16-byte Folded Spill
	mov	x21, x30
	add	x30, x13, x5
	ror	x11, x17, #1
	eor	x27, x27, x10, ror #18
	bic	x1, x0, x10
	and	x29, x9, x10
	eor	x3, x3, x24, ror #61
	ldr	x5, [sp, #168]                  // 8-byte Folded Reload
	eor	x11, x11, x17, ror #8
	ror	x13, x14, #28
	orr	x1, x29, x1
	ldp	x29, x15, [x28, #-64]
	eor	x27, x27, x10, ror #41
	eor	x11, x11, x17, lsr #7
	add	x8, x8, x1
	ror	x1, x5, #1
	eor	x3, x3, x24, lsr #6
	add	x8, x8, x27
	eor	x27, x13, x14, ror #34
	add	x11, x30, x11
	ror	x30, x25, #19
	add	x8, x8, x29
	add	x13, x11, x3
	eor	x11, x12, x14
	eor	x1, x1, x5, ror #8
	add	x8, x8, x13
	eor	x27, x27, x14, ror #39
	and	x16, x16, x11
	eor	x29, x30, x25, ror #61
	eor	x16, x16, x12
	add	x3, x8, x26
	eor	x1, x1, x5, lsr #7
	ror	x30, x3, #14
	add	x16, x16, x27
	eor	x26, x29, x25, lsr #6
	add	x29, x17, x6
	stp	x7, x6, [sp, #96]               // 16-byte Folded Spill
	add	x1, x29, x1
	add	x16, x16, x8
	ldr	x6, [sp, #160]                  // 8-byte Folded Reload
	add	x17, x1, x26
	eor	x8, x30, x3, ror #18
	bic	x1, x9, x3
	and	x26, x10, x3
	ror	x27, x16, #28
	add	x15, x0, x15
	orr	x1, x26, x1
	ror	x26, x6, #1
	eor	x8, x8, x3, ror #41
	add	x15, x17, x15
	eor	x0, x27, x16, ror #34
	ror	x27, x13, #19
	add	x15, x15, x1
	eor	x1, x16, x14
	eor	x26, x26, x6, ror #8
	eor	x0, x0, x16, ror #39
	and	x11, x1, x11
	add	x8, x15, x8
	eor	x15, x27, x13, ror #61
	eor	x11, x11, x14
	eor	x26, x26, x6, lsr #7
	add	x11, x0, x11
	add	x27, x5, x7
	add	x2, x8, x2
	eor	x15, x15, x13, lsr #6
	add	x0, x11, x8
	add	x8, x27, x26
	bic	x11, x10, x2
	ror	x29, x2, #14
	ror	x26, x0, #28
	add	x5, x8, x15
	and	x15, x3, x2
	ldr	x7, [sp, #152]                  // 8-byte Folded Reload
	orr	x11, x15, x11
	ldp	x27, x15, [x28, #-48]
	eor	x8, x29, x2, ror #18
	eor	x26, x26, x0, ror #34
	eor	x29, x0, x16
	ror	x30, x17, #19
	and	x1, x29, x1
	add	x9, x9, x27
	ror	x27, x7, #1
	eor	x8, x8, x2, ror #41
	add	x9, x5, x9
	eor	x26, x26, x0, ror #39
	add	x10, x10, x15
	eor	x27, x27, x7, ror #8
	add	x9, x9, x11
	eor	x11, x1, x16
	eor	x1, x30, x17, ror #61
	add	x8, x9, x8
	add	x9, x26, x11
	eor	x11, x27, x7, lsr #7
	add	x12, x8, x12
	add	x26, x6, x22
	eor	x1, x1, x17, lsr #6
	add	x8, x9, x8
	ldp	x20, x30, [sp, #136]            // 16-byte Folded Reload
	add	x9, x26, x11
	ror	x11, x12, #14
	ror	x15, x8, #28
	add	x6, x9, x1
	bic	x9, x3, x12
	and	x1, x2, x12
	eor	x11, x11, x12, ror #18
	orr	x9, x1, x9
	add	x10, x6, x10
	ror	x1, x4, #1
	add	x9, x10, x9
	ror	x10, x5, #19
	eor	x11, x11, x12, ror #41
	eor	x15, x15, x8, ror #34
	eor	x26, x8, x0
	eor	x1, x1, x4, ror #8
	eor	x10, x10, x5, ror #61
	str	x5, [sp, #168]                  // 8-byte Folded Spill
	add	x9, x9, x11
	and	x11, x26, x29
	eor	x15, x15, x8, ror #39
	eor	x1, x1, x4, lsr #7
	eor	x27, x11, x0
	add	x11, x9, x14
	eor	x10, x10, x5, lsr #6
	add	x14, x7, x23
	add	x15, x15, x27
	add	x14, x14, x1
	ror	x1, x11, #14
	add	x9, x15, x9
	add	x7, x14, x10
	ldp	x10, x27, [x28, #-32]
	eor	x14, x1, x11, ror #18
	bic	x15, x2, x11
	and	x1, x12, x11
	orr	x15, x1, x15
	ror	x1, x30, #1
	ror	x29, x6, #19
	add	x10, x3, x10
	ror	x3, x9, #28
	eor	x14, x14, x11, ror #41
	add	x10, x10, x7
	add	x4, x4, x24
	stp	x7, x6, [sp, #152]              // 16-byte Folded Spill
	add	x10, x10, x15
	eor	x15, x3, x9, ror #34
	eor	x3, x9, x8
	add	x14, x10, x14
	and	x26, x3, x26
	add	x10, x14, x16
	eor	x16, x1, x30, ror #8
	eor	x15, x15, x9, ror #39
	eor	x1, x26, x8
	eor	x26, x29, x6, ror #61
	ror	x29, x10, #14
	eor	x16, x16, x30, lsr #7
	add	x15, x15, x1
	eor	x1, x26, x6, lsr #6
	eor	x29, x29, x10, ror #18
	add	x26, x15, x14
	add	x14, x16, x4
	bic	x15, x12, x10
	and	x16, x11, x10
	add	x19, x14, x1
	add	x1, x27, x2
	ror	x2, x26, #28
	eor	x14, x29, x10, ror #41
	orr	x15, x16, x15
	add	x16, x1, x19
	ror	x1, x20, #1
	add	x15, x16, x15
	eor	x16, x2, x26, ror #34
	ror	x2, x7, #19
	add	x14, x15, x14
	eor	x15, x26, x9
	eor	x1, x1, x20, ror #8
	eor	x27, x16, x26, ror #39
	and	x16, x15, x3
	eor	x2, x2, x7, ror #61
	eor	x3, x16, x9
	add	x16, x14, x0
	eor	x0, x1, x20, lsr #7
	ror	x29, x16, #14
	add	x3, x27, x3
	eor	x1, x2, x7, lsr #6
	add	x2, x30, x25
	and	x27, x10, x16
	add	x0, x0, x2
	add	x4, x0, x1
	eor	x0, x29, x16, ror #18
	bic	x1, x11, x16
	ldp	x2, x29, [x28, #-16]
	orr	x1, x27, x1
	eor	x0, x0, x16, ror #41
	add	x27, x3, x14
	ror	x14, x21, #1
	ror	x3, x19, #19
	str	x4, [sp, #144]                  // 8-byte Folded Spill
	add	x12, x2, x12
	eor	x14, x14, x21, ror #8
	add	x12, x12, x4
	add	x12, x12, x1
	eor	x1, x27, x26
	add	x12, x12, x0
	ror	x0, x27, #28
	and	x15, x1, x15
	add	x2, x12, x8
	eor	x8, x14, x21, lsr #7
	eor	x14, x3, x19, ror #61
	eor	x0, x0, x27, ror #34
	ror	x3, x2, #14
	eor	x15, x15, x26
	eor	x14, x14, x19, lsr #6
	add	x8, x8, x20
	eor	x0, x0, x27, ror #39
	eor	x30, x3, x2, ror #18
	add	x8, x8, x13
	add	x20, x8, x14
	and	x14, x16, x2
	add	x15, x0, x15
	eor	x8, x30, x2, ror #41
	mov	x30, x17
	add	x3, x15, x12
	bic	x12, x10, x2
	add	x15, x29, x20
	ldr	x17, [sp, #112]                 // 8-byte Folded Reload
	orr	x12, x14, x12
	add	x11, x15, x11
	ror	x14, x3, #28
	add	x11, x11, x12
	eor	x15, x3, x27
	ror	x12, x17, #1
	add	x8, x11, x8
	stp	x30, x20, [sp, #128]            // 16-byte Folded Spill
	eor	x11, x14, x3, ror #34
	and	x14, x15, x1
	ror	x1, x4, #19
	eor	x0, x12, x17, ror #8
	add	x12, x8, x9
	eor	x14, x14, x27
	eor	x11, x11, x3, ror #39
	ror	x29, x20, #19
	eor	x9, x0, x17, lsr #7
	eor	x0, x1, x4, ror #61
	ror	x1, x12, #14
	add	x11, x11, x14
	eor	x0, x0, x4, lsr #6
	add	x9, x9, x21
	eor	x1, x1, x12, ror #18
	add	x14, x11, x8
	add	x8, x9, x30
	bic	x11, x16, x12
	add	x30, x8, x0
	eor	x8, x1, x12, ror #41
	and	x0, x2, x12
	ldp	x9, x1, [x28]
	ldr	x21, [sp, #104]                 // 8-byte Folded Reload
	orr	x11, x0, x11
	add	x9, x9, x30
	add	x9, x9, x10
	ror	x10, x14, #28
	add	x9, x9, x11
	ror	x11, x21, #1
	add	x8, x9, x8
	eor	x9, x10, x14, ror #34
	eor	x10, x14, x3
	eor	x11, x11, x21, ror #8
	and	x15, x10, x15
	add	x0, x8, x26
	eor	x9, x9, x14, ror #39
	eor	x26, x29, x20, ror #61
	eor	x15, x15, x3
	eor	x11, x11, x21, lsr #7
	ror	x29, x0, #14
	add	x9, x9, x15
	eor	x15, x26, x20, lsr #6
	add	x11, x11, x17
	add	x8, x9, x8
	eor	x26, x29, x0, ror #18
	add	x9, x11, x5
	ldr	x17, [sp, #96]                  // 8-byte Folded Reload
	bic	x11, x2, x0
	add	x5, x9, x15
	and	x15, x12, x0
	eor	x9, x26, x0, ror #41
	add	x1, x1, x5
	ror	x26, x17, #1
	orr	x11, x15, x11
	add	x15, x1, x16
	ror	x16, x8, #28
	ror	x1, x30, #19
	add	x11, x15, x11
	eor	x15, x26, x17, ror #8
	add	x9, x11, x9
	eor	x11, x16, x8, ror #34
	eor	x16, x8, x14
	and	x10, x16, x10
	eor	x15, x15, x17, lsr #7
	eor	x1, x1, x30, ror #61
	eor	x26, x11, x8, ror #39
	eor	x10, x10, x14
	add	x11, x9, x27
	eor	x1, x1, x30, lsr #6
	add	x15, x15, x21
	ror	x27, x11, #14
	add	x10, x26, x10
	ldp	x26, x29, [x28, #16]
	add	x15, x15, x6
	add	x6, x15, x1
	eor	x15, x27, x11, ror #18
	bic	x1, x12, x11
	and	x27, x0, x11
	add	x26, x26, x6
	eor	x15, x15, x11, ror #41
	orr	x1, x27, x1
	add	x26, x26, x2
	add	x2, x10, x9
	add	x9, x26, x1
	ror	x10, x22, #1
	add	x15, x9, x15
	ror	x9, x2, #28
	ror	x26, x5, #19
	eor	x10, x10, x22, ror #8
	eor	x1, x2, x8
	eor	x27, x9, x2, ror #34
	add	x9, x15, x3
	eor	x3, x26, x5, ror #61
	eor	x10, x10, x22, lsr #7
	and	x16, x1, x16
	ror	x26, x9, #14
	eor	x27, x27, x2, ror #39
	eor	x16, x16, x8
	eor	x3, x3, x5, lsr #6
	add	x10, x10, x17
	eor	x26, x26, x9, ror #18
	add	x16, x27, x16
	add	x21, x10, x7
	add	x10, x16, x15
	add	x7, x21, x3
	bic	x16, x0, x9
	and	x3, x11, x9
	eor	x15, x26, x9, ror #41
	ror	x26, x23, #1
	orr	x16, x3, x16
	add	x3, x29, x7
	ror	x27, x10, #28
	add	x12, x3, x12
	eor	x3, x26, x23, ror #8
	ror	x26, x6, #19
	add	x12, x12, x16
	eor	x16, x27, x10, ror #34
	eor	x27, x10, x2
	and	x1, x27, x1
	eor	x3, x3, x23, lsr #7
	add	x12, x12, x15
	eor	x15, x16, x10, ror #39
	eor	x26, x26, x6, ror #61
	eor	x1, x1, x2
	add	x16, x12, x14
	add	x14, x3, x22
	add	x15, x15, x1
	eor	x26, x26, x6, lsr #6
	add	x1, x14, x19
	add	x14, x15, x12
	ldp	x12, x29, [x28, #32]
	ror	x3, x16, #14
	add	x22, x1, x26
	bic	x1, x11, x16
	ror	x26, x14, #28
	add	x12, x12, x22
	eor	x15, x3, x16, ror #18
	and	x3, x9, x16
	add	x12, x12, x0
	ror	x0, x24, #1
	orr	x1, x3, x1
	eor	x15, x15, x16, ror #41
	add	x12, x12, x1
	eor	x1, x26, x14, ror #34
	eor	x0, x0, x24, ror #8
	ror	x26, x7, #19
	eor	x3, x14, x10
	add	x12, x12, x15
	eor	x15, x1, x14, ror #39
	and	x1, x3, x27
	eor	x0, x0, x24, lsr #7
	eor	x26, x26, x7, ror #61
	eor	x1, x1, x10
	add	x8, x12, x8
	add	x15, x15, x1
	eor	x1, x26, x7, lsr #6
	add	x0, x0, x23
	ror	x27, x8, #14
	add	x26, x15, x12
	add	x12, x0, x4
	bic	x15, x9, x8
	add	x23, x12, x1
	eor	x12, x27, x8, ror #18
	and	x0, x16, x8
	add	x1, x29, x23
	ror	x27, x26, #28
	orr	x15, x0, x15
	ror	x0, x25, #1
	add	x11, x1, x11
	eor	x12, x12, x8, ror #41
	add	x11, x11, x15
	eor	x15, x27, x26, ror #34
	ror	x29, x22, #19
	eor	x27, x0, x25, ror #8
	eor	x1, x26, x14
	add	x11, x11, x12
	eor	x12, x15, x26, ror #39
	and	x15, x1, x3
	add	x0, x11, x2
	eor	x2, x27, x25, lsr #7
	eor	x3, x29, x22, ror #61
	eor	x15, x15, x14
	add	x12, x12, x15
	ror	x27, x0, #14
	mov	x4, x19
	eor	x15, x3, x22, lsr #6
	add	x3, x2, x24
	add	x2, x12, x11
	add	x11, x3, x20
	eor	x12, x27, x0, ror #18
	bic	x3, x16, x0
	add	x24, x11, x15
	ldp	x15, x29, [x28, #48]
	ror	x11, x2, #28
	and	x27, x8, x0
	eor	x12, x12, x0, ror #41
	orr	x3, x27, x3
	ror	x27, x23, #19
	add	x28, x28, #128
	add	x15, x15, x24
	eor	x11, x11, x2, ror #34
	add	x9, x15, x9
	ror	x15, x13, #1
	add	x9, x9, x3
	eor	x3, x2, x26
	eor	x11, x11, x2, ror #39
	eor	x15, x15, x13, ror #8
	and	x1, x3, x1
	add	x12, x9, x12
	eor	x9, x1, x26
	eor	x1, x27, x23, ror #61
	eor	x15, x15, x13, lsr #7
	add	x11, x11, x9
	add	x9, x12, x10
	add	x12, x11, x12
	eor	x10, x1, x23, lsr #6
	add	x11, x15, x25
	ror	x15, x9, #14
	ror	x1, x12, #28
	add	x11, x11, x30
	add	x25, x11, x10
	eor	x10, x15, x9, ror #18
	bic	x11, x8, x9
	and	x15, x0, x9
	add	x27, x29, x25
	eor	x1, x1, x12, ror #34
	orr	x11, x15, x11
	add	x15, x27, x16
	eor	x16, x12, x2
	eor	x1, x1, x12, ror #39
	and	x3, x16, x3
	eor	x10, x10, x9, ror #41
	add	x11, x15, x11
	eor	x15, x3, x2
	add	x15, x1, x15
	ldr	x1, [sp, #120]                  // 8-byte Folded Reload
	add	x11, x11, x10
	add	x10, x11, x14
	add	x14, x15, x11
	subs	x1, x1, #128
	b.ne	.LBB0_3
// %bb.4:                               //   in Loop: Header=BB0_2 Depth=1
	ldr	x4, [sp, #40]                   // 8-byte Folded Reload
	ldp	x17, x27, [sp, #72]             // 16-byte Folded Reload
	ldp	x6, x5, [sp, #24]               // 16-byte Folded Reload
	ldr	x1, [sp, #64]                   // 8-byte Folded Reload
	add	x4, x4, x2
	ldr	x15, [sp, #88]                  // 8-byte Folded Reload
	ldp	x2, x3, [sp, #48]               // 16-byte Folded Reload
	add	x27, x27, #128
	add	x6, x6, x14
	add	x5, x5, x12
	add	x1, x1, x9
	add	x17, x17, x0
	add	x15, x15, x8
	add	x3, x3, x10
	ldr	x10, [sp, #16]                  // 8-byte Folded Reload
	add	x2, x2, x26
	cmp	x27, x10
	b.ne	.LBB0_2
// %bb.5:
	ldr	x8, [sp, #8]                    // 8-byte Folded Reload
	stp	x6, x5, [x8]
	stp	x4, x2, [x8, #16]
	stp	x3, x1, [x8, #32]
	stp	x17, x15, [x8, #48]
.LBB0_6:
	.seh_startepilogue
	ldp	x29, x30, [sp, #256]            // 16-byte Folded Reload
	.seh_save_fplr	256
	ldp	x27, x28, [sp, #240]            // 16-byte Folded Reload
	.seh_save_regp	x27, 240
	ldp	x25, x26, [sp, #224]            // 16-byte Folded Reload
	.seh_save_regp	x25, 224
	ldp	x23, x24, [sp, #208]            // 16-byte Folded Reload
	.seh_save_regp	x23, 208
	ldp	x21, x22, [sp, #192]            // 16-byte Folded Reload
	.seh_save_regp	x21, 192
	ldp	x19, x20, [sp, #176]            // 16-byte Folded Reload
	.seh_save_regp	x19, 176
	add	sp, sp, #272
	.seh_stackalloc	272
	.seh_endepilogue
	ret
	.seh_endfunclet
	.seh_endproc
                                        // -- End function
	.section	.rdata,"dr",discard,"?k512scalar@sha512@details@fast_io@@3U?$array@_K$0EA@@freestanding@3@B"
	.globl	"?k512scalar@sha512@details@fast_io@@3U?$array@_K$0EA@@freestanding@3@B" // @"?k512scalar@sha512@details@fast_io@@3U?$array@_K$0EA@@freestanding@3@B"
	.p2align	4, 0x0
"?k512scalar@sha512@details@fast_io@@3U?$array@_K$0EA@@freestanding@3@B":
	.xword	-1973867731355612462            // 0xe49b69c19ef14ad2
	.xword	-1171420211273849373            // 0xefbe4786384f25e3
	.xword	1135362057144423861             // 0xfc19dc68b8cd5b5
	.xword	2597628984639134821             // 0x240ca1cc77ac9c65
	.xword	3308224258029322869             // 0x2de92c6f592b0275
	.xword	5365058923640841347             // 0x4a7484aa6ea6e483
	.xword	6679025012923562964             // 0x5cb0a9dcbd41fbd4
	.xword	8573033837759648693             // 0x76f988da831153b5
	.xword	-7476448914759557205            // 0x983e5152ee66dfab
	.xword	-6327057829258317296            // 0xa831c66d2db43210
	.xword	-5763719355590565569            // 0xb00327c898fb213f
	.xword	-4658551843659510044            // 0xbf597fc7beef0ee4
	.xword	-4116276920077217854            // 0xc6e00bf33da88fc2
	.xword	-3051310485924567259            // 0xd5a79147930aa725
	.xword	489312712824947311              // 0x6ca6351e003826f
	.xword	1452737877330783856             // 0x142929670a0e6e70
	.xword	2861767655752347644             // 0x27b70a8546d22ffc
	.xword	3322285676063803686             // 0x2e1b21385c26c926
	.xword	5560940570517711597             // 0x4d2c6dfc5ac42aed
	.xword	5996557281743188959             // 0x53380d139d95b3df
	.xword	7280758554555802590             // 0x650a73548baf63de
	.xword	8532644243296465576             // 0x766a0abb3c77b2a8
	.xword	-9096487096722542874            // 0x81c2c92e47edaee6
	.xword	-7894198246740708037            // 0x92722c851482353b
	.xword	-6719396339535248540            // 0xa2bfe8a14cf10364
	.xword	-6333637450476146687            // 0xa81a664bbc423001
	.xword	-4446306890439682159            // 0xc24b8b70d0f89791
	.xword	-4076793802049405392            // 0xc76c51a30654be30
	.xword	-3345356375505022440            // 0xd192e819d6ef5218
	.xword	-2983346525034927856            // 0xd69906245565a910
	.xword	-860691631967231958             // 0xf40e35855771202a
	.xword	1182934255886127544             // 0x106aa07032bbd1b8
	.xword	1847814050463011016             // 0x19a4c116b8d2d0c8
	.xword	2177327727835720531             // 0x1e376c085141ab53
	.xword	2830643537854262169             // 0x2748774cdf8eeb99
	.xword	3796741975233480872             // 0x34b0bcb5e19b48a8
	.xword	4115178125766777443             // 0x391c0cb3c5c95a63
	.xword	5681478168544905931             // 0x4ed8aa4ae3418acb
	.xword	6601373596472566643             // 0x5b9cca4f7763e373
	.xword	7507060721942968483             // 0x682e6ff3d6b2b8a3
	.xword	8399075790359081724             // 0x748f82ee5defb2fc
	.xword	8693463985226723168             // 0x78a5636f43172f60
	.xword	-8878714635349349518            // 0x84c87814a1f0ab72
	.xword	-8302665154208450068            // 0x8cc702081a6439ec
	.xword	-8016688836872298968            // 0x90befffa23631e28
	.xword	-6606660893046293015            // 0xa4506cebde82bde9
	.xword	-4685533653050689259            // 0xbef9a3f7b2c67915
	.xword	-4147400797238176981            // 0xc67178f2e372532b
	.xword	-3880063495543823972            // 0xca273eceea26619c
	.xword	-3348786107499101689            // 0xd186b8c721c0c207
	.xword	-1523767162380948706            // 0xeada7dd6cde0eb1e
	.xword	-757361751448694408             // 0xf57d4f7fee6ed178
	.xword	500013540394364858              // 0x6f067aa72176fba
	.xword	748580250866718886              // 0xa637dc5a2c898a6
	.xword	1242879168328830382             // 0x113f9804bef90dae
	.xword	1977374033974150939             // 0x1b710b35131c471b
	.xword	2944078676154940804             // 0x28db77f523047d84
	.xword	3659926193048069267             // 0x32caab7b40c72493
	.xword	4368137639120453308             // 0x3c9ebe0a15c9bebc
	.xword	4836135668995329356             // 0x431d67c49c100d4c
	.xword	5532061633213252278             // 0x4cc5d4becb3e42b6
	.xword	6448918945643986474             // 0x597f299cfc657e2a
	.xword	6902733635092675308             // 0x5fcb6fab3ad6faec
	.xword	7801388544844847127             // 0x6c44198c4a475817

	.section	.drectve,"yni"
	.ascii	" /FAILIFMISMATCH:\"_CRT_STDIO_ISO_WIDE_SPECIFIERS=0\""
	.ascii	" /FAILIFMISMATCH:\"_MSC_VER=1900\""
	.ascii	" /FAILIFMISMATCH:\"_ITERATOR_DEBUG_LEVEL=0\""
	.ascii	" /FAILIFMISMATCH:\"RuntimeLibrary=MT_StaticRelease\""
	.ascii	" /DEFAULTLIB:libcpmt.lib"
	.ascii	" /DEFAULTLIB:ntdll.lib"
	.addrsig
