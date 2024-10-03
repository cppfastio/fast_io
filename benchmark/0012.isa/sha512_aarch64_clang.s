	.text
	.file	"sha512.cc"
	.globl	_Z14sha512_invokerPmPKSt4byteS2_ // -- Begin function _Z14sha512_invokerPmPKSt4byteS2_
	.p2align	2
	.type	_Z14sha512_invokerPmPKSt4byteS2_,@function
_Z14sha512_invokerPmPKSt4byteS2_:       // @_Z14sha512_invokerPmPKSt4byteS2_
// %bb.0:
	sub	sp, sp, #256
	cmp	x1, x2
	stp	x29, x30, [sp, #160]            // 16-byte Folded Spill
	stp	x28, x27, [sp, #176]            // 16-byte Folded Spill
	stp	x26, x25, [sp, #192]            // 16-byte Folded Spill
	stp	x24, x23, [sp, #208]            // 16-byte Folded Spill
	stp	x22, x21, [sp, #224]            // 16-byte Folded Spill
	stp	x20, x19, [sp, #240]            // 16-byte Folded Spill
	str	x2, [sp, #16]                   // 8-byte Folded Spill
	b.eq	.LBB0_6
// %bb.1:
	ldp	x16, x11, [x0, #48]
	str	x0, [sp, #8]                    // 8-byte Folded Spill
	ldp	x3, x2, [x0, #32]
	ldp	x18, x17, [x0, #16]
	ldp	x5, x4, [x0]
.LBB0_2:                                // =>This Loop Header: Depth=1
                                        //     Child Loop BB0_3 Depth 2
	bic	x9, x16, x3
	and	x10, x2, x3
	ror	x8, x3, #14
	ror	x12, x5, #28
	ldp	x14, x15, [x1]
	orr	x9, x10, x9
	str	x11, [sp, #88]                  // 8-byte Folded Spill
	eor	x8, x8, x3, ror #18
	add	x9, x11, x9
	mov	x11, #44578                     // =0xae22
	eor	x10, x12, x5, ror #34
	movk	x11, #55080, lsl #16
	eor	x13, x4, x5
	rev	x30, x14
	movk	x11, #12184, lsl #32
	bic	x12, x4, x13
	add	x9, x9, x30
	eor	x8, x8, x3, ror #41
	and	x14, x18, x13
	movk	x11, #17034, lsl #48
	eor	x10, x10, x5, ror #39
	add	x9, x9, x11
	orr	x12, x14, x12
	add	x8, x8, x9
	mov	x11, #26061                     // =0x65cd
	rev	x27, x15
	add	x9, x12, x10
	add	x14, x8, x17
	movk	x11, #9199, lsl #16
	add	x8, x9, x8
	ror	x9, x14, #14
	bic	x12, x2, x14
	ror	x10, x8, #28
	and	x15, x3, x14
	str	x16, [sp, #72]                  // 8-byte Folded Spill
	eor	x9, x9, x14, ror #18
	add	x16, x16, x27
	orr	x12, x15, x12
	eor	x10, x10, x8, ror #34
	eor	x15, x8, x5
	movk	x11, #17553, lsl #32
	add	x12, x16, x12
	eor	x9, x9, x14, ror #41
	and	x13, x15, x13
	movk	x11, #28983, lsl #48
	eor	x10, x10, x8, ror #39
	eor	x13, x13, x5
	add	x12, x12, x11
	stp	x18, x17, [sp, #40]             // 16-byte Folded Spill
	add	x9, x12, x9
	add	x10, x10, x13
	ldp	x12, x13, [x1, #16]
	add	x18, x9, x18
	add	x10, x10, x9
	ror	x9, x18, #14
	bic	x16, x3, x18
	and	x17, x14, x18
	str	x2, [sp, #64]                   // 8-byte Folded Spill
	rev	x11, x12
	ror	x12, x10, #28
	eor	x9, x9, x18, ror #18
	str	x11, [sp, #144]                 // 8-byte Folded Spill
	add	x0, x2, x11
	mov	x11, #15151                     // =0x3b2f
	eor	x12, x12, x10, ror #34
	movk	x11, #60493, lsl #16
	orr	x16, x17, x16
	eor	x17, x10, x8
	movk	x11, #64463, lsl #32
	add	x16, x0, x16
	eor	x9, x9, x18, ror #41
	and	x15, x17, x15
	movk	x11, #46528, lsl #48
	eor	x12, x12, x10, ror #39
	add	x16, x16, x11
	eor	x15, x15, x8
	add	x9, x16, x9
	rev	x11, x13
	str	x3, [sp, #56]                   // 8-byte Folded Spill
	add	x12, x12, x15
	add	x16, x9, x4
	str	x11, [sp, #128]                 // 8-byte Folded Spill
	add	x9, x12, x9
	ror	x12, x16, #14
	bic	x0, x14, x16
	ror	x15, x9, #28
	and	x2, x18, x16
	add	x3, x3, x11
	mov	x11, #56252                     // =0xdbbc
	eor	x12, x12, x16, ror #18
	orr	x0, x2, x0
	eor	x15, x15, x9, ror #34
	eor	x2, x9, x10
	movk	x11, #33161, lsl #16
	and	x17, x2, x17
	movk	x11, #56229, lsl #32
	add	x0, x3, x0
	eor	x15, x15, x9, ror #39
	eor	x12, x12, x16, ror #41
	movk	x11, #59829, lsl #48
	eor	x17, x17, x10
	stp	x5, x4, [sp, #24]               // 16-byte Folded Spill
	add	x0, x0, x11
	add	x15, x15, x17
	ldp	x17, x4, [x1, #32]
	add	x12, x0, x12
	mov	x11, #46392                     // =0xb538
	add	x0, x12, x5
	add	x12, x15, x12
	movk	x11, #62280, lsl #16
	ror	x3, x0, #14
	rev	x15, x17
	ror	x17, x12, #28
	bic	x5, x18, x0
	and	x6, x16, x0
	add	x14, x14, x15
	eor	x3, x3, x0, ror #18
	eor	x17, x17, x12, ror #34
	orr	x5, x6, x5
	eor	x6, x12, x9
	movk	x11, #49755, lsl #32
	add	x14, x14, x5
	eor	x3, x3, x0, ror #41
	and	x2, x6, x2
	movk	x11, #14678, lsl #48
	eor	x17, x17, x12, ror #39
	add	x14, x14, x11
	eor	x2, x2, x9
	add	x3, x14, x3
	mov	x11, #53273                     // =0xd019
	str	x1, [sp, #80]                   // 8-byte Folded Spill
	add	x17, x17, x2
	add	x14, x3, x8
	movk	x11, #46597, lsl #16
	add	x8, x17, x3
	ror	x2, x14, #14
	rev	x17, x4
	ror	x3, x8, #28
	bic	x4, x16, x14
	and	x5, x0, x14
	eor	x2, x2, x14, ror #18
	add	x18, x17, x18
	orr	x4, x5, x4
	eor	x3, x3, x8, ror #34
	eor	x5, x8, x12
	add	x18, x18, x4
	and	x4, x5, x6
	movk	x11, #4593, lsl #32
	eor	x2, x2, x14, ror #41
	eor	x3, x3, x8, ror #39
	movk	x11, #23025, lsl #48
	eor	x4, x4, x12
	add	x18, x18, x11
	mov	x11, #20379                     // =0x4f9b
	add	x3, x3, x4
	ldp	x4, x6, [x1, #48]
	add	x2, x18, x2
	movk	x11, #44825, lsl #16
	add	x18, x2, x10
	add	x10, x3, x2
	movk	x11, #33444, lsl #32
	ror	x2, x18, #14
	rev	x3, x4
	ror	x4, x10, #28
	bic	x7, x0, x18
	and	x19, x14, x18
	add	x16, x3, x16
	eor	x2, x2, x18, ror #18
	eor	x4, x4, x10, ror #34
	orr	x7, x19, x7
	eor	x19, x10, x8
	add	x16, x16, x7
	movk	x11, #37439, lsl #48
	eor	x2, x2, x18, ror #41
	and	x5, x19, x5
	eor	x4, x4, x10, ror #39
	add	x16, x16, x11
	eor	x5, x5, x8
	rev	x11, x6
	add	x2, x16, x2
	add	x4, x4, x5
	str	x11, [sp, #136]                 // 8-byte Folded Spill
	add	x16, x2, x9
	add	x9, x4, x2
	add	x0, x11, x0
	ror	x2, x16, #14
	ror	x5, x9, #28
	mov	x11, #33048                     // =0x8118
	bic	x6, x14, x16
	and	x7, x18, x16
	movk	x11, #55917, lsl #16
	eor	x2, x2, x16, ror #18
	eor	x5, x5, x9, ror #34
	orr	x6, x7, x6
	eor	x7, x9, x10
	movk	x11, #24277, lsl #32
	add	x0, x0, x6
	eor	x2, x2, x16, ror #41
	and	x6, x7, x19
	movk	x11, #43804, lsl #48
	eor	x5, x5, x9, ror #39
	add	x0, x0, x11
	eor	x6, x6, x10
	add	x0, x0, x2
	mov	x11, #578                       // =0x242
	add	x2, x5, x6
	add	x20, x0, x12
	movk	x11, #41731, lsl #16
	add	x12, x2, x0
	ldp	x5, x2, [x1, #64]
	ror	x0, x20, #14
	ror	x6, x12, #28
	bic	x19, x18, x20
	and	x21, x16, x20
	movk	x11, #43672, lsl #32
	rev	x4, x5
	eor	x0, x0, x20, ror #18
	eor	x6, x6, x12, ror #34
	add	x14, x4, x14
	orr	x19, x21, x19
	eor	x21, x12, x9
	add	x14, x14, x19
	eor	x0, x0, x20, ror #41
	and	x7, x21, x7
	movk	x11, #55303, lsl #48
	eor	x6, x6, x12, ror #39
	eor	x7, x7, x9
	add	x14, x14, x11
	rev	x5, x2
	mov	x11, #28606                     // =0x6fbe
	add	x14, x14, x0
	add	x6, x6, x7
	movk	x11, #17776, lsl #16
	add	x0, x14, x8
	add	x8, x6, x14
	add	x18, x5, x18
	ror	x14, x0, #14
	ror	x2, x8, #28
	bic	x7, x16, x0
	and	x19, x20, x0
	movk	x11, #23297, lsl #32
	eor	x14, x14, x0, ror #18
	eor	x2, x2, x8, ror #34
	orr	x7, x19, x7
	eor	x19, x8, x12
	add	x18, x18, x7
	movk	x11, #4739, lsl #48
	eor	x14, x14, x0, ror #41
	and	x7, x19, x21
	eor	x2, x2, x8, ror #39
	add	x18, x18, x11
	eor	x7, x7, x12
	mov	x11, #45708                     // =0xb28c
	add	x14, x18, x14
	add	x18, x2, x7
	movk	x11, #20196, lsl #16
	add	x2, x14, x10
	add	x10, x18, x14
	movk	x11, #34238, lsl #32
	ldp	x7, x18, [x1, #80]
	ror	x14, x2, #14
	ror	x21, x10, #28
	bic	x22, x20, x2
	and	x23, x0, x2
	eor	x14, x14, x2, ror #18
	orr	x22, x23, x22
	eor	x23, x10, x8
	rev	x13, x7
	eor	x21, x21, x10, ror #34
	movk	x11, #9265, lsl #48
	add	x16, x13, x16
	eor	x14, x14, x2, ror #41
	and	x19, x23, x19
	add	x16, x16, x22
	eor	x21, x21, x10, ror #39
	eor	x19, x19, x8
	add	x16, x16, x11
	rev	x7, x18
	mov	x11, #46306                     // =0xb4e2
	add	x14, x16, x14
	add	x19, x21, x19
	movk	x11, #54783, lsl #16
	add	x16, x14, x9
	add	x9, x19, x14
	add	x20, x7, x20
	ror	x14, x16, #14
	ror	x18, x9, #28
	bic	x21, x0, x16
	and	x22, x2, x16
	movk	x11, #32195, lsl #32
	mov	x19, x13
	eor	x14, x14, x16, ror #18
	orr	x21, x22, x21
	eor	x18, x18, x9, ror #34
	eor	x22, x9, x10
	add	x20, x20, x21
	movk	x11, #21772, lsl #48
	eor	x14, x14, x16, ror #41
	and	x21, x22, x23
	add	x20, x20, x11
	eor	x18, x18, x9, ror #39
	eor	x21, x21, x10
	mov	x11, #35183                     // =0x896f
	add	x14, x20, x14
	movk	x11, #62075, lsl #16
	mov	x13, x27
	add	x20, x18, x21
	add	x18, x14, x12
	movk	x11, #23924, lsl #32
	ldp	x12, x23, [x1, #96]
	ror	x21, x18, #14
	add	x14, x20, x14
	bic	x24, x2, x18
	and	x25, x16, x18
	eor	x21, x21, x18, ror #18
	orr	x24, x25, x24
	movk	x11, #29374, lsl #48
	rev	x20, x12
	ror	x12, x14, #28
	eor	x25, x14, x9
	add	x0, x20, x0
	eor	x21, x21, x18, ror #41
	and	x22, x25, x22
	eor	x12, x12, x14, ror #34
	add	x0, x0, x24
	eor	x22, x22, x9
	add	x0, x0, x11
	mov	x11, #38577                     // =0x96b1
	eor	x12, x12, x14, ror #39
	add	x0, x0, x21
	rev	x21, x23
	add	x24, x0, x8
	movk	x11, #15126, lsl #16
	add	x2, x21, x2
	add	x12, x12, x22
	ror	x8, x24, #14
	and	x22, x18, x24
	add	x26, x12, x0
	bic	x0, x16, x24
	movk	x11, #45566, lsl #32
	ror	x12, x26, #28
	eor	x8, x8, x24, ror #18
	orr	x0, x22, x0
	add	x0, x2, x0
	movk	x11, #32990, lsl #48
	eor	x23, x26, x14
	eor	x12, x12, x26, ror #34
	eor	x8, x8, x24, ror #41
	add	x0, x0, x11
	and	x2, x23, x25
	mov	x11, #4661                      // =0x1235
	eor	x12, x12, x26, ror #39
	add	x0, x0, x8
	eor	x2, x2, x14
	add	x8, x0, x10
	ldp	x10, x25, [x1, #112]
	add	x12, x12, x2
	bic	x2, x18, x8
	and	x27, x24, x8
	add	x0, x12, x0
	ror	x12, x8, #14
	movk	x11, #9671, lsl #16
	rev	x6, x10
	ror	x10, x0, #28
	orr	x2, x27, x2
	eor	x12, x12, x8, ror #18
	add	x16, x6, x16
	movk	x11, #1703, lsl #32
	eor	x10, x10, x0, ror #34
	eor	x27, x0, x26
	add	x16, x16, x2
	eor	x12, x12, x8, ror #41
	movk	x11, #39900, lsl #48
	and	x2, x27, x23
	add	x16, x16, x11
	eor	x10, x10, x0, ror #39
	eor	x2, x2, x26
	add	x16, x16, x12
	mov	x11, #9876                      // =0x2694
	rev	x23, x25
	add	x10, x10, x2
	add	x12, x16, x9
	movk	x11, #53097, lsl #16
	add	x2, x10, x16
	ror	x9, x12, #14
	bic	x16, x24, x12
	ror	x10, x2, #28
	and	x25, x8, x12
	add	x18, x23, x18
	eor	x9, x9, x12, ror #18
	orr	x16, x25, x16
	movk	x11, #61812, lsl #32
	eor	x25, x10, x2, ror #34
	add	x16, x18, x16
	eor	x10, x2, x0
	eor	x9, x9, x12, ror #41
	movk	x11, #49563, lsl #48
	and	x18, x10, x27
	add	x16, x16, x11
	eor	x25, x25, x2, ror #39
	adrp	x27, _ZN7fast_io7details6sha51210k512scalarE+64
	add	x27, x27, :lo12:_ZN7fast_io7details6sha51210k512scalarE+64
	add	x9, x16, x9
	eor	x16, x18, x0
	add	x16, x25, x16
	add	x14, x9, x14
	add	x9, x16, x9
	mov	w16, #512                       // =0x200
.LBB0_3:                                //   Parent Loop BB0_2 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	str	x16, [sp, #120]                 // 8-byte Folded Spill
	ror	x16, x13, #1
	ror	x18, x6, #19
	ror	x11, x14, #14
	bic	x25, x8, x14
	eor	x16, x16, x13, ror #8
	and	x1, x12, x14
	eor	x18, x18, x6, ror #61
	eor	x11, x11, x14, ror #18
	str	x5, [sp, #96]                   // 8-byte Folded Spill
	add	x29, x30, x5
	eor	x16, x16, x13, lsr #7
	ror	x28, x9, #28
	ldr	x5, [sp, #144]                  // 8-byte Folded Reload
	orr	x1, x1, x25
	eor	x11, x11, x14, ror #41
	eor	x18, x18, x6, lsr #6
	add	x1, x24, x1
	add	x16, x29, x16
	ror	x30, x5, #1
	ldp	x24, x29, [x27, #-64]
	eor	x28, x28, x9, ror #34
	ror	x25, x23, #19
	add	x11, x1, x11
	mov	x22, x4
	add	x4, x16, x18
	eor	x16, x2, x9
	eor	x18, x30, x5, ror #8
	add	x11, x11, x24
	eor	x28, x28, x9, ror #39
	and	x10, x10, x16
	add	x11, x11, x4
	eor	x25, x25, x23, ror #61
	eor	x10, x10, x2
	add	x24, x11, x26
	eor	x18, x18, x5, lsr #7
	add	x10, x10, x28
	eor	x25, x25, x23, lsr #6
	add	x26, x13, x19
	ror	x30, x24, #14
	add	x10, x10, x11
	str	x6, [sp, #152]                  // 8-byte Folded Spill
	mov	x6, x3
	mov	x3, x17
	mov	x17, x15
	ldr	x15, [sp, #128]                 // 8-byte Folded Reload
	add	x18, x26, x18
	ror	x26, x10, #28
	stp	x7, x20, [sp, #104]             // 16-byte Folded Spill
	mov	x13, x20
	add	x20, x18, x25
	eor	x11, x30, x24, ror #18
	bic	x18, x12, x24
	and	x25, x14, x24
	add	x8, x8, x29
	orr	x18, x25, x18
	add	x8, x20, x8
	eor	x25, x26, x10, ror #34
	ror	x26, x15, #1
	eor	x11, x11, x24, ror #41
	add	x8, x8, x18
	eor	x18, x10, x9
	ror	x28, x4, #19
	eor	x25, x25, x10, ror #39
	and	x16, x18, x16
	eor	x29, x26, x15, ror #8
	add	x8, x8, x11
	eor	x11, x16, x9
	eor	x16, x28, x4, ror #61
	add	x28, x5, x7
	add	x11, x25, x11
	eor	x25, x29, x15, lsr #7
	add	x26, x8, x0
	eor	x16, x16, x4, lsr #6
	add	x0, x11, x8
	bic	x11, x14, x26
	add	x8, x28, x25
	ror	x29, x26, #14
	ror	x25, x0, #28
	add	x5, x8, x16
	and	x16, x24, x26
	ror	x30, x20, #19
	orr	x11, x16, x11
	ldp	x28, x16, [x27, #-48]
	eor	x8, x29, x26, ror #18
	eor	x25, x25, x0, ror #34
	eor	x29, x0, x10
	and	x18, x29, x18
	add	x13, x15, x13
	mov	x1, x19
	add	x12, x12, x28
	ror	x28, x17, #1
	eor	x8, x8, x26, ror #41
	add	x12, x5, x12
	eor	x25, x25, x0, ror #39
	add	x14, x14, x16
	eor	x28, x28, x17, ror #8
	add	x11, x12, x11
	eor	x12, x18, x10
	eor	x18, x30, x20, ror #61
	add	x8, x11, x8
	add	x11, x25, x12
	eor	x25, x28, x17, lsr #7
	add	x12, x8, x2
	add	x8, x11, x8
	eor	x18, x18, x20, lsr #6
	ror	x2, x12, #14
	ror	x16, x8, #28
	add	x11, x13, x25
	eor	x25, x8, x0
	ldr	x13, [sp, #152]                 // 8-byte Folded Reload
	add	x19, x11, x18
	bic	x11, x24, x12
	and	x18, x26, x12
	eor	x2, x2, x12, ror #18
	orr	x11, x18, x11
	add	x14, x19, x14
	ror	x18, x3, #1
	add	x11, x14, x11
	ror	x14, x5, #19
	eor	x2, x2, x12, ror #41
	eor	x16, x16, x8, ror #34
	ldr	x7, [sp, #96]                   // 8-byte Folded Reload
	eor	x18, x18, x3, ror #8
	eor	x14, x14, x5, ror #61
	str	x5, [sp, #144]                  // 8-byte Folded Spill
	add	x11, x11, x2
	and	x2, x25, x29
	eor	x28, x16, x8, ror #39
	eor	x18, x18, x3, lsr #7
	eor	x2, x2, x0
	add	x16, x11, x9
	eor	x9, x14, x5, lsr #6
	add	x14, x17, x21
	add	x15, x28, x2
	add	x18, x14, x18
	ror	x2, x16, #14
	add	x14, x15, x11
	add	x15, x18, x9
	ldp	x9, x28, [x27, #-32]
	eor	x11, x2, x16, ror #18
	bic	x18, x26, x16
	and	x2, x12, x16
	orr	x18, x2, x18
	eor	x2, x14, x8
	add	x17, x3, x13
	add	x9, x24, x9
	eor	x11, x11, x16, ror #41
	ror	x24, x14, #28
	add	x9, x9, x15
	and	x25, x2, x25
	ldr	x13, [sp, #136]                 // 8-byte Folded Reload
	add	x9, x9, x18
	eor	x18, x24, x14, ror #34
	ror	x24, x19, #19
	add	x9, x9, x11
	ror	x11, x6, #1
	eor	x25, x25, x8
	add	x10, x9, x10
	eor	x18, x18, x14, ror #39
	eor	x24, x24, x19, ror #61
	eor	x11, x11, x6, ror #8
	ror	x29, x10, #14
	add	x3, x6, x23
	add	x18, x18, x25
	eor	x24, x24, x19, lsr #6
	str	x19, [sp, #128]                 // 8-byte Folded Spill
	eor	x11, x11, x6, lsr #7
	eor	x25, x29, x10, ror #18
	add	x9, x18, x9
	bic	x18, x12, x10
	add	x11, x11, x17
	add	x17, x11, x24
	eor	x11, x25, x10, ror #41
	and	x24, x16, x10
	add	x25, x28, x26
	ror	x26, x9, #28
	orr	x18, x24, x18
	add	x24, x25, x17
	ror	x25, x13, #1
	eor	x28, x9, x14
	add	x18, x24, x18
	eor	x24, x26, x9, ror #34
	ror	x26, x15, #19
	add	x11, x18, x11
	eor	x18, x25, x13, ror #8
	and	x2, x28, x2
	eor	x24, x24, x9, ror #39
	eor	x25, x26, x15, ror #61
	eor	x2, x2, x14
	add	x26, x11, x0
	eor	x18, x18, x13, lsr #7
	add	x2, x24, x2
	ldp	x24, x29, [x27, #-16]
	eor	x0, x25, x15, lsr #6
	ror	x25, x26, #14
	add	x18, x18, x3
	add	x3, x18, x0
	eor	x18, x25, x26, ror #18
	bic	x0, x16, x26
	and	x25, x10, x26
	add	x12, x24, x12
	orr	x0, x25, x0
	add	x12, x12, x3
	eor	x24, x18, x26, ror #41
	add	x18, x2, x11
	add	x11, x12, x0
	ror	x12, x22, #1
	add	x11, x11, x24
	ror	x0, x18, #28
	ror	x25, x17, #19
	eor	x24, x12, x22, ror #8
	add	x12, x11, x8
	eor	x2, x18, x9
	eor	x0, x0, x18, ror #34
	and	x28, x2, x28
	eor	x8, x24, x22, lsr #7
	eor	x24, x25, x17, ror #61
	ror	x25, x12, #14
	eor	x0, x0, x18, ror #39
	eor	x28, x28, x9
	eor	x30, x24, x17, lsr #6
	add	x8, x8, x13
	eor	x25, x25, x12, ror #18
	add	x8, x8, x4
	add	x0, x0, x28
	mov	x13, x20
	add	x6, x8, x30
	add	x24, x0, x11
	eor	x8, x25, x12, ror #41
	bic	x11, x10, x12
	and	x0, x26, x12
	add	x25, x29, x6
	orr	x11, x0, x11
	add	x16, x25, x16
	ror	x0, x24, #28
	add	x11, x16, x11
	ror	x16, x7, #1
	eor	x25, x24, x18
	add	x8, x11, x8
	eor	x11, x0, x24, ror #34
	and	x0, x25, x2
	eor	x16, x16, x7, ror #8
	ror	x2, x3, #19
	add	x14, x8, x14
	eor	x11, x11, x24, ror #39
	ror	x28, x14, #14
	eor	x0, x0, x18
	eor	x16, x16, x7, lsr #7
	eor	x2, x2, x3, ror #61
	mov	x30, x4
	add	x11, x11, x0
	eor	x28, x28, x14, ror #18
	str	x6, [sp, #136]                  // 8-byte Folded Spill
	eor	x2, x2, x3, lsr #6
	add	x16, x16, x22
	add	x0, x11, x8
	add	x8, x16, x20
	bic	x16, x26, x14
	ldr	x20, [sp, #104]                 // 8-byte Folded Reload
	add	x4, x8, x2
	eor	x8, x28, x14, ror #41
	and	x2, x12, x14
	ldp	x11, x28, [x27]
	orr	x16, x2, x16
	add	x11, x11, x4
	add	x10, x11, x10
	ror	x11, x0, #28
	add	x10, x10, x16
	ror	x16, x1, #1
	add	x8, x10, x8
	eor	x10, x11, x0, ror #34
	eor	x11, x0, x24
	and	x2, x11, x25
	eor	x16, x16, x1, ror #8
	ror	x25, x6, #19
	eor	x29, x2, x24
	add	x2, x8, x9
	eor	x10, x10, x0, ror #39
	eor	x9, x16, x1, lsr #7
	eor	x16, x25, x6, ror #61
	ror	x25, x2, #14
	add	x10, x10, x29
	eor	x16, x16, x6, lsr #6
	add	x9, x9, x7
	eor	x25, x25, x2, ror #18
	add	x9, x9, x5
	add	x8, x10, x8
	bic	x10, x12, x2
	add	x5, x9, x16
	eor	x9, x25, x2, ror #41
	and	x16, x14, x2
	add	x25, x28, x5
	ror	x28, x20, #1
	orr	x10, x16, x10
	add	x16, x25, x26
	ror	x25, x8, #28
	ror	x26, x4, #19
	add	x10, x16, x10
	eor	x16, x28, x20, ror #8
	add	x9, x10, x9
	eor	x10, x25, x8, ror #34
	eor	x25, x8, x0
	eor	x28, x16, x20, lsr #7
	eor	x26, x26, x4, ror #61
	and	x11, x25, x11
	eor	x10, x10, x8, ror #39
	add	x16, x9, x18
	eor	x11, x11, x0
	eor	x18, x26, x4, lsr #6
	add	x7, x28, x1
	ror	x26, x16, #14
	ldp	x28, x29, [x27, #16]
	add	x10, x10, x11
	add	x11, x7, x19
	ldr	x1, [sp, #112]                  // 8-byte Folded Reload
	add	x19, x11, x18
	eor	x11, x26, x16, ror #18
	bic	x18, x14, x16
	and	x26, x2, x16
	add	x28, x28, x19
	eor	x11, x11, x16, ror #41
	orr	x26, x26, x18
	add	x12, x28, x12
	add	x18, x10, x9
	add	x9, x12, x26
	ror	x10, x1, #1
	add	x9, x9, x11
	ror	x11, x18, #28
	ror	x28, x5, #19
	eor	x10, x10, x1, ror #8
	eor	x26, x18, x8
	add	x12, x9, x24
	eor	x11, x11, x18, ror #34
	eor	x24, x28, x5, ror #61
	and	x25, x26, x25
	eor	x10, x10, x1, lsr #7
	ror	x28, x12, #14
	eor	x25, x25, x8
	eor	x11, x11, x18, ror #39
	eor	x24, x24, x5, lsr #6
	add	x10, x10, x20
	eor	x28, x28, x12, ror #18
	add	x11, x11, x25
	add	x10, x10, x15
	ror	x25, x21, #1
	add	x9, x11, x9
	add	x7, x10, x24
	bic	x11, x2, x12
	and	x24, x16, x12
	eor	x10, x28, x12, ror #41
	ror	x28, x9, #28
	orr	x11, x24, x11
	add	x24, x29, x7
	eor	x25, x25, x21, ror #8
	add	x14, x24, x14
	ror	x29, x19, #19
	eor	x24, x9, x18
	add	x11, x14, x11
	eor	x14, x28, x9, ror #34
	and	x26, x24, x26
	eor	x25, x25, x21, lsr #7
	eor	x28, x29, x19, ror #61
	eor	x26, x26, x18
	eor	x14, x14, x9, ror #39
	add	x11, x11, x10
	add	x10, x11, x0
	eor	x0, x28, x19, lsr #6
	add	x20, x25, x1
	add	x14, x14, x26
	add	x20, x20, x17
	ror	x25, x10, #14
	add	x14, x14, x11
	ldp	x11, x28, [x27, #32]
	add	x20, x20, x0
	ldr	x1, [sp, #152]                  // 8-byte Folded Reload
	eor	x0, x25, x10, ror #18
	bic	x25, x16, x10
	and	x26, x12, x10
	ror	x29, x14, #28
	add	x11, x11, x20
	orr	x25, x26, x25
	eor	x0, x0, x10, ror #41
	add	x11, x11, x2
	ror	x2, x1, #1
	ror	x26, x7, #19
	add	x11, x11, x25
	eor	x25, x29, x14, ror #34
	eor	x29, x14, x9
	eor	x2, x2, x1, ror #8
	add	x11, x11, x0
	and	x24, x29, x24
	eor	x0, x25, x14, ror #39
	eor	x24, x24, x9
	eor	x25, x26, x7, ror #61
	eor	x2, x2, x1, lsr #7
	add	x0, x0, x24
	add	x24, x11, x8
	eor	x8, x25, x7, lsr #6
	add	x2, x2, x21
	ror	x25, x24, #14
	add	x26, x0, x11
	add	x11, x2, x3
	and	x0, x10, x24
	add	x21, x11, x8
	eor	x8, x25, x24, ror #18
	bic	x11, x12, x24
	ror	x25, x26, #28
	add	x2, x28, x21
	orr	x11, x0, x11
	ror	x0, x23, #1
	add	x16, x2, x16
	eor	x8, x8, x24, ror #41
	eor	x2, x25, x26, ror #34
	add	x11, x16, x11
	eor	x16, x26, x14
	eor	x0, x0, x23, ror #8
	ror	x25, x20, #19
	add	x11, x11, x8
	eor	x2, x2, x26, ror #39
	and	x28, x16, x29
	add	x8, x11, x18
	eor	x18, x0, x23, lsr #7
	eor	x0, x25, x20, ror #61
	eor	x25, x28, x14
	add	x2, x2, x25
	ror	x28, x8, #14
	eor	x25, x0, x20, lsr #6
	add	x18, x18, x1
	add	x0, x2, x11
	ldp	x2, x29, [x27, #48]
	add	x11, x18, x6
	add	x6, x11, x25
	ror	x11, x0, #28
	eor	x18, x28, x8, ror #18
	bic	x25, x10, x8
	and	x28, x24, x8
	add	x27, x27, #128
	add	x2, x2, x6
	eor	x11, x11, x0, ror #34
	orr	x25, x28, x25
	add	x12, x2, x12
	ror	x2, x30, #1
	eor	x18, x18, x8, ror #41
	add	x12, x12, x25
	eor	x25, x0, x26
	ror	x28, x21, #19
	eor	x2, x2, x30, ror #8
	eor	x11, x11, x0, ror #39
	and	x16, x25, x16
	add	x18, x12, x18
	eor	x12, x16, x26
	eor	x28, x28, x21, ror #61
	eor	x16, x2, x30, lsr #7
	add	x11, x11, x12
	add	x12, x18, x9
	add	x2, x11, x18
	eor	x9, x28, x21, lsr #6
	add	x11, x16, x23
	ror	x16, x12, #14
	ror	x18, x2, #28
	add	x11, x11, x4
	add	x23, x11, x9
	eor	x9, x16, x12, ror #18
	bic	x11, x24, x12
	and	x16, x8, x12
	add	x28, x29, x23
	eor	x18, x18, x2, ror #34
	orr	x11, x16, x11
	add	x16, x28, x10
	eor	x10, x2, x0
	eor	x9, x9, x12, ror #41
	eor	x18, x18, x2, ror #39
	and	x25, x10, x25
	add	x11, x16, x11
	eor	x16, x25, x0
	add	x9, x11, x9
	add	x11, x18, x16
	ldr	x16, [sp, #120]                 // 8-byte Folded Reload
	add	x14, x9, x14
	add	x9, x11, x9
	subs	x16, x16, #128
	b.ne	.LBB0_3
// %bb.4:                               //   in Loop: Header=BB0_2 Depth=1
	ldp	x5, x4, [sp, #24]               // 16-byte Folded Reload
	ldr	x11, [sp, #88]                  // 8-byte Folded Reload
	ldp	x16, x1, [sp, #72]              // 16-byte Folded Reload
	ldp	x18, x17, [sp, #40]             // 16-byte Folded Reload
	add	x11, x11, x24
	add	x4, x4, x2
	ldp	x3, x2, [sp, #56]               // 16-byte Folded Reload
	add	x5, x5, x9
	ldr	x9, [sp, #16]                   // 8-byte Folded Reload
	add	x1, x1, #128
	add	x18, x18, x0
	add	x17, x17, x26
	add	x16, x16, x8
	add	x3, x3, x14
	cmp	x1, x9
	add	x2, x2, x12
	b.ne	.LBB0_2
// %bb.5:
	ldr	x8, [sp, #8]                    // 8-byte Folded Reload
	stp	x5, x4, [x8]
	stp	x18, x17, [x8, #16]
	stp	x3, x2, [x8, #32]
	stp	x16, x11, [x8, #48]
.LBB0_6:
	ldp	x20, x19, [sp, #240]            // 16-byte Folded Reload
	ldp	x22, x21, [sp, #224]            // 16-byte Folded Reload
	ldp	x24, x23, [sp, #208]            // 16-byte Folded Reload
	ldp	x26, x25, [sp, #192]            // 16-byte Folded Reload
	ldp	x28, x27, [sp, #176]            // 16-byte Folded Reload
	ldp	x29, x30, [sp, #160]            // 16-byte Folded Reload
	add	sp, sp, #256
	ret
.Lfunc_end0:
	.size	_Z14sha512_invokerPmPKSt4byteS2_, .Lfunc_end0-_Z14sha512_invokerPmPKSt4byteS2_
                                        // -- End function
	.type	_ZN7fast_io7details6sha51210k512scalarE,@object // @_ZN7fast_io7details6sha51210k512scalarE
	.section	.rodata._ZN7fast_io7details6sha51210k512scalarE,"aG",@progbits,_ZN7fast_io7details6sha51210k512scalarE,comdat
	.weak	_ZN7fast_io7details6sha51210k512scalarE
	.p2align	3, 0x0
_ZN7fast_io7details6sha51210k512scalarE:
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
	.size	_ZN7fast_io7details6sha51210k512scalarE, 512

	.section	".linker-options","e",@llvm_linker_options
	.ident	"clang version 20.0.0git (git@github.com:trcrsired/llvm-project.git 3b9afec33ad48c7b4ddfc51cf9e1b286ccf9ac5c)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
