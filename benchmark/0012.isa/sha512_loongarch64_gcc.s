	.file	"sha512.cc"
	.text
	.align	2
	.align	3
	.globl	_Z14sha512_invokerPmPKSt4byteS2_
	.type	_Z14sha512_invokerPmPKSt4byteS2_, @function
_Z14sha512_invokerPmPKSt4byteS2_:
.LFB3169 = .
	.cfi_startproc
	addi.d	$r3,$r3,-224
	.cfi_def_cfa_offset 224
	st.d	$r6,$r3,128
	beq	$r5,$r6,.L1
	ld.d	$r12,$r4,8
	st.d	$r30,$r3,152
	st.d	$r31,$r3,144
	st.d	$r12,$r3,48
	ld.d	$r12,$r4,16
	.cfi_offset 30, -72
	.cfi_offset 31, -80
	ldptr.d	$r31,$r4,0
	ld.d	$r30,$r4,32
	st.d	$r12,$r3,56
	ld.d	$r12,$r4,24
	st.d	$r22,$r3,216
	st.d	$r23,$r3,208
	st.d	$r12,$r3,64
	ld.d	$r12,$r4,40
	st.d	$r24,$r3,200
	st.d	$r25,$r3,192
	st.d	$r12,$r3,72
	ld.d	$r12,$r4,48
	st.d	$r26,$r3,184
	st.d	$r27,$r3,176
	st.d	$r12,$r3,80
	ld.d	$r12,$r4,56
	st.d	$r28,$r3,168
	st.d	$r29,$r3,160
	.cfi_offset 22, -8
	.cfi_offset 23, -16
	.cfi_offset 24, -24
	.cfi_offset 25, -32
	.cfi_offset 26, -40
	.cfi_offset 27, -48
	.cfi_offset 28, -56
	.cfi_offset 29, -64
	or	$r22,$r5,$r0
	st.d	$r12,$r3,88
	st.d	$r4,$r3,136
	.align	3
.L4:
	ldptr.d	$r12,$r22,0
	ld.d	$r13,$r22,8
	rotri.d	$r15,$r30,14
	revb.4h	$r12,$r12
	revh.d	$r14,$r12
	revb.4h	$r13,$r13
	rotri.d	$r12,$r30,18
	revh.d	$r19,$r13
	xor	$r15,$r15,$r12
	rotri.d	$r13,$r30,41
	xor	$r15,$r15,$r13
	ld.d	$r13,$r3,88
	preld	0,$r22,136
	preld	0,$r22,144
	preld	0,$r22,152
	add.d	$r13,$r14,$r13
	st.d	$r14,$r3,16
	lu12i.w	$r14,-685203456>>12			# 0xffffffffd728a000
	ori	$r14,$r14,3618
	ld.d	$r20,$r3,72
	ld.d	$r18,$r3,80
	lu32i.d	$r14,0xfffa2f9800000000>>32
	lu52i.d	$r14,$r14,0x4280000000000000>>52
	add.d	$r13,$r13,$r14
	add.d	$r15,$r15,$r13
	xor	$r13,$r20,$r18
	and	$r13,$r13,$r30
	ld.d	$r5,$r3,48
	ld.d	$r4,$r3,56
	xor	$r13,$r13,$r18
	add.d	$r15,$r15,$r13
	rotri.d	$r16,$r31,28
	rotri.d	$r13,$r31,34
	xor	$r16,$r16,$r13
	rotri.d	$r14,$r31,39
	xor	$r12,$r5,$r31
	xor	$r13,$r5,$r4
	xor	$r16,$r16,$r14
	ld.d	$r14,$r22,16
	and	$r13,$r13,$r12
	xor	$r13,$r13,$r5
	add.d	$r16,$r16,$r13
	revb.4h	$r14,$r14
	ld.d	$r13,$r3,64
	revh.d	$r6,$r14
	add.d	$r14,$r19,$r18
	lu12i.w	$r18,602890240>>12			# 0x23ef6000
	ori	$r18,$r18,1485
	add.d	$r16,$r16,$r15
	xor	$r17,$r30,$r20
	add.d	$r15,$r15,$r13
	lu32i.d	$r18,0x7449100000000>>32
	and	$r17,$r17,$r15
	lu52i.d	$r18,$r18,0x7130000000000000>>52
	add.d	$r14,$r14,$r18
	xor	$r17,$r17,$r20
	rotri.d	$r18,$r15,18
	add.d	$r17,$r17,$r14
	rotri.d	$r14,$r15,14
	st.d	$r19,$r3,24
	xor	$r14,$r14,$r18
	rotri.d	$r19,$r16,28
	rotri.d	$r18,$r16,34
	xor	$r19,$r19,$r18
	rotri.d	$r18,$r15,41
	xor	$r13,$r16,$r31
	xor	$r14,$r14,$r18
	add.d	$r17,$r17,$r14
	and	$r12,$r12,$r13
	rotri.d	$r14,$r16,39
	xor	$r12,$r12,$r31
	xor	$r19,$r19,$r14
	add.d	$r19,$r19,$r12
	ld.d	$r12,$r22,24
	lu12i.w	$r18,-330485760>>12			# 0xffffffffec4d3000
	ori	$r18,$r18,2863
	add.d	$r19,$r19,$r17
	xor	$r14,$r15,$r30
	add.d	$r17,$r17,$r4
	revb.4h	$r12,$r12
	lu32i.d	$r18,0xfbcf00000000>>32
	revh.d	$r7,$r12
	and	$r14,$r14,$r17
	add.d	$r12,$r6,$r20
	lu52i.d	$r18,$r18,0xb5c0000000000000>>52
	add.d	$r12,$r12,$r18
	xor	$r14,$r14,$r30
	add.d	$r14,$r14,$r12
	rotri.d	$r18,$r17,14
	rotri.d	$r12,$r17,18
	rotri.d	$r20,$r19,34
	xor	$r18,$r18,$r12
	rotri.d	$r12,$r19,28
	xor	$r12,$r12,$r20
	rotri.d	$r20,$r17,41
	xor	$r4,$r16,$r19
	xor	$r18,$r18,$r20
	add.d	$r14,$r14,$r18
	and	$r13,$r13,$r4
	rotri.d	$r18,$r19,39
	xor	$r13,$r13,$r16
	xor	$r12,$r12,$r18
	add.d	$r12,$r12,$r13
	ld.d	$r13,$r22,32
	lu12i.w	$r18,-2121674752>>12			# 0xffffffff8189d000
	ori	$r18,$r18,3004
	add.d	$r12,$r12,$r14
	xor	$r20,$r15,$r17
	add.d	$r14,$r14,$r5
	revb.4h	$r13,$r13
	lu32i.d	$r18,0x5dba500000000>>32
	revh.d	$r8,$r13
	and	$r20,$r20,$r14
	add.d	$r13,$r7,$r30
	lu52i.d	$r18,$r18,0xe9b0000000000000>>52
	add.d	$r13,$r13,$r18
	xor	$r20,$r20,$r15
	add.d	$r20,$r20,$r13
	rotri.d	$r18,$r14,14
	rotri.d	$r13,$r14,18
	st.d	$r6,$r3,96
	xor	$r18,$r18,$r13
	rotri.d	$r6,$r12,28
	rotri.d	$r13,$r12,34
	xor	$r13,$r6,$r13
	rotri.d	$r6,$r14,41
	xor	$r18,$r18,$r6
	xor	$r5,$r19,$r12
	add.d	$r18,$r20,$r18
	rotri.d	$r20,$r12,39
	and	$r4,$r4,$r5
	xor	$r13,$r13,$r20
	lu12i.w	$r20,-213340160>>12			# 0xfffffffff348b000
	xor	$r4,$r4,$r19
	ori	$r20,$r20,1336
	add.d	$r13,$r13,$r4
	lu32i.d	$r20,0x6c25b00000000>>32
	add.d	$r13,$r13,$r18
	xor	$r4,$r17,$r14
	add.d	$r18,$r18,$r31
	lu52i.d	$r20,$r20,0x3950000000000000>>52
	and	$r4,$r4,$r18
	add.d	$r20,$r8,$r20
	add.d	$r20,$r20,$r15
	xor	$r4,$r4,$r17
	add.d	$r4,$r4,$r20
	rotri.d	$r15,$r18,18
	rotri.d	$r20,$r18,14
	st.d	$r7,$r3,32
	xor	$r20,$r20,$r15
	rotri.d	$r7,$r13,34
	rotri.d	$r15,$r13,28
	xor	$r15,$r15,$r7
	rotri.d	$r7,$r18,41
	xor	$r6,$r12,$r13
	xor	$r20,$r20,$r7
	add.d	$r20,$r4,$r20
	and	$r5,$r5,$r6
	rotri.d	$r4,$r13,39
	xor	$r5,$r5,$r12
	xor	$r15,$r15,$r4
	ld.d	$r25,$r22,40
	add.d	$r15,$r15,$r5
	add.d	$r16,$r16,$r20
	add.d	$r15,$r15,$r20
	lu12i.w	$r20,-1241133056>>12			# 0xffffffffb605d000
	ori	$r20,$r20,25
	revb.4h	$r25,$r25
	lu32i.d	$r20,0x111f100000000>>32
	xor	$r4,$r14,$r18
	revh.d	$r25,$r25
	lu52i.d	$r20,$r20,0x59f0000000000000>>52
	and	$r4,$r4,$r16
	add.d	$r20,$r25,$r20
	add.d	$r20,$r20,$r17
	xor	$r4,$r4,$r14
	add.d	$r4,$r4,$r20
	rotri.d	$r17,$r16,18
	rotri.d	$r20,$r16,14
	rotri.d	$r7,$r15,34
	xor	$r20,$r20,$r17
	rotri.d	$r17,$r15,28
	xor	$r17,$r17,$r7
	rotri.d	$r7,$r16,41
	xor	$r5,$r13,$r15
	xor	$r20,$r20,$r7
	add.d	$r20,$r4,$r20
	and	$r6,$r6,$r5
	rotri.d	$r4,$r15,39
	xor	$r6,$r6,$r13
	xor	$r17,$r17,$r4
	ld.d	$r24,$r22,48
	add.d	$r17,$r17,$r6
	add.d	$r19,$r19,$r20
	add.d	$r17,$r17,$r20
	lu12i.w	$r20,-1357299712>>12			# 0xffffffffaf194000
	ori	$r20,$r20,3995
	revb.4h	$r24,$r24
	lu32i.d	$r20,0xffff82a400000000>>32
	xor	$r4,$r18,$r16
	revh.d	$r24,$r24
	lu52i.d	$r20,$r20,0x9230000000000000>>52
	and	$r4,$r4,$r19
	add.d	$r20,$r24,$r20
	add.d	$r20,$r20,$r14
	xor	$r4,$r4,$r18
	add.d	$r4,$r4,$r20
	rotri.d	$r14,$r19,18
	rotri.d	$r20,$r19,14
	rotri.d	$r7,$r17,34
	xor	$r20,$r20,$r14
	rotri.d	$r14,$r17,28
	xor	$r14,$r14,$r7
	rotri.d	$r7,$r19,41
	xor	$r6,$r15,$r17
	xor	$r20,$r20,$r7
	add.d	$r20,$r4,$r20
	and	$r5,$r5,$r6
	rotri.d	$r4,$r17,39
	xor	$r5,$r5,$r15
	xor	$r14,$r14,$r4
	ld.d	$r23,$r22,56
	add.d	$r14,$r14,$r5
	add.d	$r12,$r12,$r20
	add.d	$r14,$r14,$r20
	lu12i.w	$r20,-630358016>>12			# 0xffffffffda6d8000
	ori	$r20,$r20,280
	revb.4h	$r23,$r23
	lu32i.d	$r20,0xfffc5ed500000000>>32
	xor	$r4,$r16,$r19
	revh.d	$r23,$r23
	lu52i.d	$r20,$r20,0xab10000000000000>>52
	and	$r4,$r4,$r12
	add.d	$r20,$r23,$r20
	add.d	$r20,$r20,$r18
	xor	$r4,$r4,$r16
	add.d	$r4,$r4,$r20
	rotri.d	$r18,$r12,18
	rotri.d	$r20,$r12,14
	rotri.d	$r7,$r14,34
	xor	$r20,$r20,$r18
	rotri.d	$r18,$r14,28
	xor	$r18,$r18,$r7
	rotri.d	$r7,$r12,41
	xor	$r5,$r17,$r14
	xor	$r20,$r20,$r7
	add.d	$r20,$r4,$r20
	and	$r6,$r6,$r5
	rotri.d	$r4,$r14,39
	xor	$r6,$r6,$r17
	xor	$r18,$r18,$r4
	ld.d	$r11,$r22,64
	add.d	$r18,$r18,$r6
	add.d	$r13,$r13,$r20
	add.d	$r18,$r18,$r20
	lu12i.w	$r20,-1560084480>>12			# 0xffffffffa3030000
	ori	$r20,$r20,578
	revb.4h	$r11,$r11
	lu32i.d	$r20,0x7aa9800000000>>32
	xor	$r4,$r19,$r12
	revh.d	$r11,$r11
	lu52i.d	$r20,$r20,0xd800000000000000>>52
	and	$r4,$r4,$r13
	add.d	$r20,$r11,$r20
	add.d	$r20,$r20,$r16
	xor	$r4,$r4,$r19
	add.d	$r4,$r4,$r20
	rotri.d	$r16,$r13,18
	rotri.d	$r20,$r13,14
	rotri.d	$r7,$r18,34
	xor	$r20,$r20,$r16
	rotri.d	$r16,$r18,28
	xor	$r16,$r16,$r7
	rotri.d	$r7,$r13,41
	xor	$r6,$r14,$r18
	xor	$r20,$r20,$r7
	add.d	$r20,$r4,$r20
	and	$r5,$r5,$r6
	rotri.d	$r4,$r18,39
	xor	$r5,$r5,$r14
	xor	$r16,$r16,$r4
	ld.d	$r10,$r22,72
	add.d	$r16,$r16,$r5
	add.d	$r15,$r15,$r20
	add.d	$r16,$r16,$r20
	lu12i.w	$r20,1164992512>>12			# 0x45706000
	ori	$r20,$r20,4030
	revb.4h	$r10,$r10
	lu32i.d	$r20,0x35b0100000000>>32
	xor	$r4,$r12,$r13
	revh.d	$r10,$r10
	lu52i.d	$r20,$r20,0x1280000000000000>>52
	and	$r4,$r4,$r15
	add.d	$r20,$r10,$r20
	add.d	$r20,$r20,$r19
	xor	$r4,$r4,$r12
	add.d	$r4,$r4,$r20
	rotri.d	$r19,$r15,18
	rotri.d	$r20,$r15,14
	xor	$r5,$r18,$r16
	rotri.d	$r7,$r16,34
	xor	$r20,$r20,$r19
	rotri.d	$r26,$r16,28
	rotri.d	$r19,$r15,41
	and	$r6,$r6,$r5
	xor	$r26,$r26,$r7
	xor	$r20,$r20,$r19
	rotri.d	$r19,$r16,39
	xor	$r6,$r6,$r18
	xor	$r26,$r26,$r19
	ld.d	$r9,$r22,80
	add.d	$r20,$r4,$r20
	add.d	$r26,$r26,$r6
	add.d	$r17,$r17,$r20
	add.d	$r19,$r26,$r20
	lu12i.w	$r20,1323610112>>12			# 0x4ee4b000
	ori	$r20,$r20,652
	revb.4h	$r9,$r9
	lu32i.d	$r20,0x185be00000000>>32
	xor	$r4,$r13,$r15
	revh.d	$r9,$r9
	lu52i.d	$r20,$r20,0x2430000000000000>>52
	and	$r4,$r4,$r17
	add.d	$r20,$r9,$r20
	add.d	$r20,$r20,$r12
	xor	$r4,$r4,$r13
	add.d	$r4,$r4,$r20
	rotri.d	$r12,$r17,18
	rotri.d	$r20,$r17,14
	rotri.d	$r7,$r19,34
	xor	$r20,$r20,$r12
	rotri.d	$r12,$r19,28
	xor	$r12,$r12,$r7
	rotri.d	$r7,$r17,41
	xor	$r6,$r16,$r19
	xor	$r20,$r20,$r7
	add.d	$r20,$r4,$r20
	and	$r5,$r5,$r6
	rotri.d	$r4,$r19,39
	xor	$r5,$r5,$r16
	xor	$r12,$r12,$r4
	st.d	$r8,$r3,40
	add.d	$r12,$r12,$r5
	ld.d	$r8,$r22,88
	add.d	$r14,$r14,$r20
	add.d	$r12,$r12,$r20
	lu12i.w	$r20,-704663552>>12			# 0xffffffffd5ffb000
	ori	$r20,$r20,1250
	revb.4h	$r8,$r8
	lu32i.d	$r20,0xfffc7dc300000000>>32
	xor	$r4,$r15,$r17
	revh.d	$r8,$r8
	lu52i.d	$r20,$r20,0x5500000000000000>>52
	and	$r4,$r4,$r14
	add.d	$r20,$r8,$r20
	add.d	$r20,$r20,$r13
	xor	$r4,$r4,$r15
	add.d	$r4,$r4,$r20
	rotri.d	$r13,$r14,18
	rotri.d	$r20,$r14,14
	rotri.d	$r26,$r12,34
	xor	$r20,$r20,$r13
	rotri.d	$r13,$r12,28
	xor	$r13,$r13,$r26
	rotri.d	$r26,$r14,41
	xor	$r5,$r19,$r12
	xor	$r20,$r20,$r26
	add.d	$r20,$r4,$r20
	and	$r6,$r6,$r5
	rotri.d	$r4,$r12,39
	xor	$r6,$r6,$r19
	xor	$r13,$r13,$r4
	ld.d	$r7,$r22,96
	add.d	$r13,$r13,$r6
	add.d	$r18,$r18,$r20
	add.d	$r13,$r13,$r20
	lu12i.w	$r20,-226787328>>12			# 0xfffffffff27b8000
	ori	$r20,$r20,2415
	revb.4h	$r7,$r7
	lu32i.d	$r20,0xfffe5d7400000000>>32
	xor	$r4,$r17,$r14
	revh.d	$r7,$r7
	lu52i.d	$r20,$r20,0x72b0000000000000>>52
	and	$r4,$r4,$r18
	add.d	$r20,$r7,$r20
	add.d	$r20,$r20,$r15
	xor	$r4,$r4,$r17
	add.d	$r4,$r4,$r20
	rotri.d	$r15,$r18,18
	rotri.d	$r20,$r18,14
	rotri.d	$r26,$r13,34
	xor	$r20,$r20,$r15
	rotri.d	$r15,$r13,28
	xor	$r15,$r15,$r26
	rotri.d	$r26,$r18,41
	xor	$r28,$r12,$r13
	xor	$r20,$r20,$r26
	add.d	$r20,$r4,$r20
	and	$r5,$r5,$r28
	rotri.d	$r4,$r13,39
	xor	$r5,$r5,$r12
	xor	$r15,$r15,$r4
	ld.d	$r6,$r22,104
	add.d	$r15,$r15,$r5
	add.d	$r16,$r16,$r20
	add.d	$r15,$r15,$r20
	lu12i.w	$r20,991334400>>12			# 0x3b169000
	ori	$r20,$r20,1713
	revb.4h	$r6,$r6
	lu32i.d	$r20,0xfffeb1fe00000000>>32
	xor	$r4,$r14,$r18
	revh.d	$r6,$r6
	lu52i.d	$r20,$r20,0x80d0000000000000>>52
	and	$r4,$r4,$r16
	add.d	$r20,$r6,$r20
	add.d	$r20,$r20,$r17
	xor	$r4,$r4,$r14
	add.d	$r4,$r4,$r20
	rotri.d	$r17,$r16,18
	rotri.d	$r20,$r16,14
	xor	$r20,$r20,$r17
	rotri.d	$r26,$r15,34
	rotri.d	$r17,$r15,28
	xor	$r17,$r17,$r26
	rotri.d	$r26,$r16,41
	xor	$r20,$r20,$r26
	add.d	$r20,$r4,$r20
	xor	$r27,$r13,$r15
	add.d	$r26,$r19,$r20
	and	$r28,$r28,$r27
	rotri.d	$r19,$r15,39
	xor	$r28,$r28,$r13
	xor	$r17,$r17,$r19
	ld.d	$r5,$r22,112
	add.d	$r17,$r17,$r28
	add.d	$r17,$r17,$r20
	lu12i.w	$r20,633802752>>12			# 0x25c71000
	ori	$r20,$r20,565
	revb.4h	$r5,$r5
	lu32i.d	$r20,0xfffc06a700000000>>32
	xor	$r28,$r18,$r16
	revh.d	$r5,$r5
	lu52i.d	$r20,$r20,0x9bd0000000000000>>52
	and	$r28,$r28,$r26
	add.d	$r20,$r5,$r20
	add.d	$r20,$r20,$r14
	xor	$r28,$r28,$r18
	add.d	$r28,$r28,$r20
	rotri.d	$r14,$r26,18
	rotri.d	$r20,$r26,14
	xor	$r20,$r20,$r14
	rotri.d	$r29,$r17,34
	rotri.d	$r14,$r17,28
	xor	$r14,$r14,$r29
	rotri.d	$r29,$r26,41
	xor	$r19,$r15,$r17
	xor	$r20,$r20,$r29
	add.d	$r20,$r28,$r20
	and	$r27,$r27,$r19
	rotri.d	$r28,$r17,39
	xor	$r27,$r27,$r15
	xor	$r14,$r14,$r28
	ld.d	$r4,$r22,120
	add.d	$r14,$r14,$r27
	add.d	$r12,$r12,$r20
	add.d	$r14,$r14,$r20
	lu12i.w	$r20,-815194112>>12			# 0xffffffffcf692000
	ori	$r20,$r20,1684
	revb.4h	$r4,$r4
	lu32i.d	$r20,0xfffbf17400000000>>32
	revh.d	$r4,$r4
	xor	$r28,$r16,$r26
	lu52i.d	$r20,$r20,0xc190000000000000>>52
	and	$r28,$r28,$r12
	add.d	$r20,$r4,$r20
	add.d	$r20,$r20,$r18
	xor	$r28,$r28,$r16
	add.d	$r28,$r28,$r20
	rotri.d	$r18,$r12,18
	rotri.d	$r20,$r12,14
	xor	$r20,$r20,$r18
	rotri.d	$r29,$r14,34
	rotri.d	$r18,$r14,28
	xor	$r18,$r18,$r29
	rotri.d	$r29,$r12,41
	addi.d	$r22,$r22,128
	xor	$r27,$r17,$r14
	xor	$r20,$r20,$r29
	add.d	$r20,$r28,$r20
	preld	0,$r22,0
	and	$r19,$r19,$r27
	rotri.d	$r28,$r14,39
	st.d	$r25,$r3,8
	st.d	$r31,$r3,104
	xor	$r19,$r19,$r17
	xor	$r18,$r18,$r28
	st.d	$r22,$r3,120
	ld.d	$r22,$r3,96
	add.d	$r18,$r18,$r19
	add.d	$r13,$r13,$r20
	add.d	$r18,$r18,$r20
	st.d	$r30,$r3,112
	la.local	$r20,_ZN7fast_io7details6sha51210k512scalarE+128
.L3:
	ld.d	$r31,$r3,24
	rotri.d	$r30,$r5,19
	preld	0,$r20,0
	rotri.d	$r28,$r31,8
	rotri.d	$r19,$r31,1
	srli.d	$r29,$r31,7
	xor	$r19,$r19,$r28
	rotri.d	$r28,$r5,61
	xor	$r30,$r30,$r28
	xor	$r19,$r19,$r29
	add.d	$r29,$r31,$r9
	srli.d	$r31,$r5,6
	xor	$r30,$r30,$r31
	add.d	$r19,$r19,$r30
	ld.d	$r30,$r3,16
	add.d	$r19,$r19,$r10
	xor	$r28,$r12,$r26
	add.d	$r25,$r30,$r19
	rotri.d	$r30,$r13,14
	rotri.d	$r19,$r13,18
	ld.d	$r31,$r20,-128
	and	$r28,$r28,$r13
	xor	$r19,$r30,$r19
	rotri.d	$r30,$r13,41
	xor	$r28,$r28,$r26
	xor	$r19,$r19,$r30
	add.d	$r19,$r19,$r28
	add.d	$r19,$r19,$r31
	add.d	$r19,$r19,$r25
	add.d	$r16,$r19,$r16
	rotri.d	$r28,$r18,34
	rotri.d	$r19,$r18,28
	xor	$r31,$r18,$r14
	xor	$r19,$r19,$r28
	rotri.d	$r28,$r18,39
	and	$r27,$r31,$r27
	xor	$r19,$r19,$r28
	xor	$r27,$r27,$r14
	add.d	$r19,$r19,$r16
	add.d	$r15,$r16,$r15
	add.d	$r19,$r19,$r27
	rotri.d	$r16,$r22,8
	rotri.d	$r27,$r22,1
	xor	$r27,$r27,$r16
	ld.d	$r16,$r20,-120
	rotri.d	$r28,$r4,19
	add.d	$r30,$r22,$r8
	add.d	$r16,$r26,$r16
	srli.d	$r26,$r22,7
	xor	$r26,$r27,$r26
	rotri.d	$r27,$r4,61
	xor	$r27,$r28,$r27
	srli.d	$r28,$r4,6
	xor	$r27,$r27,$r28
	add.d	$r26,$r26,$r29
	xor	$r28,$r13,$r12
	add.d	$r26,$r26,$r27
	and	$r28,$r28,$r15
	ld.d	$r22,$r3,32
	add.d	$r16,$r16,$r26
	xor	$r28,$r28,$r12
	preld	0,$r20,8
	preld	0,$r20,16
	preld	0,$r20,24
	add.d	$r28,$r28,$r16
	st.d	$r26,$r3,24
	rotri.d	$r16,$r15,18
	rotri.d	$r26,$r15,14
	rotri.d	$r27,$r19,34
	xor	$r26,$r26,$r16
	rotri.d	$r16,$r19,28
	xor	$r16,$r16,$r27
	rotri.d	$r27,$r22,1
	rotri.d	$r22,$r22,8
	xor	$r27,$r27,$r22
	rotri.d	$r22,$r15,41
	xor	$r29,$r19,$r18
	xor	$r26,$r26,$r22
	add.d	$r26,$r28,$r26
	and	$r31,$r31,$r29
	rotri.d	$r28,$r19,39
	xor	$r31,$r31,$r18
	xor	$r16,$r16,$r28
	add.d	$r16,$r16,$r31
	add.d	$r17,$r26,$r17
	add.d	$r16,$r16,$r26
	ld.d	$r28,$r3,32
	ld.d	$r26,$r20,-112
	st.d	$r25,$r3,16
	add.d	$r31,$r28,$r7
	add.d	$r12,$r12,$r26
	srli.d	$r26,$r28,7
	xor	$r27,$r27,$r26
	add.d	$r27,$r27,$r30
	ld.d	$r30,$r3,16
	addi.d	$r20,$r20,128
	rotri.d	$r28,$r30,19
	rotri.d	$r26,$r30,61
	xor	$r26,$r28,$r26
	srli.d	$r28,$r30,6
	xor	$r26,$r26,$r28
	xor	$r28,$r15,$r13
	add.d	$r22,$r27,$r26
	and	$r28,$r28,$r17
	add.d	$r12,$r12,$r22
	xor	$r28,$r28,$r13
	add.d	$r28,$r28,$r12
	rotri.d	$r26,$r17,14
	rotri.d	$r12,$r17,18
	rotri.d	$r27,$r16,34
	xor	$r26,$r26,$r12
	rotri.d	$r12,$r16,28
	xor	$r12,$r12,$r27
	ld.d	$r27,$r3,40
	xor	$r30,$r19,$r16
	and	$r29,$r29,$r30
	rotri.d	$r25,$r27,1
	rotri.d	$r27,$r27,8
	xor	$r27,$r25,$r27
	rotri.d	$r25,$r17,41
	xor	$r26,$r26,$r25
	add.d	$r26,$r28,$r26
	rotri.d	$r28,$r16,39
	xor	$r29,$r29,$r19
	xor	$r12,$r12,$r28
	add.d	$r12,$r12,$r29
	add.d	$r14,$r26,$r14
	add.d	$r12,$r12,$r26
	ld.d	$r28,$r3,40
	ld.d	$r26,$r20,-232
	ld.d	$r25,$r3,8
	add.d	$r29,$r28,$r6
	add.d	$r13,$r13,$r26
	srli.d	$r26,$r28,7
	xor	$r27,$r27,$r26
	ld.d	$r26,$r3,24
	add.d	$r27,$r27,$r31
	xor	$r31,$r15,$r17
	rotri.d	$r28,$r26,19
	rotri.d	$r26,$r26,61
	xor	$r26,$r28,$r26
	ld.d	$r28,$r3,24
	and	$r31,$r31,$r14
	xor	$r31,$r31,$r15
	srli.d	$r28,$r28,6
	xor	$r26,$r26,$r28
	add.d	$r26,$r27,$r26
	add.d	$r13,$r13,$r26
	st.d	$r26,$r3,32
	add.d	$r31,$r31,$r13
	rotri.d	$r26,$r14,14
	rotri.d	$r13,$r14,18
	rotri.d	$r28,$r12,34
	xor	$r26,$r26,$r13
	rotri.d	$r13,$r12,28
	xor	$r13,$r13,$r28
	rotri.d	$r28,$r25,1
	rotri.d	$r25,$r25,8
	xor	$r28,$r28,$r25
	rotri.d	$r25,$r14,41
	xor	$r27,$r16,$r12
	xor	$r26,$r26,$r25
	add.d	$r26,$r31,$r26
	and	$r30,$r30,$r27
	rotri.d	$r31,$r12,39
	xor	$r30,$r30,$r16
	xor	$r13,$r13,$r31
	add.d	$r13,$r13,$r30
	add.d	$r18,$r26,$r18
	add.d	$r13,$r13,$r26
	ld.d	$r25,$r3,8
	ld.d	$r26,$r20,-224
	rotri.d	$r31,$r24,8
	add.d	$r15,$r15,$r26
	srli.d	$r26,$r25,7
	xor	$r28,$r28,$r26
	add.d	$r28,$r28,$r29
	rotri.d	$r26,$r22,61
	rotri.d	$r29,$r22,19
	xor	$r26,$r29,$r26
	srli.d	$r29,$r22,6
	xor	$r26,$r26,$r29
	xor	$r29,$r17,$r14
	add.d	$r28,$r28,$r26
	and	$r29,$r29,$r18
	st.d	$r28,$r3,40
	add.d	$r15,$r15,$r28
	xor	$r29,$r29,$r17
	xor	$r28,$r12,$r13
	add.d	$r29,$r29,$r15
	and	$r27,$r27,$r28
	rotri.d	$r15,$r18,18
	rotri.d	$r26,$r18,14
	xor	$r30,$r27,$r12
	xor	$r26,$r26,$r15
	rotri.d	$r27,$r13,34
	rotri.d	$r15,$r13,28
	xor	$r15,$r15,$r27
	rotri.d	$r27,$r24,1
	xor	$r27,$r27,$r31
	rotri.d	$r31,$r18,41
	xor	$r26,$r26,$r31
	add.d	$r26,$r29,$r26
	rotri.d	$r29,$r13,39
	xor	$r15,$r15,$r29
	add.d	$r15,$r15,$r30
	add.d	$r19,$r19,$r26
	add.d	$r15,$r15,$r26
	ld.d	$r26,$r20,-216
	ld.d	$r29,$r3,32
	add.d	$r25,$r5,$r25
	add.d	$r17,$r17,$r26
	srli.d	$r26,$r24,7
	xor	$r27,$r27,$r26
	add.d	$r27,$r27,$r25
	rotri.d	$r26,$r29,19
	rotri.d	$r25,$r29,61
	xor	$r25,$r26,$r25
	srli.d	$r26,$r29,6
	xor	$r25,$r25,$r26
	add.d	$r30,$r27,$r25
	xor	$r27,$r14,$r18
	and	$r27,$r27,$r19
	add.d	$r17,$r17,$r30
	xor	$r27,$r27,$r14
	add.d	$r27,$r27,$r17
	rotri.d	$r29,$r23,1
	rotri.d	$r17,$r23,8
	xor	$r29,$r29,$r17
	rotri.d	$r26,$r19,14
	rotri.d	$r17,$r19,18
	rotri.d	$r31,$r15,34
	xor	$r26,$r26,$r17
	rotri.d	$r17,$r15,28
	xor	$r17,$r17,$r31
	srli.d	$r31,$r23,7
	add.d	$r24,$r4,$r24
	xor	$r29,$r29,$r31
	add.d	$r29,$r29,$r24
	rotri.d	$r24,$r19,41
	xor	$r26,$r26,$r24
	st.d	$r30,$r3,8
	add.d	$r26,$r27,$r26
	xor	$r30,$r13,$r15
	add.d	$r27,$r16,$r26
	and	$r28,$r28,$r30
	rotri.d	$r16,$r15,39
	xor	$r28,$r28,$r13
	xor	$r17,$r17,$r16
	add.d	$r17,$r17,$r28
	add.d	$r17,$r17,$r26
	ld.d	$r26,$r3,40
	xor	$r28,$r18,$r19
	and	$r28,$r28,$r27
	rotri.d	$r16,$r26,19
	rotri.d	$r24,$r26,61
	xor	$r24,$r16,$r24
	srli.d	$r16,$r26,6
	xor	$r24,$r24,$r16
	ld.d	$r16,$r20,-208
	add.d	$r24,$r29,$r24
	add.d	$r14,$r14,$r24
	add.d	$r14,$r14,$r16
	rotri.d	$r26,$r11,8
	rotri.d	$r16,$r11,1
	xor	$r28,$r28,$r18
	add.d	$r28,$r28,$r14
	xor	$r16,$r16,$r26
	srli.d	$r14,$r11,7
	xor	$r16,$r16,$r14
	add.d	$r16,$r16,$r23
	rotri.d	$r14,$r27,18
	rotri.d	$r23,$r27,14
	rotri.d	$r29,$r17,34
	xor	$r23,$r23,$r14
	rotri.d	$r14,$r17,28
	xor	$r14,$r14,$r29
	rotri.d	$r29,$r27,41
	xor	$r26,$r15,$r17
	xor	$r23,$r23,$r29
	add.d	$r23,$r28,$r23
	and	$r30,$r30,$r26
	rotri.d	$r28,$r17,39
	xor	$r30,$r30,$r15
	xor	$r14,$r14,$r28
	add.d	$r14,$r14,$r30
	ld.d	$r30,$r3,8
	ld.d	$r31,$r3,16
	add.d	$r12,$r12,$r23
	add.d	$r14,$r14,$r23
	rotri.d	$r28,$r30,19
	rotri.d	$r23,$r30,61
	ld.d	$r29,$r20,-200
	xor	$r23,$r28,$r23
	srli.d	$r28,$r30,6
	add.d	$r16,$r16,$r31
	xor	$r23,$r23,$r28
	add.d	$r23,$r16,$r23
	add.d	$r16,$r23,$r29
	xor	$r29,$r19,$r27
	and	$r29,$r29,$r12
	add.d	$r18,$r16,$r18
	rotri.d	$r28,$r10,8
	rotri.d	$r16,$r10,1
	xor	$r29,$r29,$r19
	add.d	$r29,$r29,$r18
	xor	$r16,$r16,$r28
	srli.d	$r18,$r10,7
	xor	$r16,$r16,$r18
	ld.d	$r18,$r3,24
	add.d	$r16,$r16,$r11
	rotri.d	$r11,$r12,14
	add.d	$r16,$r16,$r18
	rotri.d	$r18,$r12,18
	rotri.d	$r30,$r14,34
	xor	$r11,$r11,$r18
	rotri.d	$r18,$r14,28
	xor	$r18,$r18,$r30
	rotri.d	$r30,$r12,41
	xor	$r28,$r17,$r14
	xor	$r11,$r11,$r30
	add.d	$r11,$r29,$r11
	and	$r26,$r26,$r28
	rotri.d	$r29,$r14,39
	xor	$r26,$r26,$r17
	xor	$r18,$r18,$r29
	add.d	$r18,$r18,$r26
	add.d	$r13,$r13,$r11
	add.d	$r18,$r18,$r11
	rotri.d	$r26,$r24,19
	rotri.d	$r11,$r24,61
	ld.d	$r29,$r20,-192
	xor	$r11,$r26,$r11
	srli.d	$r26,$r24,6
	xor	$r11,$r11,$r26
	add.d	$r11,$r16,$r11
	add.d	$r16,$r11,$r29
	xor	$r29,$r27,$r12
	and	$r29,$r29,$r13
	add.d	$r16,$r16,$r19
	rotri.d	$r26,$r9,8
	rotri.d	$r19,$r9,1
	xor	$r29,$r29,$r27
	add.d	$r29,$r29,$r16
	xor	$r19,$r19,$r26
	srli.d	$r16,$r9,7
	xor	$r19,$r19,$r16
	add.d	$r19,$r19,$r10
	rotri.d	$r16,$r13,18
	rotri.d	$r10,$r13,14
	rotri.d	$r30,$r18,34
	xor	$r10,$r10,$r16
	rotri.d	$r16,$r18,28
	xor	$r16,$r16,$r30
	rotri.d	$r30,$r13,41
	xor	$r26,$r14,$r18
	xor	$r10,$r10,$r30
	add.d	$r10,$r29,$r10
	and	$r28,$r28,$r26
	rotri.d	$r29,$r18,39
	xor	$r28,$r28,$r14
	xor	$r16,$r16,$r29
	add.d	$r16,$r16,$r28
	add.d	$r15,$r15,$r10
	add.d	$r16,$r16,$r10
	rotri.d	$r28,$r23,19
	rotri.d	$r10,$r23,61
	ld.d	$r29,$r20,-184
	xor	$r10,$r28,$r10
	srli.d	$r28,$r23,6
	add.d	$r19,$r19,$r22
	xor	$r10,$r10,$r28
	add.d	$r10,$r19,$r10
	add.d	$r19,$r10,$r29
	rotri.d	$r28,$r8,8
	add.d	$r27,$r19,$r27
	rotri.d	$r19,$r8,1
	xor	$r19,$r19,$r28
	xor	$r28,$r12,$r13
	and	$r28,$r28,$r15
	xor	$r28,$r28,$r12
	add.d	$r27,$r27,$r28
	xor	$r28,$r18,$r16
	and	$r26,$r26,$r28
	xor	$r29,$r26,$r18
	srli.d	$r26,$r8,7
	xor	$r19,$r19,$r26
	add.d	$r19,$r19,$r9
	ld.d	$r9,$r3,32
	rotri.d	$r26,$r15,18
	rotri.d	$r30,$r16,34
	add.d	$r19,$r19,$r9
	rotri.d	$r9,$r15,14
	xor	$r9,$r9,$r26
	rotri.d	$r26,$r16,28
	xor	$r26,$r26,$r30
	rotri.d	$r30,$r15,41
	xor	$r9,$r9,$r30
	add.d	$r9,$r27,$r9
	rotri.d	$r27,$r16,39
	xor	$r26,$r26,$r27
	add.d	$r26,$r26,$r29
	add.d	$r17,$r17,$r9
	add.d	$r26,$r26,$r9
	rotri.d	$r27,$r11,19
	rotri.d	$r9,$r11,61
	xor	$r9,$r27,$r9
	ld.d	$r29,$r20,-176
	srli.d	$r27,$r11,6
	xor	$r9,$r9,$r27
	add.d	$r9,$r19,$r9
	add.d	$r29,$r9,$r29
	add.d	$r29,$r29,$r12
	rotri.d	$r19,$r7,1
	rotri.d	$r12,$r7,8
	xor	$r19,$r19,$r12
	xor	$r12,$r13,$r15
	and	$r12,$r12,$r17
	xor	$r12,$r12,$r13
	add.d	$r29,$r29,$r12
	srli.d	$r12,$r7,7
	xor	$r19,$r19,$r12
	ld.d	$r12,$r3,40
	add.d	$r19,$r19,$r8
	rotri.d	$r8,$r17,14
	add.d	$r19,$r19,$r12
	rotri.d	$r12,$r17,18
	rotri.d	$r30,$r26,34
	xor	$r8,$r8,$r12
	rotri.d	$r12,$r26,28
	xor	$r12,$r12,$r30
	rotri.d	$r30,$r17,41
	xor	$r27,$r16,$r26
	xor	$r8,$r8,$r30
	add.d	$r8,$r29,$r8
	and	$r28,$r28,$r27
	rotri.d	$r29,$r26,39
	xor	$r28,$r28,$r16
	xor	$r12,$r12,$r29
	add.d	$r12,$r12,$r28
	add.d	$r14,$r14,$r8
	add.d	$r12,$r12,$r8
	rotri.d	$r28,$r10,19
	rotri.d	$r8,$r10,61
	xor	$r8,$r28,$r8
	ld.d	$r29,$r20,-168
	srli.d	$r28,$r10,6
	xor	$r8,$r8,$r28
	add.d	$r8,$r19,$r8
	add.d	$r29,$r8,$r29
	add.d	$r29,$r29,$r13
	rotri.d	$r19,$r6,1
	rotri.d	$r13,$r6,8
	xor	$r19,$r19,$r13
	xor	$r13,$r15,$r17
	and	$r13,$r13,$r14
	xor	$r13,$r13,$r15
	add.d	$r29,$r29,$r13
	ld.d	$r30,$r3,8
	srli.d	$r13,$r6,7
	xor	$r19,$r19,$r13
	add.d	$r19,$r19,$r7
	rotri.d	$r13,$r14,18
	rotri.d	$r7,$r14,14
	add.d	$r19,$r19,$r30
	xor	$r7,$r7,$r13
	rotri.d	$r30,$r12,34
	rotri.d	$r13,$r12,28
	xor	$r13,$r13,$r30
	rotri.d	$r30,$r14,41
	xor	$r28,$r26,$r12
	xor	$r7,$r7,$r30
	add.d	$r7,$r29,$r7
	and	$r27,$r27,$r28
	rotri.d	$r29,$r12,39
	xor	$r27,$r27,$r26
	xor	$r13,$r13,$r29
	add.d	$r13,$r13,$r27
	add.d	$r18,$r18,$r7
	add.d	$r13,$r13,$r7
	rotri.d	$r27,$r9,19
	rotri.d	$r7,$r9,61
	xor	$r7,$r27,$r7
	ld.d	$r29,$r20,-160
	srli.d	$r27,$r9,6
	xor	$r7,$r7,$r27
	add.d	$r7,$r19,$r7
	add.d	$r29,$r7,$r29
	add.d	$r29,$r29,$r15
	rotri.d	$r19,$r5,1
	rotri.d	$r15,$r5,8
	xor	$r19,$r19,$r15
	xor	$r15,$r17,$r14
	and	$r15,$r15,$r18
	xor	$r15,$r15,$r17
	add.d	$r29,$r29,$r15
	srli.d	$r15,$r5,7
	xor	$r19,$r19,$r15
	add.d	$r19,$r19,$r6
	rotri.d	$r15,$r18,18
	rotri.d	$r6,$r18,14
	rotri.d	$r30,$r13,34
	xor	$r6,$r6,$r15
	rotri.d	$r15,$r13,28
	xor	$r15,$r15,$r30
	rotri.d	$r30,$r18,41
	xor	$r27,$r12,$r13
	xor	$r6,$r6,$r30
	add.d	$r6,$r29,$r6
	and	$r28,$r28,$r27
	rotri.d	$r29,$r13,39
	xor	$r28,$r28,$r12
	xor	$r15,$r15,$r29
	add.d	$r15,$r15,$r28
	add.d	$r16,$r16,$r6
	add.d	$r15,$r15,$r6
	rotri.d	$r28,$r8,19
	rotri.d	$r6,$r8,61
	ld.d	$r29,$r20,-152
	xor	$r6,$r28,$r6
	srli.d	$r28,$r8,6
	add.d	$r19,$r19,$r24
	xor	$r6,$r6,$r28
	add.d	$r6,$r19,$r6
	add.d	$r28,$r6,$r29
	add.d	$r28,$r28,$r17
	rotri.d	$r19,$r4,1
	rotri.d	$r17,$r4,8
	xor	$r19,$r19,$r17
	xor	$r17,$r14,$r18
	and	$r17,$r17,$r16
	xor	$r17,$r17,$r14
	add.d	$r28,$r28,$r17
	srli.d	$r17,$r4,7
	xor	$r19,$r19,$r17
	add.d	$r19,$r19,$r5
	rotri.d	$r17,$r16,18
	rotri.d	$r5,$r16,14
	xor	$r5,$r5,$r17
	rotri.d	$r30,$r15,34
	rotri.d	$r17,$r15,28
	xor	$r17,$r17,$r30
	rotri.d	$r30,$r16,41
	xor	$r29,$r13,$r15
	xor	$r5,$r5,$r30
	add.d	$r5,$r28,$r5
	and	$r27,$r27,$r29
	rotri.d	$r28,$r15,39
	xor	$r27,$r27,$r13
	xor	$r17,$r17,$r28
	add.d	$r17,$r17,$r27
	add.d	$r17,$r17,$r5
	add.d	$r26,$r26,$r5
	rotri.d	$r27,$r7,19
	rotri.d	$r5,$r7,61
	ld.d	$r28,$r20,-144
	xor	$r5,$r27,$r5
	srli.d	$r27,$r7,6
	add.d	$r19,$r19,$r23
	xor	$r5,$r5,$r27
	xor	$r30,$r18,$r16
	add.d	$r5,$r19,$r5
	add.d	$r19,$r5,$r28
	and	$r30,$r30,$r26
	add.d	$r14,$r19,$r14
	xor	$r30,$r30,$r18
	add.d	$r30,$r30,$r14
	rotri.d	$r19,$r31,1
	rotri.d	$r14,$r31,8
	xor	$r19,$r19,$r14
	rotri.d	$r27,$r26,14
	rotri.d	$r14,$r26,18
	or	$r25,$r31,$r0
	xor	$r27,$r27,$r14
	rotri.d	$r31,$r17,34
	rotri.d	$r14,$r17,28
	xor	$r14,$r14,$r31
	srli.d	$r31,$r25,7
	xor	$r19,$r19,$r31
	xor	$r28,$r15,$r17
	add.d	$r19,$r19,$r4
	rotri.d	$r4,$r26,41
	and	$r29,$r29,$r28
	xor	$r27,$r27,$r4
	rotri.d	$r4,$r17,39
	xor	$r29,$r29,$r15
	xor	$r14,$r14,$r4
	add.d	$r27,$r30,$r27
	add.d	$r14,$r14,$r29
	add.d	$r12,$r12,$r27
	add.d	$r14,$r14,$r27
	rotri.d	$r4,$r6,19
	rotri.d	$r27,$r6,61
	xor	$r4,$r4,$r27
	srli.d	$r29,$r6,6
	ld.d	$r27,$r20,-136
	add.d	$r19,$r19,$r11
	xor	$r4,$r4,$r29
	add.d	$r4,$r19,$r4
	add.d	$r27,$r4,$r27
	add.d	$r27,$r27,$r18
	xor	$r29,$r16,$r26
	rotri.d	$r18,$r12,18
	rotri.d	$r19,$r12,14
	xor	$r19,$r19,$r18
	rotri.d	$r30,$r14,34
	and	$r29,$r29,$r12
	rotri.d	$r18,$r14,28
	xor	$r18,$r18,$r30
	xor	$r29,$r29,$r16
	rotri.d	$r30,$r12,41
	add.d	$r29,$r29,$r27
	xor	$r19,$r19,$r30
	xor	$r27,$r17,$r14
	add.d	$r19,$r29,$r19
	and	$r28,$r28,$r27
	rotri.d	$r29,$r14,39
	xor	$r28,$r28,$r17
	xor	$r18,$r18,$r29
	add.d	$r18,$r18,$r28
	add.d	$r13,$r13,$r19
	add.d	$r18,$r18,$r19
	la.local	$r19,_ZN7fast_io7details6sha51210k512scalarE+640
	bne	$r19,$r20,.L3
	ld.d	$r30,$r3,112
	ld.d	$r31,$r3,104
	ld.d	$r22,$r3,120
	add.d	$r30,$r30,$r13
	ld.d	$r13,$r3,72
	add.d	$r31,$r31,$r18
	ld.d	$r18,$r3,48
	add.d	$r12,$r13,$r12
	st.d	$r12,$r3,72
	ld.d	$r12,$r3,80
	add.d	$r14,$r18,$r14
	st.d	$r14,$r3,48
	ld.d	$r14,$r3,56
	add.d	$r12,$r12,$r26
	st.d	$r12,$r3,80
	ld.d	$r12,$r3,88
	add.d	$r14,$r14,$r17
	st.d	$r14,$r3,56
	ld.d	$r14,$r3,64
	add.d	$r12,$r12,$r16
	st.d	$r12,$r3,88
	ld.d	$r12,$r3,128
	add.d	$r14,$r14,$r15
	st.d	$r14,$r3,64
	bne	$r22,$r12,.L4
	ld.d	$r4,$r3,136
	ld.d	$r12,$r3,48
	ld.d	$r22,$r3,216
	.cfi_restore 22
	stptr.d	$r31,$r4,0
	st.d	$r12,$r4,8
	ld.d	$r12,$r3,56
	st.d	$r30,$r4,32
	ld.d	$r23,$r3,208
	.cfi_restore 23
	st.d	$r12,$r4,16
	ld.d	$r12,$r3,72
	ld.d	$r24,$r3,200
	.cfi_restore 24
	ld.d	$r25,$r3,192
	.cfi_restore 25
	st.d	$r12,$r4,40
	ld.d	$r12,$r3,80
	ld.d	$r26,$r3,184
	.cfi_restore 26
	ld.d	$r27,$r3,176
	.cfi_restore 27
	st.d	$r12,$r4,48
	ld.d	$r12,$r3,88
	ld.d	$r28,$r3,168
	.cfi_restore 28
	ld.d	$r29,$r3,160
	.cfi_restore 29
	ld.d	$r30,$r3,152
	.cfi_restore 30
	ld.d	$r31,$r3,144
	.cfi_restore 31
	st.d	$r14,$r4,24
	st.d	$r12,$r4,56
.L1:
	addi.d	$r3,$r3,224
	.cfi_def_cfa_offset 0
	jr	$r1
	.cfi_endproc
.LFE3169:
	.size	_Z14sha512_invokerPmPKSt4byteS2_, .-_Z14sha512_invokerPmPKSt4byteS2_
	.weak	_ZN7fast_io7details6sha51210k512scalarE
	.section	.rodata._ZN7fast_io7details6sha51210k512scalarE,"aG",@progbits,_ZN7fast_io7details6sha51210k512scalarE,comdat
	.align	3
	.type	_ZN7fast_io7details6sha51210k512scalarE, @object
	.size	_ZN7fast_io7details6sha51210k512scalarE, 512
_ZN7fast_io7details6sha51210k512scalarE:
	.dword	-1973867731355612462
	.dword	-1171420211273849373
	.dword	1135362057144423861
	.dword	2597628984639134821
	.dword	3308224258029322869
	.dword	5365058923640841347
	.dword	6679025012923562964
	.dword	8573033837759648693
	.dword	-7476448914759557205
	.dword	-6327057829258317296
	.dword	-5763719355590565569
	.dword	-4658551843659510044
	.dword	-4116276920077217854
	.dword	-3051310485924567259
	.dword	489312712824947311
	.dword	1452737877330783856
	.dword	2861767655752347644
	.dword	3322285676063803686
	.dword	5560940570517711597
	.dword	5996557281743188959
	.dword	7280758554555802590
	.dword	8532644243296465576
	.dword	-9096487096722542874
	.dword	-7894198246740708037
	.dword	-6719396339535248540
	.dword	-6333637450476146687
	.dword	-4446306890439682159
	.dword	-4076793802049405392
	.dword	-3345356375505022440
	.dword	-2983346525034927856
	.dword	-860691631967231958
	.dword	1182934255886127544
	.dword	1847814050463011016
	.dword	2177327727835720531
	.dword	2830643537854262169
	.dword	3796741975233480872
	.dword	4115178125766777443
	.dword	5681478168544905931
	.dword	6601373596472566643
	.dword	7507060721942968483
	.dword	8399075790359081724
	.dword	8693463985226723168
	.dword	-8878714635349349518
	.dword	-8302665154208450068
	.dword	-8016688836872298968
	.dword	-6606660893046293015
	.dword	-4685533653050689259
	.dword	-4147400797238176981
	.dword	-3880063495543823972
	.dword	-3348786107499101689
	.dword	-1523767162380948706
	.dword	-757361751448694408
	.dword	500013540394364858
	.dword	748580250866718886
	.dword	1242879168328830382
	.dword	1977374033974150939
	.dword	2944078676154940804
	.dword	3659926193048069267
	.dword	4368137639120453308
	.dword	4836135668995329356
	.dword	5532061633213252278
	.dword	6448918945643986474
	.dword	6902733635092675308
	.dword	7801388544844847127


	.ident	"GCC: (GNU) 15.0.0 20240727 (experimental)"
	.section	.note.GNU-stack,"",@progbits
