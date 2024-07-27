	.text
	.file	"sha512.cc"
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z14sha512_invokerPmPKSt4byteS2_
.LCPI0_0:
	.dword	1                               # 0x1
	.dword	19                              # 0x13
.LCPI0_1:
	.dword	63                              # 0x3f
	.dword	45                              # 0x2d
.LCPI0_2:
	.dword	8                               # 0x8
	.dword	61                              # 0x3d
.LCPI0_3:
	.dword	56                              # 0x38
	.dword	3                               # 0x3
.LCPI0_4:
	.dword	7                               # 0x7
	.dword	6                               # 0x6
	.text
	.globl	_Z14sha512_invokerPmPKSt4byteS2_
	.p2align	5
	.type	_Z14sha512_invokerPmPKSt4byteS2_,@function
_Z14sha512_invokerPmPKSt4byteS2_:       # @_Z14sha512_invokerPmPKSt4byteS2_
# %bb.0:
	addi.d	$sp, $sp, -448
	st.d	$ra, $sp, 440                   # 8-byte Folded Spill
	st.d	$fp, $sp, 432                   # 8-byte Folded Spill
	st.d	$s0, $sp, 424                   # 8-byte Folded Spill
	st.d	$s1, $sp, 416                   # 8-byte Folded Spill
	st.d	$s2, $sp, 408                   # 8-byte Folded Spill
	st.d	$s3, $sp, 400                   # 8-byte Folded Spill
	st.d	$s4, $sp, 392                   # 8-byte Folded Spill
	st.d	$s5, $sp, 384                   # 8-byte Folded Spill
	st.d	$s6, $sp, 376                   # 8-byte Folded Spill
	st.d	$s7, $sp, 368                   # 8-byte Folded Spill
	st.d	$s8, $sp, 360                   # 8-byte Folded Spill
	st.d	$a2, $sp, 144                   # 8-byte Folded Spill
	beq	$a1, $a2, .LBB0_6
# %bb.1:
	move	$t6, $a1
	ld.d	$a3, $a0, 56
	ld.d	$a6, $a0, 48
	ld.d	$t0, $a0, 40
	ld.d	$t3, $a0, 32
	ld.d	$a7, $a0, 24
	ld.d	$t1, $a0, 16
	ld.d	$t4, $a0, 8
	st.d	$a0, $sp, 8                     # 8-byte Folded Spill
	ld.d	$t5, $a0, 0
	lu12i.w	$a0, -167286
	ori	$a0, $a0, 3618
	lu32i.d	$a0, -381032
	lu52i.d	$a0, $a0, 1064
	st.d	$a0, $sp, 136                   # 8-byte Folded Spill
	lu12i.w	$a0, 147190
	ori	$a0, $a0, 1485
	lu32i.d	$a0, 476305
	lu52i.d	$a0, $a0, 1811
	st.d	$a0, $sp, 128                   # 8-byte Folded Spill
	lu12i.w	$a0, -80685
	ori	$a0, $a0, 2863
	lu32i.d	$a0, 64463
	lu52i.d	$a0, $a0, -1188
	st.d	$a0, $sp, 120                   # 8-byte Folded Spill
	lu12i.w	$a0, -517987
	ori	$a0, $a0, 3004
	lu32i.d	$a0, 383909
	lu52i.d	$a0, $a0, -357
	st.d	$a0, $sp, 112                   # 8-byte Folded Spill
	lu12i.w	$a0, -52085
	ori	$a0, $a0, 1336
	lu32i.d	$a0, 442971
	lu52i.d	$a0, $a0, 917
	st.d	$a0, $sp, 104                   # 8-byte Folded Spill
	lu12i.w	$a0, -303011
	ori	$a0, $a0, 25
	lu32i.d	$a0, 70129
	lu52i.d	$a0, $a0, 1439
	st.d	$a0, $sp, 96                    # 8-byte Folded Spill
	lu12i.w	$a0, -331372
	ori	$a0, $a0, 3995
	lu32i.d	$a0, -32092
	lu52i.d	$a0, $a0, -1757
	st.d	$a0, $sp, 88                    # 8-byte Folded Spill
	lu12i.w	$a0, -153896
	ori	$a0, $a0, 280
	lu32i.d	$a0, -237867
	lu52i.d	$a0, $a0, -1359
	st.d	$a0, $sp, 80                    # 8-byte Folded Spill
	lu12i.w	$a0, -380880
	ori	$a0, $a0, 578
	lu32i.d	$a0, 502424
	lu52i.d	$a0, $a0, -640
	st.d	$a0, $sp, 72                    # 8-byte Folded Spill
	lu12i.w	$a0, 284422
	ori	$a0, $a0, 4030
	lu32i.d	$a0, 219905
	lu52i.d	$a0, $a0, 296
	st.d	$a0, $sp, 64                    # 8-byte Folded Spill
	lu12i.w	$a0, 323147
	ori	$a0, $a0, 652
	lu32i.d	$a0, 99774
	lu52i.d	$a0, $a0, 579
	st.d	$a0, $sp, 56                    # 8-byte Folded Spill
	lu12i.w	$a0, -172037
	ori	$a0, $a0, 1250
	lu32i.d	$a0, -229949
	lu52i.d	$a0, $a0, 1360
	st.d	$a0, $sp, 48                    # 8-byte Folded Spill
	lu12i.w	$a0, -55368
	ori	$a0, $a0, 2415
	lu32i.d	$a0, -107148
	lu52i.d	$a0, $a0, 1835
	st.d	$a0, $sp, 40                    # 8-byte Folded Spill
	lu12i.w	$a0, 242025
	ori	$a0, $a0, 1713
	lu32i.d	$a0, -85506
	lu52i.d	$a0, $a0, -2035
	st.d	$a0, $sp, 32                    # 8-byte Folded Spill
	lu12i.w	$a0, 154737
	ori	$a0, $a0, 565
	lu32i.d	$a0, -260441
	lu52i.d	$a0, $a0, -1603
	st.d	$a0, $sp, 24                    # 8-byte Folded Spill
	lu12i.w	$a0, -199022
	ori	$a0, $a0, 1684
	lu32i.d	$a0, -265868
	lu52i.d	$a0, $a0, -999
	st.d	$a0, $sp, 16                    # 8-byte Folded Spill
	.p2align	4, , 16
.LBB0_2:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	ld.d	$a0, $t6, 0
	move	$s0, $zero
	revb.d	$s2, $a0
	rotri.d	$a0, $t3, 14
	rotri.d	$a1, $t3, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t3, 41
	xor	$a0, $a0, $a1
	andn	$a1, $a6, $t3
	and	$a2, $t0, $t3
	or	$a1, $a2, $a1
	st.d	$a3, $sp, 216                   # 8-byte Folded Spill
	add.d	$a0, $a0, $a3
	add.d	$a0, $a0, $a1
	add.d	$a0, $a0, $s2
	ld.d	$a1, $sp, 136                   # 8-byte Folded Reload
	add.d	$a0, $a0, $a1
	rotri.d	$a1, $t5, 28
	rotri.d	$a2, $t5, 34
	xor	$a1, $a1, $a2
	rotri.d	$a2, $t5, 39
	xor	$a1, $a1, $a2
	xor	$a2, $t4, $t5
	andn	$a3, $t4, $a2
	and	$a4, $t1, $a2
	or	$a3, $a4, $a3
	ld.d	$a5, $t6, 8
	add.d	$a1, $a3, $a1
	st.d	$a7, $sp, 176                   # 8-byte Folded Spill
	add.d	$a4, $a0, $a7
	add.d	$a0, $a1, $a0
	revb.d	$a7, $a5
	rotri.d	$a1, $a4, 14
	rotri.d	$a3, $a4, 18
	xor	$a1, $a1, $a3
	rotri.d	$a3, $a4, 41
	xor	$a1, $a1, $a3
	andn	$a3, $t0, $a4
	and	$a5, $t3, $a4
	or	$a3, $a5, $a3
	st.d	$a6, $sp, 208                   # 8-byte Folded Spill
	st.d	$a7, $sp, 296                   # 8-byte Folded Spill
	add.d	$a5, $a6, $a7
	add.d	$a3, $a5, $a3
	add.d	$a1, $a3, $a1
	ld.d	$a3, $sp, 128                   # 8-byte Folded Reload
	add.d	$a1, $a1, $a3
	rotri.d	$a3, $a0, 28
	rotri.d	$a5, $a0, 34
	xor	$a3, $a3, $a5
	rotri.d	$a5, $a0, 39
	xor	$a3, $a3, $a5
	xor	$a6, $a0, $t5
	and	$a2, $a6, $a2
	xor	$a2, $a2, $t5
	ld.d	$a7, $t6, 16
	add.d	$a2, $a3, $a2
	st.d	$t1, $sp, 168                   # 8-byte Folded Spill
	add.d	$a5, $a1, $t1
	add.d	$a1, $a2, $a1
	revb.d	$t1, $a7
	rotri.d	$a2, $a5, 14
	rotri.d	$a3, $a5, 18
	xor	$a2, $a2, $a3
	rotri.d	$a3, $a5, 41
	xor	$a2, $a2, $a3
	andn	$a3, $t3, $a5
	and	$a7, $a4, $a5
	or	$a3, $a7, $a3
	st.d	$t0, $sp, 192                   # 8-byte Folded Spill
	st.d	$t1, $sp, 272                   # 8-byte Folded Spill
	add.d	$a7, $t0, $t1
	add.d	$a3, $a7, $a3
	add.d	$a2, $a3, $a2
	ld.d	$a3, $sp, 120                   # 8-byte Folded Reload
	add.d	$a2, $a2, $a3
	rotri.d	$a3, $a1, 28
	rotri.d	$a7, $a1, 34
	xor	$a3, $a3, $a7
	rotri.d	$a7, $a1, 39
	xor	$a3, $a3, $a7
	xor	$a7, $a1, $a0
	and	$a6, $a7, $a6
	xor	$a6, $a6, $a0
	ld.d	$t0, $t6, 24
	add.d	$a3, $a3, $a6
	st.d	$t4, $sp, 160                   # 8-byte Folded Spill
	add.d	$a6, $a2, $t4
	add.d	$a2, $a3, $a2
	revb.d	$t2, $t0
	rotri.d	$a3, $a6, 14
	rotri.d	$t0, $a6, 18
	xor	$a3, $a3, $t0
	rotri.d	$t0, $a6, 41
	xor	$a3, $a3, $t0
	andn	$t0, $a4, $a6
	and	$t1, $a5, $a6
	or	$t0, $t1, $t0
	st.d	$t3, $sp, 184                   # 8-byte Folded Spill
	st.d	$t2, $sp, 304                   # 8-byte Folded Spill
	add.d	$t1, $t3, $t2
	add.d	$t0, $t1, $t0
	add.d	$a3, $t0, $a3
	ld.d	$t0, $sp, 112                   # 8-byte Folded Reload
	add.d	$a3, $a3, $t0
	rotri.d	$t0, $a2, 28
	rotri.d	$t1, $a2, 34
	xor	$t0, $t0, $t1
	rotri.d	$t1, $a2, 39
	xor	$t0, $t0, $t1
	xor	$t1, $a2, $a1
	and	$a7, $t1, $a7
	xor	$a7, $a7, $a1
	ld.d	$t2, $t6, 32
	add.d	$t0, $t0, $a7
	st.d	$t5, $sp, 152                   # 8-byte Folded Spill
	add.d	$a7, $a3, $t5
	add.d	$a3, $t0, $a3
	revb.d	$t4, $t2
	rotri.d	$t0, $a7, 14
	rotri.d	$t2, $a7, 18
	xor	$t0, $t0, $t2
	rotri.d	$t2, $a7, 41
	xor	$t0, $t0, $t2
	andn	$t2, $a5, $a7
	and	$t3, $a6, $a7
	or	$t2, $t3, $t2
	st.d	$t4, $sp, 288                   # 8-byte Folded Spill
	add.d	$a4, $a4, $t4
	add.d	$a4, $a4, $t2
	add.d	$a4, $a4, $t0
	ld.d	$t0, $sp, 104                   # 8-byte Folded Reload
	add.d	$a4, $a4, $t0
	rotri.d	$t0, $a3, 28
	rotri.d	$t2, $a3, 34
	xor	$t0, $t0, $t2
	rotri.d	$t2, $a3, 39
	xor	$t0, $t0, $t2
	xor	$t2, $a3, $a2
	and	$t1, $t2, $t1
	xor	$t1, $t1, $a2
	ld.d	$t3, $t6, 40
	add.d	$t1, $t0, $t1
	add.d	$t0, $a4, $a0
	add.d	$a0, $t1, $a4
	revb.d	$t4, $t3
	rotri.d	$a4, $t0, 14
	rotri.d	$t1, $t0, 18
	xor	$a4, $a4, $t1
	rotri.d	$t1, $t0, 41
	xor	$a4, $a4, $t1
	andn	$t1, $a6, $t0
	and	$t3, $a7, $t0
	or	$t1, $t3, $t1
	st.d	$t4, $sp, 280                   # 8-byte Folded Spill
	add.d	$a5, $t4, $a5
	add.d	$a5, $a5, $t1
	add.d	$a4, $a5, $a4
	ld.d	$a5, $sp, 96                    # 8-byte Folded Reload
	add.d	$a4, $a4, $a5
	rotri.d	$a5, $a0, 28
	rotri.d	$t1, $a0, 34
	xor	$a5, $a5, $t1
	rotri.d	$t1, $a0, 39
	xor	$a5, $a5, $t1
	xor	$t1, $a0, $a3
	and	$t2, $t1, $t2
	xor	$t2, $t2, $a3
	ld.d	$t3, $t6, 48
	add.d	$t2, $a5, $t2
	add.d	$a5, $a4, $a1
	add.d	$a1, $t2, $a4
	revb.d	$t4, $t3
	rotri.d	$a4, $a5, 14
	rotri.d	$t2, $a5, 18
	xor	$a4, $a4, $t2
	rotri.d	$t2, $a5, 41
	xor	$a4, $a4, $t2
	andn	$t2, $a7, $a5
	and	$t3, $t0, $a5
	or	$t2, $t3, $t2
	st.d	$t4, $sp, 264                   # 8-byte Folded Spill
	add.d	$a6, $t4, $a6
	add.d	$a6, $a6, $t2
	add.d	$a4, $a6, $a4
	ld.d	$a6, $sp, 88                    # 8-byte Folded Reload
	add.d	$a4, $a4, $a6
	rotri.d	$a6, $a1, 28
	rotri.d	$t2, $a1, 34
	xor	$a6, $a6, $t2
	rotri.d	$t2, $a1, 39
	xor	$a6, $a6, $t2
	xor	$t2, $a1, $a0
	and	$t1, $t2, $t1
	xor	$t1, $t1, $a0
	ld.d	$t3, $t6, 56
	add.d	$t1, $a6, $t1
	add.d	$a6, $a4, $a2
	add.d	$a4, $t1, $a4
	revb.d	$s1, $t3
	rotri.d	$a2, $a6, 14
	rotri.d	$t1, $a6, 18
	xor	$a2, $a2, $t1
	rotri.d	$t1, $a6, 41
	xor	$a2, $a2, $t1
	andn	$t1, $t0, $a6
	and	$t3, $a5, $a6
	or	$t1, $t3, $t1
	add.d	$a7, $s1, $a7
	add.d	$a7, $a7, $t1
	add.d	$a2, $a7, $a2
	ld.d	$a7, $sp, 80                    # 8-byte Folded Reload
	add.d	$a2, $a2, $a7
	rotri.d	$a7, $a4, 28
	rotri.d	$t1, $a4, 34
	xor	$a7, $a7, $t1
	rotri.d	$t1, $a4, 39
	xor	$a7, $a7, $t1
	xor	$t3, $a4, $a1
	and	$t1, $t3, $t2
	xor	$t1, $t1, $a1
	ld.d	$t2, $t6, 64
	add.d	$a7, $a7, $t1
	add.d	$t1, $a2, $a3
	add.d	$a3, $a7, $a2
	revb.d	$s6, $t2
	rotri.d	$a2, $t1, 14
	rotri.d	$a7, $t1, 18
	xor	$a2, $a2, $a7
	rotri.d	$a7, $t1, 41
	xor	$a2, $a2, $a7
	andn	$a7, $a5, $t1
	and	$t2, $a6, $t1
	or	$a7, $t2, $a7
	add.d	$t0, $s6, $t0
	add.d	$a7, $t0, $a7
	add.d	$a2, $a7, $a2
	ld.d	$a7, $sp, 72                    # 8-byte Folded Reload
	add.d	$a2, $a2, $a7
	rotri.d	$a7, $a3, 28
	rotri.d	$t0, $a3, 34
	xor	$a7, $a7, $t0
	rotri.d	$t0, $a3, 39
	xor	$a7, $a7, $t0
	xor	$t0, $a3, $a4
	and	$t2, $t0, $t3
	xor	$t2, $t2, $a4
	ld.d	$t3, $t6, 72
	add.d	$a7, $a7, $t2
	add.d	$t2, $a2, $a0
	add.d	$a0, $a7, $a2
	revb.d	$s4, $t3
	rotri.d	$a2, $t2, 14
	rotri.d	$a7, $t2, 18
	xor	$a2, $a2, $a7
	rotri.d	$a7, $t2, 41
	xor	$a2, $a2, $a7
	andn	$a7, $a6, $t2
	and	$t3, $t1, $t2
	or	$a7, $t3, $a7
	add.d	$a5, $s4, $a5
	add.d	$a5, $a5, $a7
	add.d	$a2, $a5, $a2
	ld.d	$a5, $sp, 64                    # 8-byte Folded Reload
	add.d	$a2, $a2, $a5
	rotri.d	$a5, $a0, 28
	rotri.d	$a7, $a0, 34
	xor	$a5, $a5, $a7
	rotri.d	$a7, $a0, 39
	xor	$a5, $a5, $a7
	xor	$a7, $a0, $a3
	and	$t0, $a7, $t0
	xor	$t0, $t0, $a3
	ld.d	$t4, $t6, 80
	add.d	$a5, $a5, $t0
	add.d	$t3, $a2, $a1
	add.d	$a5, $a5, $a2
	revb.d	$a2, $t4
	rotri.d	$a1, $t3, 14
	rotri.d	$t0, $t3, 18
	xor	$a1, $a1, $t0
	rotri.d	$t0, $t3, 41
	xor	$a1, $a1, $t0
	andn	$t0, $t1, $t3
	and	$t4, $t2, $t3
	or	$t0, $t4, $t0
	add.d	$a6, $a2, $a6
	add.d	$a6, $a6, $t0
	add.d	$a1, $a6, $a1
	ld.d	$a6, $sp, 56                    # 8-byte Folded Reload
	add.d	$a1, $a1, $a6
	rotri.d	$a6, $a5, 28
	rotri.d	$t0, $a5, 34
	xor	$a6, $a6, $t0
	rotri.d	$t0, $a5, 39
	xor	$a6, $a6, $t0
	xor	$t0, $a5, $a0
	and	$a7, $t0, $a7
	xor	$a7, $a7, $a0
	ld.d	$t5, $t6, 88
	add.d	$a6, $a6, $a7
	add.d	$t4, $a1, $a4
	add.d	$a6, $a6, $a1
	revb.d	$a7, $t5
	rotri.d	$a1, $t4, 14
	rotri.d	$a4, $t4, 18
	xor	$a1, $a1, $a4
	rotri.d	$a4, $t4, 41
	xor	$a1, $a1, $a4
	andn	$a4, $t2, $t4
	and	$t5, $t3, $t4
	or	$a4, $t5, $a4
	add.d	$t1, $a7, $t1
	add.d	$a4, $t1, $a4
	add.d	$a1, $a4, $a1
	ld.d	$a4, $sp, 48                    # 8-byte Folded Reload
	add.d	$a1, $a1, $a4
	rotri.d	$a4, $a6, 28
	rotri.d	$t1, $a6, 34
	xor	$a4, $a4, $t1
	rotri.d	$t1, $a6, 39
	xor	$a4, $a4, $t1
	xor	$t1, $a6, $a5
	and	$t0, $t1, $t0
	xor	$t0, $t0, $a5
	ld.d	$t7, $t6, 96
	add.d	$a4, $a4, $t0
	add.d	$t5, $a1, $a3
	add.d	$t0, $a4, $a1
	revb.d	$s8, $t7
	rotri.d	$a1, $t5, 14
	rotri.d	$a3, $t5, 18
	xor	$a1, $a1, $a3
	rotri.d	$a3, $t5, 41
	xor	$a1, $a1, $a3
	andn	$a3, $t3, $t5
	and	$a4, $t4, $t5
	or	$a3, $a4, $a3
	add.d	$a4, $s8, $t2
	add.d	$a3, $a4, $a3
	add.d	$a1, $a3, $a1
	ld.d	$a3, $sp, 40                    # 8-byte Folded Reload
	add.d	$a3, $a1, $a3
	rotri.d	$a1, $t0, 28
	rotri.d	$a4, $t0, 34
	xor	$a1, $a1, $a4
	rotri.d	$a4, $t0, 39
	xor	$a1, $a1, $a4
	xor	$t2, $t0, $a6
	and	$a4, $t2, $t1
	xor	$a4, $a4, $a6
	ld.d	$t1, $t6, 104
	add.d	$a4, $a1, $a4
	add.d	$a1, $a3, $a0
	add.d	$a4, $a4, $a3
	revb.d	$s5, $t1
	rotri.d	$a0, $a1, 14
	rotri.d	$a3, $a1, 18
	xor	$a0, $a0, $a3
	rotri.d	$a3, $a1, 41
	xor	$a0, $a0, $a3
	andn	$a3, $t4, $a1
	and	$t1, $t5, $a1
	or	$a3, $t1, $a3
	add.d	$t1, $s5, $t3
	add.d	$a3, $t1, $a3
	add.d	$a0, $a3, $a0
	ld.d	$a3, $sp, 32                    # 8-byte Folded Reload
	add.d	$a0, $a0, $a3
	rotri.d	$a3, $a4, 28
	rotri.d	$t1, $a4, 34
	xor	$a3, $a3, $t1
	rotri.d	$t1, $a4, 39
	xor	$a3, $a3, $t1
	xor	$t1, $a4, $t0
	and	$t2, $t1, $t2
	xor	$t2, $t2, $t0
	ld.d	$t3, $t6, 112
	add.d	$t2, $a3, $t2
	add.d	$a3, $a0, $a5
	add.d	$a5, $t2, $a0
	revb.d	$s3, $t3
	rotri.d	$a0, $a3, 14
	rotri.d	$t2, $a3, 18
	xor	$a0, $a0, $t2
	rotri.d	$t2, $a3, 41
	xor	$a0, $a0, $t2
	andn	$t2, $t5, $a3
	and	$t3, $a1, $a3
	or	$t2, $t3, $t2
	add.d	$t3, $s3, $t4
	add.d	$t2, $t3, $t2
	add.d	$a0, $t2, $a0
	ld.d	$t2, $sp, 24                    # 8-byte Folded Reload
	add.d	$a0, $a0, $t2
	rotri.d	$t2, $a5, 28
	rotri.d	$t3, $a5, 34
	xor	$t2, $t2, $t3
	rotri.d	$t3, $a5, 39
	xor	$t2, $t2, $t3
	xor	$t3, $a5, $a4
	and	$t1, $t3, $t1
	xor	$t1, $t1, $a4
	add.d	$t1, $t2, $t1
	add.d	$t2, $a0, $a6
	add.d	$t1, $t1, $a0
	st.d	$t6, $sp, 200                   # 8-byte Folded Spill
	ld.d	$a0, $t6, 120
	rotri.d	$a6, $t2, 14
	rotri.d	$t4, $t2, 18
	xor	$a6, $a6, $t4
	rotri.d	$t4, $t2, 41
	xor	$a6, $a6, $t4
	andn	$t4, $a1, $t2
	and	$t7, $a3, $t2
	or	$t4, $t7, $t4
	revb.d	$fp, $a0
	add.d	$a0, $fp, $t5
	add.d	$a0, $a0, $t4
	add.d	$a6, $a0, $a6
	rotri.d	$a0, $t1, 28
	rotri.d	$t4, $t1, 34
	xor	$a0, $a0, $t4
	rotri.d	$t4, $t1, 39
	xor	$t4, $a0, $t4
	xor	$a0, $t1, $a5
	and	$t3, $a0, $t3
	xor	$t3, $t3, $a5
	add.d	$t3, $t4, $t3
	ld.d	$t4, $sp, 16                    # 8-byte Folded Reload
	add.d	$a6, $a6, $t4
	add.d	$ra, $a6, $t0
	add.d	$a6, $t3, $a6
	.p2align	4, , 16
.LBB0_3:                                #   Parent Loop BB0_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	st.d	$fp, $sp, 256                   # 8-byte Folded Spill
	st.d	$s0, $sp, 312                   # 8-byte Folded Spill
	ld.d	$t6, $sp, 296                   # 8-byte Folded Reload
	rotri.d	$t0, $t6, 1
	rotri.d	$t3, $t6, 8
	xor	$t0, $t0, $t3
	srli.d	$t3, $t6, 7
	xor	$t0, $t0, $t3
	rotri.d	$t3, $s3, 19
	rotri.d	$t4, $s3, 61
	xor	$t3, $t3, $t4
	srli.d	$t4, $s3, 6
	xor	$t3, $t3, $t4
	st.d	$t3, $sp, 224                   # 8-byte Folded Spill
	add.d	$t3, $s2, $s4
	add.d	$t8, $t3, $t0
	rotri.d	$t0, $ra, 14
	rotri.d	$t3, $ra, 18
	xor	$t0, $t0, $t3
	rotri.d	$t3, $ra, 41
	xor	$t0, $t0, $t3
	andn	$t3, $a3, $ra
	and	$t4, $t2, $ra
	or	$t3, $t4, $t3
	add.d	$a1, $a1, $t0
	add.d	$t5, $a1, $t3
	rotri.d	$a1, $a6, 28
	rotri.d	$t0, $a6, 34
	xor	$a1, $a1, $t0
	rotri.d	$t0, $a6, 39
	xor	$a1, $a1, $t0
	xor	$t0, $t1, $a6
	and	$a0, $a0, $t0
	xor	$a0, $a0, $t1
	add.d	$t4, $a0, $a1
	ld.d	$t7, $sp, 272                   # 8-byte Folded Reload
	rotri.d	$a0, $t7, 1
	rotri.d	$a1, $t7, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $t7, 7
	xor	$a1, $a0, $a1
	rotri.d	$a0, $fp, 19
	rotri.d	$t3, $fp, 61
	xor	$a0, $a0, $t3
	srli.d	$t3, $fp, 6
	xor	$t3, $a0, $t3
	pcalau12i	$a0, %pc_hi20(_ZN7fast_io7details6sha51210k512scalarE)
	addi.d	$a0, $a0, %pc_lo12(_ZN7fast_io7details6sha51210k512scalarE)
	add.d	$t6, $t6, $a2
	ld.d	$s0, $sp, 312                   # 8-byte Folded Reload
	add.d	$s0, $a0, $s0
	ld.d	$s2, $s0, 8
	add.d	$a1, $t6, $a1
	add.d	$fp, $a1, $t3
	add.d	$a1, $fp, $a3
	add.d	$t3, $a1, $s2
	ld.d	$s7, $sp, 304                   # 8-byte Folded Reload
	rotri.d	$a1, $s7, 1
	rotri.d	$a3, $s7, 8
	xor	$a1, $a1, $a3
	srli.d	$a3, $s7, 7
	xor	$a1, $a1, $a3
	add.d	$a3, $t7, $a7
	add.d	$t6, $a3, $a1
	ld.d	$t7, $sp, 288                   # 8-byte Folded Reload
	rotri.d	$a1, $t7, 1
	rotri.d	$a3, $t7, 8
	xor	$a1, $a1, $a3
	srli.d	$a3, $t7, 7
	xor	$a1, $a1, $a3
	add.d	$a3, $s7, $s8
	add.d	$a1, $a3, $a1
	ld.d	$s7, $sp, 280                   # 8-byte Folded Reload
	rotri.d	$a3, $s7, 1
	rotri.d	$s2, $s7, 8
	xor	$a3, $a3, $s2
	srli.d	$s2, $s7, 7
	xor	$a3, $a3, $s2
	add.d	$s2, $t7, $s5
	add.d	$a3, $s2, $a3
	st.d	$a3, $sp, 232                   # 8-byte Folded Spill
	ld.d	$t7, $sp, 264                   # 8-byte Folded Reload
	rotri.d	$a3, $t7, 1
	rotri.d	$s2, $t7, 8
	xor	$a3, $a3, $s2
	srli.d	$s2, $t7, 7
	xor	$a3, $a3, $s2
	add.d	$s2, $a3, $s7
	rotri.d	$a3, $s1, 1
	rotri.d	$s7, $s1, 8
	xor	$a3, $a3, $s7
	srli.d	$s7, $s1, 7
	xor	$a3, $a3, $s7
	add.d	$a3, $a3, $t7
	st.d	$a3, $sp, 240                   # 8-byte Folded Spill
	rotri.d	$a3, $s6, 1
	rotri.d	$s7, $s6, 8
	xor	$a3, $a3, $s7
	srli.d	$s7, $s6, 7
	xor	$a3, $a3, $s7
	add.d	$a3, $a3, $s1
	st.d	$a3, $sp, 264                   # 8-byte Folded Spill
	rotri.d	$a3, $s4, 1
	rotri.d	$s1, $s4, 8
	xor	$a3, $a3, $s1
	srli.d	$s1, $s4, 7
	xor	$a3, $a3, $s1
	add.d	$a3, $a3, $s6
	st.d	$a3, $sp, 272                   # 8-byte Folded Spill
	ld.d	$a3, $sp, 312                   # 8-byte Folded Reload
	ldx.d	$a0, $a0, $a3
	ld.d	$a3, $s0, 16
	ld.d	$s7, $s0, 24
	ld.d	$s1, $s0, 32
	ld.d	$s6, $s0, 40
	ld.d	$t7, $s0, 48
	st.d	$t7, $sp, 248                   # 8-byte Folded Spill
	ld.d	$t7, $s0, 56
	st.d	$t7, $sp, 296                   # 8-byte Folded Spill
	ld.d	$t7, $s0, 64
	st.d	$t7, $sp, 280                   # 8-byte Folded Spill
	ld.d	$t7, $s0, 72
	st.d	$t7, $sp, 288                   # 8-byte Folded Spill
	st.d	$a7, $sp, 320
	pcalau12i	$a7, %pc_hi20(.LCPI0_0)
	addi.d	$a7, $a7, %pc_lo12(.LCPI0_0)
	vld	$vr0, $a7, 0
	pcalau12i	$a7, %pc_hi20(.LCPI0_1)
	addi.d	$a7, $a7, %pc_lo12(.LCPI0_1)
	vld	$vr1, $a7, 0
	pcalau12i	$a7, %pc_hi20(.LCPI0_2)
	addi.d	$a7, $a7, %pc_lo12(.LCPI0_2)
	vld	$vr2, $a7, 0
	vld	$vr6, $sp, 320
	st.d	$a2, $sp, 336
	pcalau12i	$a2, %pc_hi20(.LCPI0_3)
	addi.d	$a2, $a2, %pc_lo12(.LCPI0_3)
	vld	$vr5, $sp, 336
	vld	$vr3, $a2, 0
	pcalau12i	$a2, %pc_hi20(.LCPI0_4)
	addi.d	$a2, $a2, %pc_lo12(.LCPI0_4)
	vld	$vr4, $a2, 0
	vori.b	$vr7, $vr5, 0
	vinsgr2vr.d	$vr7, $a1, 1
	vori.b	$vr8, $vr6, 0
	vinsgr2vr.d	$vr8, $fp, 1
	vsrl.d	$vr9, $vr8, $vr0
	vsll.d	$vr10, $vr8, $vr1
	vor.v	$vr9, $vr10, $vr9
	vsrl.d	$vr10, $vr8, $vr2
	vsll.d	$vr11, $vr8, $vr3
	vor.v	$vr10, $vr11, $vr10
	vxor.v	$vr9, $vr9, $vr10
	vsrl.d	$vr8, $vr8, $vr4
	vxor.v	$vr8, $vr9, $vr8
	vadd.d	$vr7, $vr8, $vr7
	vpickve2gr.d	$a1, $vr7, 1
	st.d	$a1, $sp, 304                   # 8-byte Folded Spill
	add.d	$a1, $a1, $ra
	add.d	$a1, $a1, $s7
	add.d	$a2, $s2, $s3
	move	$t7, $fp
	vinsgr2vr.d	$vr8, $s8, 0
	vinsgr2vr.d	$vr8, $a2, 1
	ld.d	$a2, $sp, 224                   # 8-byte Folded Reload
	add.d	$s2, $t8, $a2
	add.d	$a2, $t5, $s2
	add.d	$a0, $a2, $a0
	add.d	$a7, $a0, $a4
	add.d	$a2, $t4, $a0
	rotri.d	$a0, $a7, 14
	rotri.d	$a4, $a7, 18
	xor	$a0, $a0, $a4
	rotri.d	$a4, $a7, 41
	xor	$a0, $a0, $a4
	andn	$a4, $t2, $a7
	and	$t4, $ra, $a7
	or	$a4, $t4, $a4
	add.d	$a4, $t3, $a4
	add.d	$a0, $a4, $a0
	rotri.d	$a4, $a2, 28
	rotri.d	$t3, $a2, 34
	xor	$a4, $a4, $t3
	rotri.d	$t3, $a2, 39
	xor	$a4, $a4, $t3
	xor	$t3, $a2, $a6
	and	$t0, $t3, $t0
	xor	$t0, $t0, $a6
	add.d	$t0, $a4, $t0
	add.d	$a4, $a0, $a5
	add.d	$a0, $t0, $a0
	vinsgr2vr.d	$vr9, $s4, 0
	vinsgr2vr.d	$vr9, $t6, 1
	rotri.d	$a5, $a4, 14
	rotri.d	$t0, $a4, 18
	xor	$a5, $a5, $t0
	rotri.d	$t0, $a4, 41
	xor	$a5, $a5, $t0
	andn	$t0, $ra, $a4
	and	$t4, $a7, $a4
	or	$t0, $t4, $t0
	vinsgr2vr.d	$vr5, $s2, 1
	vsrl.d	$vr10, $vr5, $vr0
	vsll.d	$vr11, $vr5, $vr1
	vor.v	$vr10, $vr11, $vr10
	vsrl.d	$vr11, $vr5, $vr2
	vsll.d	$vr12, $vr5, $vr3
	vor.v	$vr11, $vr12, $vr11
	vxor.v	$vr10, $vr10, $vr11
	vsrl.d	$vr5, $vr5, $vr4
	vxor.v	$vr5, $vr10, $vr5
	vadd.d	$vr5, $vr5, $vr9
	vpickve2gr.d	$ra, $vr5, 1
	add.d	$t2, $ra, $t2
	add.d	$a3, $t2, $a3
	add.d	$a3, $a3, $t0
	add.d	$a3, $a3, $a5
	rotri.d	$a5, $a0, 28
	rotri.d	$t0, $a0, 34
	xor	$a5, $a5, $t0
	rotri.d	$t0, $a0, 39
	xor	$a5, $a5, $t0
	xor	$t4, $a0, $a2
	and	$t0, $t4, $t3
	xor	$t0, $t0, $a2
	add.d	$a5, $a5, $t0
	add.d	$t0, $a3, $t1
	add.d	$t2, $a5, $a3
	rotri.d	$a3, $t0, 14
	rotri.d	$a5, $t0, 18
	xor	$a3, $a3, $a5
	rotri.d	$a5, $t0, 41
	xor	$a3, $a3, $a5
	andn	$a5, $a7, $t0
	and	$t1, $a4, $t0
	or	$a5, $t1, $a5
	add.d	$a1, $a1, $a5
	add.d	$a1, $a1, $a3
	rotri.d	$a3, $t2, 28
	rotri.d	$a5, $t2, 34
	xor	$a3, $a3, $a5
	rotri.d	$a5, $t2, 39
	xor	$a3, $a3, $a5
	xor	$a5, $t2, $a0
	and	$t1, $a5, $t4
	xor	$t1, $t1, $a0
	add.d	$a3, $a3, $t1
	add.d	$s4, $a1, $a6
	add.d	$t1, $a3, $a1
	vori.b	$vr9, $vr5, 0
	vinsgr2vr.d	$vr9, $s8, 0
	ld.d	$a1, $sp, 232                   # 8-byte Folded Reload
	vinsgr2vr.d	$vr6, $a1, 1
	rotri.d	$a1, $s4, 14
	rotri.d	$a3, $s4, 18
	xor	$a1, $a1, $a3
	rotri.d	$a3, $s4, 41
	xor	$a1, $a1, $a3
	andn	$a3, $a4, $s4
	and	$a6, $t0, $s4
	or	$a3, $a6, $a3
	vsrl.d	$vr10, $vr9, $vr0
	vsll.d	$vr11, $vr9, $vr1
	vor.v	$vr10, $vr11, $vr10
	vsrl.d	$vr11, $vr9, $vr2
	vsll.d	$vr12, $vr9, $vr3
	vor.v	$vr11, $vr12, $vr11
	vxor.v	$vr10, $vr10, $vr11
	vsrl.d	$vr9, $vr9, $vr4
	vxor.v	$vr9, $vr10, $vr9
	vadd.d	$vr6, $vr9, $vr6
	vpickve2gr.d	$s8, $vr6, 1
	add.d	$a6, $a7, $s8
	add.d	$a6, $a6, $s1
	add.d	$a3, $a6, $a3
	add.d	$a1, $a3, $a1
	rotri.d	$a3, $t1, 28
	rotri.d	$a6, $t1, 34
	xor	$a3, $a3, $a6
	rotri.d	$a6, $t1, 39
	xor	$a3, $a3, $a6
	xor	$a6, $t1, $t2
	and	$a5, $a6, $a5
	xor	$a5, $a5, $t2
	add.d	$a3, $a3, $a5
	add.d	$a5, $a1, $a2
	add.d	$a2, $a3, $a1
	rotri.d	$a1, $a5, 14
	rotri.d	$a3, $a5, 18
	xor	$a1, $a1, $a3
	rotri.d	$a3, $a5, 41
	xor	$a1, $a1, $a3
	andn	$a3, $t0, $a5
	and	$a7, $s4, $a5
	or	$a3, $a7, $a3
	vpickve2gr.d	$a7, $vr7, 0
	vinsgr2vr.d	$vr7, $s5, 0
	vsrl.d	$vr9, $vr7, $vr0
	vsll.d	$vr10, $vr7, $vr1
	vor.v	$vr9, $vr10, $vr9
	vsrl.d	$vr10, $vr7, $vr2
	vsll.d	$vr11, $vr7, $vr3
	vor.v	$vr10, $vr11, $vr10
	vxor.v	$vr9, $vr9, $vr10
	vsrl.d	$vr7, $vr7, $vr4
	vxor.v	$vr7, $vr9, $vr7
	vadd.d	$vr8, $vr7, $vr8
	vpickve2gr.d	$s7, $vr8, 1
	add.d	$t3, $s6, $s7
	add.d	$a4, $t3, $a4
	add.d	$a3, $a4, $a3
	add.d	$a3, $a3, $a1
	rotri.d	$a1, $a2, 28
	rotri.d	$a4, $a2, 34
	xor	$a1, $a1, $a4
	rotri.d	$a4, $a2, 39
	xor	$a4, $a1, $a4
	xor	$a1, $a2, $t1
	and	$a6, $a1, $a6
	xor	$a6, $a6, $t1
	add.d	$a4, $a4, $a6
	ld.d	$t5, $sp, 256                   # 8-byte Folded Reload
	ld.d	$a6, $sp, 240                   # 8-byte Folded Reload
	add.d	$t3, $a6, $t5
	add.d	$a6, $a3, $a0
	add.d	$a4, $a4, $a3
	vori.b	$vr7, $vr6, 0
	vinsgr2vr.d	$vr7, $s3, 0
	vsrl.d	$vr9, $vr7, $vr0
	vsll.d	$vr10, $vr7, $vr1
	vor.v	$vr9, $vr10, $vr9
	vsrl.d	$vr10, $vr7, $vr2
	vsll.d	$vr11, $vr7, $vr3
	vor.v	$vr10, $vr11, $vr10
	vxor.v	$vr9, $vr9, $vr10
	vsrl.d	$vr7, $vr7, $vr4
	vxor.v	$vr7, $vr9, $vr7
	vinsgr2vr.d	$vr9, $s5, 0
	vinsgr2vr.d	$vr9, $t3, 1
	vadd.d	$vr7, $vr7, $vr9
	rotri.d	$a0, $a6, 14
	rotri.d	$a3, $a6, 18
	xor	$a0, $a0, $a3
	rotri.d	$a3, $a6, 41
	xor	$a0, $a0, $a3
	andn	$a3, $s4, $a6
	and	$t3, $a5, $a6
	or	$a3, $t3, $a3
	vpickve2gr.d	$s5, $vr7, 1
	ld.d	$t3, $sp, 248                   # 8-byte Folded Reload
	add.d	$t3, $t3, $s5
	add.d	$t0, $t3, $t0
	vpickve2gr.d	$t4, $vr5, 0
	add.d	$a3, $t0, $a3
	add.d	$a0, $a3, $a0
	rotri.d	$a3, $a4, 28
	rotri.d	$t0, $a4, 34
	xor	$a3, $a3, $t0
	rotri.d	$t0, $a4, 39
	xor	$a3, $a3, $t0
	xor	$t0, $a4, $a2
	and	$a1, $t0, $a1
	xor	$a1, $a1, $a2
	add.d	$a1, $a3, $a1
	vpickve2gr.d	$a3, $vr8, 0
	st.d	$a3, $sp, 248                   # 8-byte Folded Spill
	add.d	$t3, $a0, $t2
	add.d	$t2, $a1, $a0
	ld.d	$a0, $sp, 264                   # 8-byte Folded Reload
	add.d	$a0, $a0, $s2
	vinsgr2vr.d	$vr8, $t5, 0
	move	$t6, $t5
	vsrl.d	$vr5, $vr8, $vr0
	vsll.d	$vr9, $vr8, $vr1
	vor.v	$vr5, $vr9, $vr5
	vsrl.d	$vr9, $vr8, $vr2
	vsll.d	$vr10, $vr8, $vr3
	vor.v	$vr9, $vr10, $vr9
	vxor.v	$vr5, $vr5, $vr9
	vsrl.d	$vr8, $vr8, $vr4
	vxor.v	$vr5, $vr5, $vr8
	vinsgr2vr.d	$vr8, $s3, 0
	vinsgr2vr.d	$vr8, $a0, 1
	vadd.d	$vr5, $vr5, $vr8
	rotri.d	$a0, $t3, 14
	rotri.d	$a1, $t3, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t3, 41
	xor	$a0, $a0, $a1
	andn	$a1, $a5, $t3
	and	$a3, $a6, $t3
	or	$a1, $a3, $a1
	vpickve2gr.d	$s1, $vr5, 1
	ld.d	$a3, $sp, 296                   # 8-byte Folded Reload
	add.d	$a3, $a3, $s1
	add.d	$a3, $a3, $s4
	add.d	$a1, $a3, $a1
	add.d	$a1, $a1, $a0
	rotri.d	$a0, $t2, 28
	rotri.d	$a3, $t2, 34
	xor	$a0, $a0, $a3
	rotri.d	$a3, $t2, 39
	xor	$a3, $a0, $a3
	xor	$a0, $t2, $a4
	and	$t0, $a0, $t0
	xor	$t0, $t0, $a4
	add.d	$a3, $a3, $t0
	add.d	$t1, $a1, $t1
	add.d	$a1, $a3, $a1
	rotri.d	$a3, $t1, 14
	rotri.d	$t0, $t1, 18
	xor	$a3, $a3, $t0
	rotri.d	$t0, $t1, 41
	xor	$t5, $a3, $t0
	andn	$a3, $a6, $t1
	and	$t0, $t3, $t1
	or	$fp, $t0, $a3
	vpickve2gr.d	$a3, $vr6, 0
	st.d	$t7, $sp, 296                   # 8-byte Folded Spill
	ld.d	$t0, $sp, 272                   # 8-byte Folded Reload
	add.d	$t8, $t0, $t7
	ld.d	$t0, $sp, 304                   # 8-byte Folded Reload
	add.d	$a7, $a7, $t0
	vinsgr2vr.d	$vr6, $t6, 0
	vpickve2gr.d	$t0, $vr7, 0
	vinsgr2vr.d	$vr7, $s2, 0
	vsrl.d	$vr0, $vr7, $vr0
	vsll.d	$vr1, $vr7, $vr1
	vor.v	$vr0, $vr1, $vr0
	vsrl.d	$vr1, $vr7, $vr2
	vsll.d	$vr2, $vr7, $vr3
	vor.v	$vr1, $vr2, $vr1
	vxor.v	$vr0, $vr0, $vr1
	vsrl.d	$vr1, $vr7, $vr4
	vxor.v	$vr0, $vr0, $vr1
	vinsgr2vr.d	$vr6, $t8, 1
	vadd.d	$vr0, $vr0, $vr6
	vpickve2gr.d	$s6, $vr0, 1
	ld.d	$t6, $sp, 280                   # 8-byte Folded Reload
	add.d	$t6, $t6, $s6
	add.d	$a5, $t6, $a5
	add.d	$a5, $a5, $fp
	add.d	$a5, $a5, $t5
	rotri.d	$t5, $a1, 28
	rotri.d	$t6, $a1, 34
	xor	$t5, $t5, $t6
	rotri.d	$t6, $a1, 39
	xor	$t5, $t5, $t6
	xor	$t6, $a1, $t2
	and	$a0, $t6, $a0
	xor	$a0, $a0, $t2
	add.d	$a0, $t5, $a0
	add.d	$t5, $a5, $a2
	add.d	$a0, $a0, $a5
	rotri.d	$a2, $s1, 19
	rotri.d	$a5, $s1, 61
	xor	$a2, $a2, $a5
	srli.d	$a5, $s1, 6
	xor	$a2, $a2, $a5
	st.d	$ra, $sp, 272                   # 8-byte Folded Spill
	add.d	$a5, $t4, $ra
	add.d	$s4, $a5, $a2
	rotri.d	$a2, $t5, 14
	rotri.d	$a5, $t5, 18
	xor	$a2, $a2, $a5
	rotri.d	$a5, $t5, 41
	xor	$a2, $a2, $a5
	andn	$a5, $t3, $t5
	and	$t4, $t1, $t5
	or	$a5, $t4, $a5
	ld.d	$t7, $s0, 80
	ld.d	$t4, $sp, 288                   # 8-byte Folded Reload
	add.d	$t4, $t4, $s4
	add.d	$a6, $t4, $a6
	add.d	$a5, $a6, $a5
	add.d	$a2, $a5, $a2
	rotri.d	$a5, $a0, 28
	rotri.d	$a6, $a0, 34
	xor	$a5, $a5, $a6
	rotri.d	$a6, $a0, 39
	xor	$a5, $a5, $a6
	xor	$a6, $a0, $a1
	and	$t4, $a6, $t6
	xor	$t4, $t4, $a1
	add.d	$a5, $a5, $t4
	add.d	$t4, $a2, $a4
	add.d	$a5, $a5, $a2
	rotri.d	$a2, $s6, 19
	rotri.d	$a4, $s6, 61
	xor	$a2, $a2, $a4
	srli.d	$a4, $s6, 6
	xor	$a2, $a2, $a4
	add.d	$a2, $a7, $a2
	rotri.d	$a4, $t4, 14
	rotri.d	$a7, $t4, 18
	xor	$a4, $a4, $a7
	rotri.d	$a7, $t4, 41
	xor	$a4, $a4, $a7
	andn	$a7, $t1, $t4
	and	$t6, $t5, $t4
	or	$a7, $t6, $a7
	ld.d	$t6, $s0, 88
	st.d	$s7, $sp, 280                   # 8-byte Folded Spill
	ld.d	$t8, $sp, 248                   # 8-byte Folded Reload
	add.d	$t8, $t8, $s7
	add.d	$t7, $t7, $a2
	add.d	$t3, $t7, $t3
	add.d	$a7, $t3, $a7
	add.d	$a4, $a7, $a4
	rotri.d	$a7, $a5, 28
	rotri.d	$t3, $a5, 34
	xor	$a7, $a7, $t3
	rotri.d	$t3, $a5, 39
	xor	$a7, $a7, $t3
	xor	$t3, $a5, $a0
	and	$a6, $t3, $a6
	xor	$a6, $a6, $a0
	add.d	$a6, $a7, $a6
	add.d	$t7, $a4, $t2
	add.d	$t2, $a6, $a4
	rotri.d	$a4, $s4, 19
	rotri.d	$a6, $s4, 61
	xor	$a4, $a4, $a6
	srli.d	$a6, $s4, 6
	xor	$a4, $a4, $a6
	st.d	$s8, $sp, 288                   # 8-byte Folded Spill
	add.d	$a3, $a3, $s8
	add.d	$a7, $a3, $a4
	rotri.d	$a3, $t7, 14
	rotri.d	$a4, $t7, 18
	xor	$a3, $a3, $a4
	rotri.d	$a4, $t7, 41
	xor	$a3, $a3, $a4
	andn	$a4, $t5, $t7
	and	$a6, $t4, $t7
	or	$a4, $a6, $a4
	ld.d	$fp, $s0, 96
	add.d	$a6, $t6, $a7
	add.d	$a6, $a6, $t1
	add.d	$a4, $a6, $a4
	add.d	$a3, $a4, $a3
	rotri.d	$a4, $t2, 28
	rotri.d	$a6, $t2, 34
	xor	$a4, $a4, $a6
	rotri.d	$a6, $t2, 39
	xor	$a4, $a4, $a6
	xor	$t1, $t2, $a5
	and	$a6, $t1, $t3
	xor	$a6, $a6, $a5
	add.d	$a4, $a4, $a6
	add.d	$t3, $a3, $a1
	add.d	$a6, $a4, $a3
	rotri.d	$a1, $a2, 19
	rotri.d	$a3, $a2, 61
	xor	$a1, $a1, $a3
	srli.d	$a3, $a2, 6
	xor	$a1, $a1, $a3
	add.d	$s8, $t8, $a1
	ori	$t8, $zero, 512
	rotri.d	$a1, $t3, 14
	rotri.d	$a3, $t3, 18
	xor	$a1, $a1, $a3
	rotri.d	$a3, $t3, 41
	xor	$a1, $a1, $a3
	andn	$a3, $t4, $t3
	and	$a4, $t7, $t3
	or	$a3, $a4, $a3
	ld.d	$t6, $s0, 104
	add.d	$a4, $fp, $s8
	add.d	$a4, $a4, $t5
	add.d	$a3, $a4, $a3
	add.d	$a3, $a3, $a1
	rotri.d	$a1, $a6, 28
	rotri.d	$a4, $a6, 34
	xor	$a1, $a1, $a4
	rotri.d	$a4, $a6, 39
	xor	$a1, $a1, $a4
	xor	$t5, $a6, $t2
	and	$a4, $t5, $t1
	xor	$a4, $a4, $t2
	add.d	$a4, $a1, $a4
	add.d	$a1, $a3, $a0
	add.d	$a4, $a4, $a3
	rotri.d	$a0, $a7, 19
	rotri.d	$a3, $a7, 61
	xor	$a0, $a0, $a3
	srli.d	$a3, $a7, 6
	xor	$a0, $a0, $a3
	st.d	$s5, $sp, 264                   # 8-byte Folded Spill
	add.d	$a3, $t0, $s5
	add.d	$s5, $a3, $a0
	rotri.d	$a0, $a1, 14
	rotri.d	$a3, $a1, 18
	xor	$a0, $a0, $a3
	rotri.d	$a3, $a1, 41
	xor	$a0, $a0, $a3
	andn	$a3, $t7, $a1
	and	$t0, $t3, $a1
	or	$a3, $t0, $a3
	ld.d	$t0, $s0, 112
	add.d	$t1, $t6, $s5
	add.d	$t1, $t1, $t4
	add.d	$a3, $t1, $a3
	add.d	$a0, $a3, $a0
	rotri.d	$a3, $a4, 28
	rotri.d	$t1, $a4, 34
	xor	$a3, $a3, $t1
	rotri.d	$t1, $a4, 39
	xor	$a3, $a3, $t1
	xor	$t1, $a4, $a6
	and	$t4, $t1, $t5
	xor	$t4, $t4, $a6
	add.d	$t4, $a3, $t4
	add.d	$a3, $a0, $a5
	add.d	$a5, $t4, $a0
	rotri.d	$a0, $s8, 19
	rotri.d	$t4, $s8, 61
	xor	$a0, $a0, $t4
	srli.d	$t4, $s8, 6
	xor	$a0, $a0, $t4
	vpickve2gr.d	$t4, $vr5, 0
	add.d	$t4, $t4, $s1
	add.d	$s3, $t4, $a0
	rotri.d	$a0, $a3, 14
	rotri.d	$t4, $a3, 18
	xor	$a0, $a0, $t4
	rotri.d	$t4, $a3, 41
	xor	$a0, $a0, $t4
	andn	$t4, $t3, $a3
	and	$t5, $a1, $a3
	or	$t4, $t5, $t4
	add.d	$t0, $t0, $s3
	add.d	$t0, $t0, $t7
	add.d	$t0, $t0, $t4
	add.d	$a0, $t0, $a0
	rotri.d	$t0, $a5, 28
	rotri.d	$t4, $a5, 34
	xor	$t0, $t0, $t4
	rotri.d	$t4, $a5, 39
	xor	$t0, $t0, $t4
	xor	$t4, $a5, $a4
	and	$t1, $t4, $t1
	xor	$t1, $t1, $a4
	add.d	$t0, $t0, $t1
	add.d	$t2, $a0, $t2
	add.d	$t1, $t0, $a0
	rotri.d	$a0, $s5, 19
	rotri.d	$t0, $s5, 61
	xor	$a0, $a0, $t0
	srli.d	$t0, $s5, 6
	xor	$a0, $a0, $t0
	vpickve2gr.d	$t0, $vr0, 0
	add.d	$t0, $t0, $s6
	add.d	$fp, $t0, $a0
	ld.d	$a0, $s0, 120
	ld.d	$s0, $sp, 312                   # 8-byte Folded Reload
	rotri.d	$t0, $t2, 14
	rotri.d	$t5, $t2, 18
	xor	$t0, $t0, $t5
	rotri.d	$t5, $t2, 41
	xor	$t0, $t0, $t5
	andn	$t5, $a1, $t2
	and	$t6, $a3, $t2
	or	$t5, $t6, $t5
	add.d	$a0, $a0, $fp
	add.d	$a0, $a0, $t3
	add.d	$a0, $a0, $t5
	add.d	$t0, $a0, $t0
	rotri.d	$a0, $t1, 28
	rotri.d	$t3, $t1, 34
	xor	$a0, $a0, $t3
	rotri.d	$t3, $t1, 39
	xor	$t3, $a0, $t3
	xor	$a0, $t1, $a5
	and	$t4, $a0, $t4
	xor	$t4, $t4, $a5
	add.d	$t3, $t3, $t4
	add.d	$ra, $t0, $a6
	addi.d	$s0, $s0, 128
	add.d	$a6, $t3, $t0
	bne	$s0, $t8, .LBB0_3
# %bb.4:                                #   in Loop: Header=BB0_2 Depth=1
	ld.d	$t5, $sp, 152                   # 8-byte Folded Reload
	add.d	$t5, $t5, $a6
	ld.d	$t4, $sp, 160                   # 8-byte Folded Reload
	add.d	$t4, $t4, $t1
	ld.d	$t1, $sp, 168                   # 8-byte Folded Reload
	add.d	$t1, $t1, $a5
	ld.d	$a7, $sp, 176                   # 8-byte Folded Reload
	add.d	$a7, $a7, $a4
	ld.d	$t3, $sp, 184                   # 8-byte Folded Reload
	add.d	$t3, $t3, $ra
	ld.d	$t0, $sp, 192                   # 8-byte Folded Reload
	add.d	$t0, $t0, $t2
	ld.d	$a6, $sp, 208                   # 8-byte Folded Reload
	add.d	$a6, $a6, $a3
	ld.d	$t6, $sp, 200                   # 8-byte Folded Reload
	addi.d	$t6, $t6, 128
	ld.d	$a3, $sp, 216                   # 8-byte Folded Reload
	add.d	$a3, $a3, $a1
	ld.d	$a0, $sp, 144                   # 8-byte Folded Reload
	bne	$t6, $a0, .LBB0_2
# %bb.5:
	ld.d	$a0, $sp, 8                     # 8-byte Folded Reload
	st.d	$t5, $a0, 0
	st.d	$t4, $a0, 8
	st.d	$t1, $a0, 16
	st.d	$a7, $a0, 24
	st.d	$t3, $a0, 32
	st.d	$t0, $a0, 40
	st.d	$a6, $a0, 48
	st.d	$a3, $a0, 56
.LBB0_6:
	ld.d	$s8, $sp, 360                   # 8-byte Folded Reload
	ld.d	$s7, $sp, 368                   # 8-byte Folded Reload
	ld.d	$s6, $sp, 376                   # 8-byte Folded Reload
	ld.d	$s5, $sp, 384                   # 8-byte Folded Reload
	ld.d	$s4, $sp, 392                   # 8-byte Folded Reload
	ld.d	$s3, $sp, 400                   # 8-byte Folded Reload
	ld.d	$s2, $sp, 408                   # 8-byte Folded Reload
	ld.d	$s1, $sp, 416                   # 8-byte Folded Reload
	ld.d	$s0, $sp, 424                   # 8-byte Folded Reload
	ld.d	$fp, $sp, 432                   # 8-byte Folded Reload
	ld.d	$ra, $sp, 440                   # 8-byte Folded Reload
	addi.d	$sp, $sp, 448
	ret
.Lfunc_end0:
	.size	_Z14sha512_invokerPmPKSt4byteS2_, .Lfunc_end0-_Z14sha512_invokerPmPKSt4byteS2_
                                        # -- End function
	.type	_ZN7fast_io7details6sha51210k512scalarE,@object # @_ZN7fast_io7details6sha51210k512scalarE
	.section	.rodata._ZN7fast_io7details6sha51210k512scalarE,"aG",@progbits,_ZN7fast_io7details6sha51210k512scalarE,comdat
	.weak	_ZN7fast_io7details6sha51210k512scalarE
	.p2align	3, 0x0
_ZN7fast_io7details6sha51210k512scalarE:
	.dword	-1973867731355612462            # 0xe49b69c19ef14ad2
	.dword	-1171420211273849373            # 0xefbe4786384f25e3
	.dword	1135362057144423861             # 0xfc19dc68b8cd5b5
	.dword	2597628984639134821             # 0x240ca1cc77ac9c65
	.dword	3308224258029322869             # 0x2de92c6f592b0275
	.dword	5365058923640841347             # 0x4a7484aa6ea6e483
	.dword	6679025012923562964             # 0x5cb0a9dcbd41fbd4
	.dword	8573033837759648693             # 0x76f988da831153b5
	.dword	-7476448914759557205            # 0x983e5152ee66dfab
	.dword	-6327057829258317296            # 0xa831c66d2db43210
	.dword	-5763719355590565569            # 0xb00327c898fb213f
	.dword	-4658551843659510044            # 0xbf597fc7beef0ee4
	.dword	-4116276920077217854            # 0xc6e00bf33da88fc2
	.dword	-3051310485924567259            # 0xd5a79147930aa725
	.dword	489312712824947311              # 0x6ca6351e003826f
	.dword	1452737877330783856             # 0x142929670a0e6e70
	.dword	2861767655752347644             # 0x27b70a8546d22ffc
	.dword	3322285676063803686             # 0x2e1b21385c26c926
	.dword	5560940570517711597             # 0x4d2c6dfc5ac42aed
	.dword	5996557281743188959             # 0x53380d139d95b3df
	.dword	7280758554555802590             # 0x650a73548baf63de
	.dword	8532644243296465576             # 0x766a0abb3c77b2a8
	.dword	-9096487096722542874            # 0x81c2c92e47edaee6
	.dword	-7894198246740708037            # 0x92722c851482353b
	.dword	-6719396339535248540            # 0xa2bfe8a14cf10364
	.dword	-6333637450476146687            # 0xa81a664bbc423001
	.dword	-4446306890439682159            # 0xc24b8b70d0f89791
	.dword	-4076793802049405392            # 0xc76c51a30654be30
	.dword	-3345356375505022440            # 0xd192e819d6ef5218
	.dword	-2983346525034927856            # 0xd69906245565a910
	.dword	-860691631967231958             # 0xf40e35855771202a
	.dword	1182934255886127544             # 0x106aa07032bbd1b8
	.dword	1847814050463011016             # 0x19a4c116b8d2d0c8
	.dword	2177327727835720531             # 0x1e376c085141ab53
	.dword	2830643537854262169             # 0x2748774cdf8eeb99
	.dword	3796741975233480872             # 0x34b0bcb5e19b48a8
	.dword	4115178125766777443             # 0x391c0cb3c5c95a63
	.dword	5681478168544905931             # 0x4ed8aa4ae3418acb
	.dword	6601373596472566643             # 0x5b9cca4f7763e373
	.dword	7507060721942968483             # 0x682e6ff3d6b2b8a3
	.dword	8399075790359081724             # 0x748f82ee5defb2fc
	.dword	8693463985226723168             # 0x78a5636f43172f60
	.dword	-8878714635349349518            # 0x84c87814a1f0ab72
	.dword	-8302665154208450068            # 0x8cc702081a6439ec
	.dword	-8016688836872298968            # 0x90befffa23631e28
	.dword	-6606660893046293015            # 0xa4506cebde82bde9
	.dword	-4685533653050689259            # 0xbef9a3f7b2c67915
	.dword	-4147400797238176981            # 0xc67178f2e372532b
	.dword	-3880063495543823972            # 0xca273eceea26619c
	.dword	-3348786107499101689            # 0xd186b8c721c0c207
	.dword	-1523767162380948706            # 0xeada7dd6cde0eb1e
	.dword	-757361751448694408             # 0xf57d4f7fee6ed178
	.dword	500013540394364858              # 0x6f067aa72176fba
	.dword	748580250866718886              # 0xa637dc5a2c898a6
	.dword	1242879168328830382             # 0x113f9804bef90dae
	.dword	1977374033974150939             # 0x1b710b35131c471b
	.dword	2944078676154940804             # 0x28db77f523047d84
	.dword	3659926193048069267             # 0x32caab7b40c72493
	.dword	4368137639120453308             # 0x3c9ebe0a15c9bebc
	.dword	4836135668995329356             # 0x431d67c49c100d4c
	.dword	5532061633213252278             # 0x4cc5d4becb3e42b6
	.dword	6448918945643986474             # 0x597f299cfc657e2a
	.dword	6902733635092675308             # 0x5fcb6fab3ad6faec
	.dword	7801388544844847127             # 0x6c44198c4a475817
	.size	_ZN7fast_io7details6sha51210k512scalarE, 512

	.section	".linker-options","e",@llvm_linker_options
	.ident	"clang version 20.0.0git (git@github.com:trcrsired/llvm-project.git 1ccbabd27406055793ca2fd73a6fd2f0d9474828)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
