	.text
	.file	"sha512.cc"
	.globl	_Z14sha512_invokerPmPKSt4byteS2_ # -- Begin function _Z14sha512_invokerPmPKSt4byteS2_
	.p2align	5
	.type	_Z14sha512_invokerPmPKSt4byteS2_,@function
_Z14sha512_invokerPmPKSt4byteS2_:       # @_Z14sha512_invokerPmPKSt4byteS2_
# %bb.0:
	addi.d	$sp, $sp, -400
	st.d	$ra, $sp, 392                   # 8-byte Folded Spill
	st.d	$fp, $sp, 384                   # 8-byte Folded Spill
	st.d	$s0, $sp, 376                   # 8-byte Folded Spill
	st.d	$s1, $sp, 368                   # 8-byte Folded Spill
	st.d	$s2, $sp, 360                   # 8-byte Folded Spill
	st.d	$s3, $sp, 352                   # 8-byte Folded Spill
	st.d	$s4, $sp, 344                   # 8-byte Folded Spill
	st.d	$s5, $sp, 336                   # 8-byte Folded Spill
	st.d	$s6, $sp, 328                   # 8-byte Folded Spill
	st.d	$s7, $sp, 320                   # 8-byte Folded Spill
	st.d	$s8, $sp, 312                   # 8-byte Folded Spill
	st.d	$a2, $sp, 136                   # 8-byte Folded Spill
	beq	$a1, $a2, .LBB0_6
# %bb.1:
	ld.d	$a5, $a0, 56
	ld.d	$t0, $a0, 48
	ld.d	$t2, $a0, 40
	ld.d	$t3, $a0, 32
	ld.d	$t7, $a0, 24
	ld.d	$t5, $a0, 16
	ld.d	$t6, $a0, 8
	st.d	$a0, $sp, 0                     # 8-byte Folded Spill
	ld.d	$fp, $a0, 0
	lu12i.w	$a0, -167286
	ori	$a0, $a0, 3618
	lu32i.d	$a0, -381032
	lu52i.d	$a0, $a0, 1064
	st.d	$a0, $sp, 128                   # 8-byte Folded Spill
	lu12i.w	$a0, 147190
	ori	$a0, $a0, 1485
	lu32i.d	$a0, 476305
	lu52i.d	$a0, $a0, 1811
	st.d	$a0, $sp, 120                   # 8-byte Folded Spill
	lu12i.w	$a0, -80685
	ori	$a0, $a0, 2863
	lu32i.d	$a0, 64463
	lu52i.d	$a0, $a0, -1188
	st.d	$a0, $sp, 112                   # 8-byte Folded Spill
	lu12i.w	$a0, -517987
	ori	$a0, $a0, 3004
	lu32i.d	$a0, 383909
	lu52i.d	$a0, $a0, -357
	st.d	$a0, $sp, 104                   # 8-byte Folded Spill
	lu12i.w	$a0, -52085
	ori	$a0, $a0, 1336
	lu32i.d	$a0, 442971
	lu52i.d	$a0, $a0, 917
	st.d	$a0, $sp, 96                    # 8-byte Folded Spill
	lu12i.w	$a0, -303011
	ori	$a0, $a0, 25
	lu32i.d	$a0, 70129
	lu52i.d	$a0, $a0, 1439
	st.d	$a0, $sp, 88                    # 8-byte Folded Spill
	lu12i.w	$a0, -331372
	ori	$a0, $a0, 3995
	lu32i.d	$a0, -32092
	lu52i.d	$a0, $a0, -1757
	st.d	$a0, $sp, 80                    # 8-byte Folded Spill
	lu12i.w	$a0, -153896
	ori	$a0, $a0, 280
	lu32i.d	$a0, -237867
	lu52i.d	$a0, $a0, -1359
	st.d	$a0, $sp, 72                    # 8-byte Folded Spill
	lu12i.w	$a0, -380880
	ori	$a0, $a0, 578
	lu32i.d	$a0, 502424
	lu52i.d	$a0, $a0, -640
	st.d	$a0, $sp, 64                    # 8-byte Folded Spill
	lu12i.w	$a0, 284422
	ori	$a0, $a0, 4030
	lu32i.d	$a0, 219905
	lu52i.d	$a0, $a0, 296
	st.d	$a0, $sp, 56                    # 8-byte Folded Spill
	lu12i.w	$a0, 323147
	ori	$a0, $a0, 652
	lu32i.d	$a0, 99774
	lu52i.d	$a0, $a0, 579
	st.d	$a0, $sp, 48                    # 8-byte Folded Spill
	lu12i.w	$a0, -172037
	ori	$a0, $a0, 1250
	lu32i.d	$a0, -229949
	lu52i.d	$a0, $a0, 1360
	st.d	$a0, $sp, 40                    # 8-byte Folded Spill
	lu12i.w	$a0, -55368
	ori	$a0, $a0, 2415
	lu32i.d	$a0, -107148
	lu52i.d	$a0, $a0, 1835
	st.d	$a0, $sp, 32                    # 8-byte Folded Spill
	lu12i.w	$a0, 242025
	ori	$a0, $a0, 1713
	lu32i.d	$a0, -85506
	lu52i.d	$a0, $a0, -2035
	st.d	$a0, $sp, 24                    # 8-byte Folded Spill
	lu12i.w	$a0, 154737
	ori	$a0, $a0, 565
	lu32i.d	$a0, -260441
	lu52i.d	$a0, $a0, -1603
	st.d	$a0, $sp, 16                    # 8-byte Folded Spill
	lu12i.w	$a0, -199022
	ori	$a0, $a0, 1684
	lu32i.d	$a0, -265868
	lu52i.d	$a0, $a0, -999
	st.d	$a0, $sp, 8                     # 8-byte Folded Spill
	.p2align	4, , 16
.LBB0_2:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	ld.d	$a0, $a1, 0
	move	$t4, $zero
	revb.d	$a0, $a0
	rotri.d	$a2, $t3, 14
	rotri.d	$a3, $t3, 18
	xor	$a2, $a2, $a3
	rotri.d	$a3, $t3, 41
	xor	$a2, $a2, $a3
	andn	$a3, $t0, $t3
	and	$a4, $t2, $t3
	or	$a3, $a4, $a3
	st.d	$a5, $sp, 208                   # 8-byte Folded Spill
	add.d	$a2, $a2, $a5
	add.d	$a2, $a2, $a3
	st.d	$a0, $sp, 304                   # 8-byte Folded Spill
	add.d	$a2, $a2, $a0
	ld.d	$a0, $sp, 128                   # 8-byte Folded Reload
	add.d	$a2, $a2, $a0
	rotri.d	$a3, $fp, 28
	rotri.d	$a4, $fp, 34
	xor	$a3, $a3, $a4
	rotri.d	$a4, $fp, 39
	xor	$a3, $a3, $a4
	xor	$a4, $t6, $fp
	andn	$a5, $t6, $a4
	and	$a6, $t5, $a4
	or	$a5, $a6, $a5
	ld.d	$a7, $a1, 8
	add.d	$a3, $a5, $a3
	st.d	$t7, $sp, 168                   # 8-byte Folded Spill
	add.d	$a6, $a2, $t7
	add.d	$a2, $a3, $a2
	revb.d	$a0, $a7
	rotri.d	$a3, $a6, 14
	rotri.d	$a5, $a6, 18
	xor	$a3, $a3, $a5
	rotri.d	$a5, $a6, 41
	xor	$a3, $a3, $a5
	andn	$a5, $t2, $a6
	and	$a7, $t3, $a6
	or	$a5, $a7, $a5
	st.d	$t0, $sp, 200                   # 8-byte Folded Spill
	st.d	$a0, $sp, 296                   # 8-byte Folded Spill
	add.d	$a7, $t0, $a0
	add.d	$a5, $a7, $a5
	add.d	$a3, $a5, $a3
	ld.d	$a0, $sp, 120                   # 8-byte Folded Reload
	add.d	$a3, $a3, $a0
	rotri.d	$a5, $a2, 28
	rotri.d	$a7, $a2, 34
	xor	$a5, $a5, $a7
	rotri.d	$a7, $a2, 39
	xor	$a5, $a5, $a7
	xor	$t0, $a2, $fp
	and	$a4, $t0, $a4
	xor	$a4, $a4, $fp
	ld.d	$t1, $a1, 16
	add.d	$a4, $a5, $a4
	st.d	$t5, $sp, 160                   # 8-byte Folded Spill
	add.d	$a7, $a3, $t5
	add.d	$a3, $a4, $a3
	revb.d	$a0, $t1
	rotri.d	$a4, $a7, 14
	rotri.d	$a5, $a7, 18
	xor	$a4, $a4, $a5
	rotri.d	$a5, $a7, 41
	xor	$a4, $a4, $a5
	andn	$a5, $t3, $a7
	and	$t1, $a6, $a7
	or	$a5, $t1, $a5
	st.d	$t2, $sp, 184                   # 8-byte Folded Spill
	st.d	$a0, $sp, 288                   # 8-byte Folded Spill
	add.d	$t1, $t2, $a0
	add.d	$a5, $t1, $a5
	add.d	$a4, $a5, $a4
	ld.d	$a0, $sp, 112                   # 8-byte Folded Reload
	add.d	$a4, $a4, $a0
	rotri.d	$a5, $a3, 28
	rotri.d	$t1, $a3, 34
	xor	$a5, $a5, $t1
	rotri.d	$t1, $a3, 39
	xor	$a5, $a5, $t1
	xor	$t1, $a3, $a2
	and	$t0, $t1, $t0
	xor	$t0, $t0, $a2
	ld.d	$t2, $a1, 24
	add.d	$a5, $a5, $t0
	st.d	$t6, $sp, 152                   # 8-byte Folded Spill
	add.d	$t0, $a4, $t6
	add.d	$a4, $a5, $a4
	revb.d	$a0, $t2
	rotri.d	$a5, $t0, 14
	rotri.d	$t2, $t0, 18
	xor	$a5, $a5, $t2
	rotri.d	$t2, $t0, 41
	xor	$a5, $a5, $t2
	andn	$t2, $a6, $t0
	and	$t7, $a7, $t0
	or	$t2, $t7, $t2
	st.d	$t3, $sp, 176                   # 8-byte Folded Spill
	st.d	$a0, $sp, 280                   # 8-byte Folded Spill
	add.d	$t7, $t3, $a0
	add.d	$t2, $t7, $t2
	add.d	$a5, $t2, $a5
	ld.d	$a0, $sp, 104                   # 8-byte Folded Reload
	add.d	$a5, $a5, $a0
	rotri.d	$t2, $a4, 28
	rotri.d	$t7, $a4, 34
	xor	$t2, $t2, $t7
	rotri.d	$t7, $a4, 39
	xor	$t2, $t2, $t7
	xor	$t8, $a4, $a3
	and	$t1, $t8, $t1
	xor	$t1, $t1, $a3
	ld.d	$t7, $a1, 32
	add.d	$t2, $t2, $t1
	st.d	$fp, $sp, 144                   # 8-byte Folded Spill
	add.d	$t1, $a5, $fp
	add.d	$a5, $t2, $a5
	revb.d	$a0, $t7
	rotri.d	$t2, $t1, 14
	rotri.d	$fp, $t1, 18
	xor	$t2, $t2, $fp
	rotri.d	$fp, $t1, 41
	xor	$t2, $t2, $fp
	andn	$fp, $a7, $t1
	and	$s0, $t0, $t1
	or	$fp, $s0, $fp
	st.d	$a0, $sp, 272                   # 8-byte Folded Spill
	add.d	$a6, $a6, $a0
	add.d	$a6, $a6, $fp
	add.d	$a6, $a6, $t2
	ld.d	$a0, $sp, 96                    # 8-byte Folded Reload
	add.d	$t2, $a6, $a0
	rotri.d	$a6, $a5, 28
	rotri.d	$fp, $a5, 34
	xor	$a6, $a6, $fp
	rotri.d	$fp, $a5, 39
	xor	$a6, $a6, $fp
	xor	$fp, $a5, $a4
	and	$t8, $fp, $t8
	xor	$t8, $t8, $a4
	ld.d	$s0, $a1, 40
	add.d	$t8, $a6, $t8
	add.d	$a6, $t2, $a2
	add.d	$a2, $t8, $t2
	revb.d	$a0, $s0
	rotri.d	$t2, $a6, 14
	rotri.d	$s0, $a6, 18
	xor	$t2, $t2, $s0
	rotri.d	$s0, $a6, 41
	xor	$t2, $t2, $s0
	andn	$s0, $t0, $a6
	and	$s1, $t1, $a6
	or	$s0, $s1, $s0
	st.d	$a0, $sp, 264                   # 8-byte Folded Spill
	add.d	$a7, $a0, $a7
	add.d	$a7, $a7, $s0
	add.d	$a7, $a7, $t2
	ld.d	$a0, $sp, 88                    # 8-byte Folded Reload
	add.d	$t2, $a7, $a0
	rotri.d	$a7, $a2, 28
	rotri.d	$s0, $a2, 34
	xor	$a7, $a7, $s0
	rotri.d	$s0, $a2, 39
	xor	$a7, $a7, $s0
	xor	$s0, $a2, $a5
	and	$fp, $s0, $fp
	xor	$fp, $fp, $a5
	ld.d	$s1, $a1, 48
	add.d	$fp, $a7, $fp
	add.d	$a7, $t2, $a3
	add.d	$a3, $fp, $t2
	revb.d	$a0, $s1
	rotri.d	$t2, $a7, 14
	rotri.d	$s1, $a7, 18
	xor	$t2, $t2, $s1
	rotri.d	$s1, $a7, 41
	xor	$t2, $t2, $s1
	andn	$s1, $t1, $a7
	and	$s2, $a6, $a7
	or	$s1, $s2, $s1
	st.d	$a0, $sp, 256                   # 8-byte Folded Spill
	add.d	$t0, $a0, $t0
	add.d	$t0, $t0, $s1
	add.d	$t0, $t0, $t2
	ld.d	$a0, $sp, 80                    # 8-byte Folded Reload
	add.d	$t2, $t0, $a0
	rotri.d	$t0, $a3, 28
	rotri.d	$s1, $a3, 34
	xor	$t0, $t0, $s1
	rotri.d	$s1, $a3, 39
	xor	$t0, $t0, $s1
	xor	$s1, $a3, $a2
	and	$s0, $s1, $s0
	xor	$s0, $s0, $a2
	ld.d	$s2, $a1, 56
	add.d	$s0, $t0, $s0
	add.d	$t0, $t2, $a4
	add.d	$a4, $s0, $t2
	revb.d	$a0, $s2
	rotri.d	$t2, $t0, 14
	rotri.d	$s2, $t0, 18
	xor	$t2, $t2, $s2
	rotri.d	$s2, $t0, 41
	xor	$t2, $t2, $s2
	andn	$s2, $a6, $t0
	and	$s3, $a7, $t0
	or	$s2, $s3, $s2
	st.d	$a0, $sp, 248                   # 8-byte Folded Spill
	add.d	$t1, $a0, $t1
	add.d	$t1, $t1, $s2
	add.d	$t1, $t1, $t2
	ld.d	$a0, $sp, 72                    # 8-byte Folded Reload
	add.d	$t2, $t1, $a0
	rotri.d	$t1, $a4, 28
	rotri.d	$s2, $a4, 34
	xor	$t1, $t1, $s2
	rotri.d	$s2, $a4, 39
	xor	$t1, $t1, $s2
	xor	$s2, $a4, $a3
	and	$s1, $s2, $s1
	xor	$s1, $s1, $a3
	ld.d	$s3, $a1, 64
	add.d	$s1, $t1, $s1
	add.d	$t1, $t2, $a5
	add.d	$a5, $s1, $t2
	revb.d	$t6, $s3
	rotri.d	$t2, $t1, 14
	rotri.d	$s3, $t1, 18
	xor	$t2, $t2, $s3
	rotri.d	$s3, $t1, 41
	xor	$t2, $t2, $s3
	andn	$s3, $a7, $t1
	and	$s4, $t0, $t1
	or	$s3, $s4, $s3
	add.d	$a6, $t6, $a6
	add.d	$a6, $a6, $s3
	add.d	$a6, $a6, $t2
	ld.d	$a0, $sp, 64                    # 8-byte Folded Reload
	add.d	$a6, $a6, $a0
	rotri.d	$t2, $a5, 28
	rotri.d	$s3, $a5, 34
	xor	$t2, $t2, $s3
	rotri.d	$s3, $a5, 39
	xor	$t2, $t2, $s3
	xor	$s3, $a5, $a4
	and	$s2, $s3, $s2
	xor	$s2, $s2, $a4
	ld.d	$s4, $a1, 72
	add.d	$t2, $t2, $s2
	add.d	$s6, $a6, $a2
	add.d	$a2, $t2, $a6
	revb.d	$t7, $s4
	rotri.d	$a6, $s6, 14
	rotri.d	$t2, $s6, 18
	xor	$a6, $a6, $t2
	rotri.d	$t2, $s6, 41
	xor	$a6, $a6, $t2
	andn	$t2, $t0, $s6
	and	$s4, $t1, $s6
	or	$t2, $s4, $t2
	add.d	$a7, $t7, $a7
	add.d	$a7, $a7, $t2
	add.d	$a6, $a7, $a6
	ld.d	$a0, $sp, 56                    # 8-byte Folded Reload
	add.d	$a6, $a6, $a0
	rotri.d	$a7, $a2, 28
	rotri.d	$t2, $a2, 34
	xor	$a7, $a7, $t2
	rotri.d	$t2, $a2, 39
	xor	$a7, $a7, $t2
	xor	$t2, $a2, $a5
	and	$s3, $t2, $s3
	xor	$s3, $s3, $a5
	ld.d	$s4, $a1, 80
	add.d	$a7, $a7, $s3
	add.d	$s7, $a6, $a3
	add.d	$a3, $a7, $a6
	revb.d	$t8, $s4
	rotri.d	$a6, $s7, 14
	rotri.d	$a7, $s7, 18
	xor	$a6, $a6, $a7
	rotri.d	$a7, $s7, 41
	xor	$a6, $a6, $a7
	andn	$a7, $t1, $s7
	and	$s4, $s6, $s7
	or	$a7, $s4, $a7
	add.d	$t0, $t8, $t0
	add.d	$a7, $t0, $a7
	add.d	$a6, $a7, $a6
	ld.d	$a0, $sp, 48                    # 8-byte Folded Reload
	add.d	$a6, $a6, $a0
	rotri.d	$a7, $a3, 28
	rotri.d	$t0, $a3, 34
	xor	$a7, $a7, $t0
	rotri.d	$t0, $a3, 39
	xor	$a7, $a7, $t0
	xor	$t0, $a3, $a2
	and	$t2, $t0, $t2
	xor	$t2, $t2, $a2
	ld.d	$s4, $a1, 88
	add.d	$a7, $a7, $t2
	add.d	$s8, $a6, $a4
	add.d	$a6, $a7, $a6
	revb.d	$fp, $s4
	rotri.d	$a4, $s8, 14
	rotri.d	$a7, $s8, 18
	xor	$a4, $a4, $a7
	rotri.d	$a7, $s8, 41
	xor	$a4, $a4, $a7
	andn	$a7, $s6, $s8
	and	$t2, $s7, $s8
	or	$a7, $t2, $a7
	add.d	$t1, $fp, $t1
	add.d	$a7, $t1, $a7
	add.d	$a4, $a7, $a4
	ld.d	$a0, $sp, 40                    # 8-byte Folded Reload
	add.d	$a4, $a4, $a0
	rotri.d	$a7, $a6, 28
	rotri.d	$t1, $a6, 34
	xor	$a7, $a7, $t1
	rotri.d	$t1, $a6, 39
	xor	$a7, $a7, $t1
	xor	$t1, $a6, $a3
	and	$t0, $t1, $t0
	xor	$t0, $t0, $a3
	ld.d	$s5, $a1, 96
	add.d	$a7, $a7, $t0
	add.d	$ra, $a4, $a5
	add.d	$t2, $a7, $a4
	revb.d	$s0, $s5
	rotri.d	$a4, $ra, 14
	rotri.d	$a5, $ra, 18
	xor	$a4, $a4, $a5
	rotri.d	$a5, $ra, 41
	xor	$a4, $a4, $a5
	andn	$a5, $s7, $ra
	and	$a7, $s8, $ra
	or	$a5, $a7, $a5
	add.d	$a7, $s0, $s6
	add.d	$a5, $a7, $a5
	add.d	$a4, $a5, $a4
	ld.d	$a0, $sp, 32                    # 8-byte Folded Reload
	add.d	$a4, $a4, $a0
	rotri.d	$a5, $t2, 28
	rotri.d	$a7, $t2, 34
	xor	$a5, $a5, $a7
	rotri.d	$a7, $t2, 39
	xor	$a5, $a5, $a7
	xor	$t0, $t2, $a6
	and	$a7, $t0, $t1
	xor	$a7, $a7, $a6
	ld.d	$t1, $a1, 104
	add.d	$a5, $a5, $a7
	add.d	$a7, $a4, $a2
	add.d	$a2, $a5, $a4
	revb.d	$s4, $t1
	rotri.d	$a4, $a7, 14
	rotri.d	$a5, $a7, 18
	xor	$a4, $a4, $a5
	rotri.d	$a5, $a7, 41
	xor	$a4, $a4, $a5
	andn	$a5, $s8, $a7
	and	$t1, $ra, $a7
	or	$a5, $t1, $a5
	add.d	$t1, $s4, $s7
	add.d	$a5, $t1, $a5
	add.d	$a4, $a5, $a4
	ld.d	$a0, $sp, 24                    # 8-byte Folded Reload
	add.d	$a4, $a4, $a0
	rotri.d	$a5, $a2, 28
	rotri.d	$t1, $a2, 34
	xor	$a5, $a5, $t1
	rotri.d	$t1, $a2, 39
	xor	$a5, $a5, $t1
	xor	$t3, $a2, $t2
	and	$t0, $t3, $t0
	xor	$t0, $t0, $t2
	ld.d	$s7, $a1, 112
	add.d	$a5, $a5, $t0
	add.d	$t0, $a4, $a3
	add.d	$a4, $a5, $a4
	revb.d	$s7, $s7
	rotri.d	$a3, $t0, 14
	rotri.d	$a5, $t0, 18
	xor	$a3, $a3, $a5
	rotri.d	$a5, $t0, 41
	xor	$a3, $a3, $a5
	andn	$a5, $ra, $t0
	and	$t1, $a7, $t0
	or	$a5, $t1, $a5
	add.d	$t1, $s7, $s8
	add.d	$a5, $t1, $a5
	add.d	$a3, $a5, $a3
	ld.d	$a0, $sp, 16                    # 8-byte Folded Reload
	add.d	$a3, $a3, $a0
	rotri.d	$a5, $a4, 28
	rotri.d	$t1, $a4, 34
	xor	$a5, $a5, $t1
	rotri.d	$t1, $a4, 39
	xor	$a5, $a5, $t1
	xor	$a0, $a4, $a2
	and	$t1, $a0, $t3
	xor	$t1, $t1, $a2
	add.d	$a5, $a5, $t1
	add.d	$t1, $a3, $a6
	add.d	$a5, $a5, $a3
	st.d	$a1, $sp, 192                   # 8-byte Folded Spill
	ld.d	$a1, $a1, 120
	rotri.d	$a6, $t1, 14
	rotri.d	$s8, $t1, 18
	xor	$a6, $a6, $s8
	rotri.d	$s8, $t1, 41
	xor	$a6, $a6, $s8
	andn	$s8, $a7, $t1
	and	$a3, $t0, $t1
	or	$a3, $a3, $s8
	revb.d	$s8, $a1
	add.d	$ra, $s8, $ra
	add.d	$a3, $ra, $a3
	add.d	$a6, $a3, $a6
	rotri.d	$a3, $a5, 28
	rotri.d	$ra, $a5, 34
	xor	$a3, $a3, $ra
	rotri.d	$ra, $a5, 39
	xor	$ra, $a3, $ra
	xor	$a3, $a5, $a4
	and	$a1, $a3, $a0
	xor	$a1, $a1, $a4
	add.d	$a1, $ra, $a1
	ld.d	$a0, $sp, 8                     # 8-byte Folded Reload
	add.d	$ra, $a6, $a0
	add.d	$a6, $ra, $t2
	add.d	$t2, $a1, $ra
	.p2align	4, , 16
.LBB0_3:                                #   Parent Loop BB0_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	st.d	$s0, $sp, 216                   # 8-byte Folded Spill
	st.d	$t8, $sp, 224                   # 8-byte Folded Spill
	st.d	$t7, $sp, 232                   # 8-byte Folded Spill
	move	$s6, $t6
	ld.d	$t6, $sp, 296                   # 8-byte Folded Reload
	rotri.d	$a1, $t6, 1
	rotri.d	$ra, $t6, 8
	xor	$a1, $a1, $ra
	srli.d	$ra, $t6, 7
	xor	$a1, $a1, $ra
	rotri.d	$ra, $s7, 19
	rotri.d	$t3, $s7, 61
	xor	$t3, $ra, $t3
	srli.d	$ra, $s7, 6
	xor	$t3, $t3, $ra
	ld.d	$a0, $sp, 304                   # 8-byte Folded Reload
	add.d	$a0, $a0, $t7
	add.d	$a0, $a0, $a1
	add.d	$t5, $a0, $t3
	rotri.d	$a1, $a6, 14
	rotri.d	$t3, $a6, 18
	xor	$a1, $a1, $t3
	rotri.d	$t3, $a6, 41
	xor	$a1, $a1, $t3
	andn	$t3, $t0, $a6
	and	$ra, $t1, $a6
	or	$t3, $ra, $t3
	pcalau12i	$ra, %pc_hi20(_ZN7fast_io7details6sha51210k512scalarE)
	addi.d	$ra, $ra, %pc_lo12(_ZN7fast_io7details6sha51210k512scalarE)
	ldx.d	$a0, $ra, $t4
	st.d	$t4, $sp, 240                   # 8-byte Folded Spill
	add.d	$a1, $a7, $a1
	add.d	$a1, $a1, $t3
	add.d	$a1, $a1, $t5
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $t2, 28
	rotri.d	$a7, $t2, 34
	xor	$a1, $a1, $a7
	rotri.d	$a7, $t2, 39
	xor	$a1, $a1, $a7
	xor	$t3, $a5, $t2
	and	$a3, $a3, $t3
	xor	$a3, $a3, $a5
	add.d	$a1, $a3, $a1
	add.d	$a7, $a0, $a2
	add.d	$a2, $a1, $a0
	ld.d	$t7, $sp, 288                   # 8-byte Folded Reload
	rotri.d	$a0, $t7, 1
	rotri.d	$a1, $t7, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $t7, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $s8, 19
	rotri.d	$a3, $s8, 61
	xor	$a1, $a1, $a3
	srli.d	$a3, $s8, 6
	xor	$a1, $a1, $a3
	add.d	$a3, $ra, $t4
	add.d	$t4, $t6, $t8
	add.d	$a0, $t4, $a0
	add.d	$t6, $a0, $a1
	rotri.d	$a0, $a7, 14
	rotri.d	$a1, $a7, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $a7, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 8
	andn	$ra, $t1, $a7
	and	$t4, $a6, $a7
	or	$t4, $t4, $ra
	add.d	$t0, $t6, $t0
	add.d	$a1, $t0, $a1
	add.d	$a1, $a1, $t4
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a2, 28
	rotri.d	$t0, $a2, 34
	xor	$a1, $a1, $t0
	rotri.d	$t0, $a2, 39
	xor	$a1, $a1, $t0
	xor	$t4, $a2, $t2
	and	$t0, $t4, $t3
	xor	$t0, $t0, $t2
	add.d	$a1, $a1, $t0
	add.d	$t0, $a0, $a4
	add.d	$a4, $a1, $a0
	ld.d	$t8, $sp, 280                   # 8-byte Folded Reload
	rotri.d	$a0, $t8, 1
	rotri.d	$a1, $t8, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $t8, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t5, 19
	rotri.d	$t3, $t5, 61
	xor	$a1, $a1, $t3
	srli.d	$t3, $t5, 6
	xor	$a1, $a1, $t3
	add.d	$t3, $t7, $fp
	add.d	$a0, $t3, $a0
	add.d	$t7, $a0, $a1
	rotri.d	$a0, $t0, 14
	rotri.d	$a1, $t0, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t0, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 16
	andn	$t3, $a6, $t0
	and	$ra, $a7, $t0
	or	$t3, $ra, $t3
	add.d	$t1, $t7, $t1
	add.d	$a1, $t1, $a1
	add.d	$a1, $a1, $t3
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a4, 28
	rotri.d	$t1, $a4, 34
	xor	$a1, $a1, $t1
	rotri.d	$t1, $a4, 39
	xor	$a1, $a1, $t1
	xor	$t3, $a4, $a2
	and	$t1, $t3, $t4
	xor	$t1, $t1, $a2
	add.d	$a1, $a1, $t1
	add.d	$t1, $a0, $a5
	add.d	$a5, $a1, $a0
	move	$s5, $fp
	ld.d	$fp, $sp, 272                   # 8-byte Folded Reload
	rotri.d	$a0, $fp, 1
	rotri.d	$a1, $fp, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $fp, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t6, 19
	rotri.d	$t4, $t6, 61
	xor	$a1, $a1, $t4
	srli.d	$t4, $t6, 6
	xor	$a1, $a1, $t4
	add.d	$t4, $t8, $s0
	add.d	$a0, $t4, $a0
	add.d	$t8, $a0, $a1
	rotri.d	$a0, $t1, 14
	rotri.d	$a1, $t1, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t1, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 24
	andn	$t4, $a7, $t1
	and	$ra, $t0, $t1
	or	$t4, $ra, $t4
	add.d	$a6, $t8, $a6
	add.d	$a1, $a6, $a1
	add.d	$a1, $a1, $t4
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a5, 28
	rotri.d	$a6, $a5, 34
	xor	$a1, $a1, $a6
	rotri.d	$a6, $a5, 39
	xor	$a1, $a1, $a6
	xor	$t4, $a5, $a4
	and	$a6, $t4, $t3
	xor	$a6, $a6, $a4
	add.d	$a1, $a1, $a6
	add.d	$t2, $a0, $t2
	add.d	$a6, $a1, $a0
	ld.d	$s0, $sp, 264                   # 8-byte Folded Reload
	rotri.d	$a0, $s0, 1
	rotri.d	$a1, $s0, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s0, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t7, 19
	rotri.d	$t3, $t7, 61
	xor	$a1, $a1, $t3
	srli.d	$t3, $t7, 6
	xor	$a1, $a1, $t3
	add.d	$t3, $fp, $s4
	add.d	$a0, $t3, $a0
	add.d	$fp, $a0, $a1
	rotri.d	$a0, $t2, 14
	rotri.d	$a1, $t2, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t2, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 32
	andn	$t3, $t0, $t2
	and	$ra, $t1, $t2
	or	$t3, $ra, $t3
	add.d	$a7, $a7, $fp
	add.d	$a1, $a7, $a1
	add.d	$a1, $a1, $t3
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a6, 28
	rotri.d	$a7, $a6, 34
	xor	$a1, $a1, $a7
	rotri.d	$a7, $a6, 39
	xor	$a1, $a1, $a7
	xor	$t3, $a6, $a5
	and	$a7, $t3, $t4
	xor	$a7, $a7, $a5
	add.d	$a1, $a1, $a7
	add.d	$a7, $a0, $a2
	add.d	$a2, $a1, $a0
	ld.d	$s1, $sp, 256                   # 8-byte Folded Reload
	rotri.d	$a0, $s1, 1
	rotri.d	$a1, $s1, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s1, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t8, 19
	rotri.d	$t4, $t8, 61
	xor	$a1, $a1, $t4
	srli.d	$t4, $t8, 6
	xor	$a1, $a1, $t4
	add.d	$a0, $a0, $s0
	add.d	$a0, $a0, $s7
	add.d	$s0, $a0, $a1
	rotri.d	$a0, $a7, 14
	rotri.d	$a1, $a7, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $a7, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 40
	andn	$t4, $t1, $a7
	and	$ra, $t2, $a7
	or	$t4, $ra, $t4
	add.d	$a1, $a1, $s0
	add.d	$a1, $a1, $t0
	add.d	$a1, $a1, $t4
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a2, 28
	rotri.d	$t0, $a2, 34
	xor	$a1, $a1, $t0
	rotri.d	$t0, $a2, 39
	xor	$a1, $a1, $t0
	xor	$t4, $a2, $a6
	and	$t0, $t4, $t3
	xor	$t0, $t0, $a6
	add.d	$a1, $a1, $t0
	add.d	$t0, $a0, $a4
	add.d	$a4, $a1, $a0
	ld.d	$s2, $sp, 248                   # 8-byte Folded Reload
	rotri.d	$a0, $s2, 1
	rotri.d	$a1, $s2, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s2, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $fp, 19
	rotri.d	$t3, $fp, 61
	xor	$a1, $a1, $t3
	srli.d	$t3, $fp, 6
	xor	$a1, $a1, $t3
	add.d	$a0, $a0, $s1
	add.d	$a0, $a0, $s8
	add.d	$s1, $a0, $a1
	rotri.d	$a0, $t0, 14
	rotri.d	$a1, $t0, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t0, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 48
	andn	$t3, $t2, $t0
	and	$ra, $a7, $t0
	or	$t3, $ra, $t3
	add.d	$a1, $a1, $s1
	add.d	$a1, $a1, $t1
	add.d	$a1, $a1, $t3
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a4, 28
	rotri.d	$t1, $a4, 34
	xor	$a1, $a1, $t1
	rotri.d	$t1, $a4, 39
	xor	$a1, $a1, $t1
	xor	$t3, $a4, $a2
	and	$t1, $t3, $t4
	xor	$t1, $t1, $a2
	add.d	$a1, $a1, $t1
	add.d	$t1, $a0, $a5
	add.d	$a5, $a1, $a0
	rotri.d	$a0, $s6, 1
	rotri.d	$a1, $s6, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s6, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $s0, 19
	rotri.d	$t4, $s0, 61
	xor	$a1, $a1, $t4
	srli.d	$t4, $s0, 6
	xor	$a1, $a1, $t4
	add.d	$a0, $a0, $s2
	add.d	$a0, $a0, $t5
	add.d	$s2, $a0, $a1
	rotri.d	$a0, $t1, 14
	rotri.d	$a1, $t1, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t1, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 56
	andn	$t4, $a7, $t1
	and	$ra, $t0, $t1
	or	$t4, $ra, $t4
	add.d	$a1, $a1, $s2
	add.d	$a1, $a1, $t2
	add.d	$a1, $a1, $t4
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a5, 28
	rotri.d	$t2, $a5, 34
	xor	$a1, $a1, $t2
	rotri.d	$t2, $a5, 39
	xor	$a1, $a1, $t2
	xor	$t4, $a5, $a4
	and	$t2, $t4, $t3
	xor	$t2, $t2, $a4
	add.d	$a1, $a1, $t2
	add.d	$t2, $a0, $a6
	add.d	$a6, $a1, $a0
	ld.d	$s3, $sp, 232                   # 8-byte Folded Reload
	rotri.d	$a0, $s3, 1
	rotri.d	$a1, $s3, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s3, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $s1, 19
	rotri.d	$t3, $s1, 61
	xor	$a1, $a1, $t3
	srli.d	$t3, $s1, 6
	xor	$a1, $a1, $t3
	add.d	$a0, $a0, $s6
	st.d	$t6, $sp, 296                   # 8-byte Folded Spill
	add.d	$a0, $a0, $t6
	add.d	$t6, $a0, $a1
	rotri.d	$a0, $t2, 14
	rotri.d	$a1, $t2, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t2, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 64
	andn	$t3, $t0, $t2
	and	$ra, $t1, $t2
	or	$t3, $ra, $t3
	add.d	$a1, $a1, $t6
	add.d	$a1, $a1, $a7
	add.d	$a1, $a1, $t3
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a6, 28
	rotri.d	$a7, $a6, 34
	xor	$a1, $a1, $a7
	rotri.d	$a7, $a6, 39
	xor	$a1, $a1, $a7
	xor	$t3, $a6, $a5
	and	$a7, $t3, $t4
	xor	$a7, $a7, $a5
	add.d	$a1, $a1, $a7
	add.d	$a7, $a0, $a2
	add.d	$a2, $a1, $a0
	ld.d	$t4, $sp, 224                   # 8-byte Folded Reload
	rotri.d	$a0, $t4, 1
	rotri.d	$a1, $t4, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $t4, 7
	move	$s6, $t4
	xor	$a0, $a0, $a1
	rotri.d	$a1, $s2, 19
	rotri.d	$t4, $s2, 61
	xor	$a1, $a1, $t4
	srli.d	$t4, $s2, 6
	xor	$a1, $a1, $t4
	add.d	$a0, $a0, $s3
	st.d	$t7, $sp, 288                   # 8-byte Folded Spill
	add.d	$a0, $a0, $t7
	add.d	$t7, $a0, $a1
	rotri.d	$a0, $a7, 14
	rotri.d	$a1, $a7, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $a7, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 72
	andn	$t4, $t1, $a7
	and	$ra, $t2, $a7
	or	$t4, $ra, $t4
	add.d	$a1, $a1, $t7
	add.d	$a1, $a1, $t0
	add.d	$a1, $a1, $t4
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a2, 28
	rotri.d	$t0, $a2, 34
	xor	$a1, $a1, $t0
	rotri.d	$t0, $a2, 39
	xor	$a1, $a1, $t0
	xor	$t4, $a2, $a6
	and	$t0, $t4, $t3
	xor	$t0, $t0, $a6
	add.d	$a1, $a1, $t0
	add.d	$t0, $a0, $a4
	add.d	$a4, $a1, $a0
	rotri.d	$a0, $s5, 1
	rotri.d	$a1, $s5, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s5, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t6, 19
	rotri.d	$t3, $t6, 61
	xor	$a1, $a1, $t3
	srli.d	$t3, $t6, 6
	xor	$a1, $a1, $t3
	add.d	$a0, $a0, $s6
	st.d	$t8, $sp, 280                   # 8-byte Folded Spill
	add.d	$a0, $a0, $t8
	add.d	$t8, $a0, $a1
	rotri.d	$a0, $t0, 14
	rotri.d	$a1, $t0, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t0, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 80
	andn	$t3, $t2, $t0
	and	$ra, $a7, $t0
	or	$t3, $ra, $t3
	add.d	$a1, $a1, $t8
	add.d	$a1, $a1, $t1
	add.d	$a1, $a1, $t3
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a4, 28
	rotri.d	$t1, $a4, 34
	xor	$a1, $a1, $t1
	rotri.d	$t1, $a4, 39
	xor	$a1, $a1, $t1
	xor	$t3, $a4, $a2
	and	$t1, $t3, $t4
	xor	$t1, $t1, $a2
	add.d	$a1, $a1, $t1
	add.d	$t1, $a0, $a5
	add.d	$a5, $a1, $a0
	ld.d	$t4, $sp, 216                   # 8-byte Folded Reload
	rotri.d	$a0, $t4, 1
	rotri.d	$a1, $t4, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $t4, 7
	move	$s3, $t4
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t7, 19
	rotri.d	$t4, $t7, 61
	xor	$a1, $a1, $t4
	srli.d	$t4, $t7, 6
	xor	$a1, $a1, $t4
	add.d	$a0, $a0, $s5
	st.d	$fp, $sp, 272                   # 8-byte Folded Spill
	add.d	$a0, $a0, $fp
	add.d	$fp, $a0, $a1
	rotri.d	$a0, $t1, 14
	rotri.d	$a1, $t1, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t1, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 88
	andn	$t4, $a7, $t1
	and	$ra, $t0, $t1
	or	$t4, $ra, $t4
	add.d	$a1, $a1, $fp
	add.d	$a1, $a1, $t2
	add.d	$a1, $a1, $t4
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a5, 28
	rotri.d	$t2, $a5, 34
	xor	$a1, $a1, $t2
	rotri.d	$t2, $a5, 39
	xor	$a1, $a1, $t2
	xor	$t4, $a5, $a4
	and	$t2, $t4, $t3
	xor	$t2, $t2, $a4
	add.d	$a1, $a1, $t2
	add.d	$t2, $a0, $a6
	add.d	$a6, $a1, $a0
	rotri.d	$a0, $s4, 1
	rotri.d	$a1, $s4, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s4, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t8, 19
	rotri.d	$t3, $t8, 61
	xor	$a1, $a1, $t3
	srli.d	$t3, $t8, 6
	xor	$a1, $a1, $t3
	add.d	$a0, $a0, $s3
	st.d	$s0, $sp, 264                   # 8-byte Folded Spill
	add.d	$a0, $a0, $s0
	add.d	$s0, $a0, $a1
	rotri.d	$a0, $t2, 14
	rotri.d	$a1, $t2, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t2, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 96
	andn	$t3, $t0, $t2
	and	$ra, $t1, $t2
	or	$t3, $ra, $t3
	add.d	$a1, $a1, $s0
	add.d	$a1, $a1, $a7
	add.d	$a1, $a1, $t3
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a6, 28
	rotri.d	$a7, $a6, 34
	xor	$a1, $a1, $a7
	rotri.d	$a7, $a6, 39
	xor	$a1, $a1, $a7
	xor	$t3, $a6, $a5
	and	$a7, $t3, $t4
	xor	$a7, $a7, $a5
	add.d	$a1, $a1, $a7
	add.d	$a7, $a0, $a2
	add.d	$a2, $a1, $a0
	rotri.d	$a0, $s7, 1
	rotri.d	$a1, $s7, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s7, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $fp, 19
	rotri.d	$t4, $fp, 61
	xor	$a1, $a1, $t4
	srli.d	$t4, $fp, 6
	xor	$a1, $a1, $t4
	add.d	$a0, $a0, $s4
	st.d	$s1, $sp, 256                   # 8-byte Folded Spill
	add.d	$a0, $a0, $s1
	add.d	$s4, $a0, $a1
	rotri.d	$a0, $a7, 14
	rotri.d	$a1, $a7, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $a7, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 104
	andn	$t4, $t1, $a7
	and	$ra, $t2, $a7
	or	$t4, $ra, $t4
	add.d	$a1, $a1, $s4
	add.d	$a1, $a1, $t0
	add.d	$a1, $a1, $t4
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a2, 28
	rotri.d	$t0, $a2, 34
	xor	$a1, $a1, $t0
	rotri.d	$t0, $a2, 39
	xor	$a1, $a1, $t0
	xor	$t4, $a2, $a6
	and	$t0, $t4, $t3
	xor	$t0, $t0, $a6
	add.d	$a1, $a1, $t0
	add.d	$t0, $a0, $a4
	add.d	$a4, $a1, $a0
	rotri.d	$a0, $s8, 1
	rotri.d	$a1, $s8, 8
	xor	$a0, $a0, $a1
	srli.d	$a1, $s8, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $s0, 19
	rotri.d	$t3, $s0, 61
	xor	$a1, $a1, $t3
	srli.d	$t3, $s0, 6
	xor	$a1, $a1, $t3
	add.d	$a0, $a0, $s7
	st.d	$s2, $sp, 248                   # 8-byte Folded Spill
	add.d	$a0, $a0, $s2
	add.d	$s7, $a0, $a1
	rotri.d	$a0, $t0, 14
	rotri.d	$a1, $t0, 18
	xor	$a0, $a0, $a1
	rotri.d	$a1, $t0, 41
	xor	$a0, $a0, $a1
	ld.d	$a1, $a3, 112
	andn	$t3, $t2, $t0
	and	$ra, $a7, $t0
	or	$t3, $ra, $t3
	add.d	$a1, $a1, $s7
	add.d	$a1, $a1, $t1
	add.d	$a1, $a1, $t3
	add.d	$a0, $a1, $a0
	rotri.d	$a1, $a4, 28
	rotri.d	$t1, $a4, 34
	xor	$a1, $a1, $t1
	rotri.d	$t1, $a4, 39
	xor	$a1, $a1, $t1
	xor	$t3, $a4, $a2
	and	$t1, $t3, $t4
	xor	$t1, $t1, $a2
	add.d	$a1, $a1, $t1
	add.d	$t1, $a0, $a5
	add.d	$a5, $a1, $a0
	rotri.d	$a0, $t5, 1
	rotri.d	$a1, $t5, 8
	xor	$a0, $a0, $a1
	st.d	$t5, $sp, 304                   # 8-byte Folded Spill
	srli.d	$a1, $t5, 7
	xor	$a0, $a0, $a1
	rotri.d	$a1, $s4, 19
	rotri.d	$t4, $s4, 61
	xor	$a1, $a1, $t4
	srli.d	$t4, $s4, 6
	xor	$a1, $a1, $t4
	add.d	$a0, $a0, $s8
	add.d	$a0, $a0, $t6
	add.d	$s8, $a0, $a1
	ld.d	$a0, $a3, 120
	rotri.d	$a1, $t1, 14
	rotri.d	$a3, $t1, 18
	xor	$a1, $a1, $a3
	rotri.d	$a3, $t1, 41
	xor	$a1, $a1, $a3
	andn	$a3, $a7, $t1
	and	$t4, $t0, $t1
	or	$a3, $t4, $a3
	add.d	$a0, $a0, $s8
	add.d	$a0, $a0, $t2
	add.d	$a0, $a0, $a3
	add.d	$a0, $a0, $a1
	rotri.d	$a1, $a5, 28
	rotri.d	$a3, $a5, 34
	xor	$a1, $a1, $a3
	rotri.d	$a3, $a5, 39
	xor	$a1, $a1, $a3
	xor	$a3, $a5, $a4
	and	$t2, $a3, $t3
	xor	$t2, $t2, $a4
	add.d	$a1, $a1, $t2
	add.d	$a6, $a0, $a6
	ld.d	$t3, $sp, 240                   # 8-byte Folded Reload
	addi.d	$t3, $t3, 128
	add.d	$t2, $a1, $a0
	move	$t4, $t3
	ori	$a0, $zero, 512
	bne	$t3, $a0, .LBB0_3
# %bb.4:                                #   in Loop: Header=BB0_2 Depth=1
	ld.d	$fp, $sp, 144                   # 8-byte Folded Reload
	add.d	$fp, $fp, $t2
	ld.d	$t6, $sp, 152                   # 8-byte Folded Reload
	add.d	$t6, $t6, $a5
	ld.d	$t5, $sp, 160                   # 8-byte Folded Reload
	add.d	$t5, $t5, $a4
	ld.d	$t7, $sp, 168                   # 8-byte Folded Reload
	add.d	$t7, $t7, $a2
	ld.d	$t3, $sp, 176                   # 8-byte Folded Reload
	add.d	$t3, $t3, $a6
	ld.d	$t2, $sp, 184                   # 8-byte Folded Reload
	add.d	$t2, $t2, $t1
	ld.d	$a0, $sp, 200                   # 8-byte Folded Reload
	add.d	$t0, $a0, $t0
	ld.d	$a1, $sp, 192                   # 8-byte Folded Reload
	addi.d	$a1, $a1, 128
	ld.d	$a5, $sp, 208                   # 8-byte Folded Reload
	add.d	$a5, $a5, $a7
	ld.d	$a0, $sp, 136                   # 8-byte Folded Reload
	bne	$a1, $a0, .LBB0_2
# %bb.5:
	ld.d	$a0, $sp, 0                     # 8-byte Folded Reload
	st.d	$fp, $a0, 0
	st.d	$t6, $a0, 8
	st.d	$t5, $a0, 16
	st.d	$t7, $a0, 24
	st.d	$t3, $a0, 32
	st.d	$t2, $a0, 40
	st.d	$t0, $a0, 48
	st.d	$a5, $a0, 56
.LBB0_6:
	ld.d	$s8, $sp, 312                   # 8-byte Folded Reload
	ld.d	$s7, $sp, 320                   # 8-byte Folded Reload
	ld.d	$s6, $sp, 328                   # 8-byte Folded Reload
	ld.d	$s5, $sp, 336                   # 8-byte Folded Reload
	ld.d	$s4, $sp, 344                   # 8-byte Folded Reload
	ld.d	$s3, $sp, 352                   # 8-byte Folded Reload
	ld.d	$s2, $sp, 360                   # 8-byte Folded Reload
	ld.d	$s1, $sp, 368                   # 8-byte Folded Reload
	ld.d	$s0, $sp, 376                   # 8-byte Folded Reload
	ld.d	$fp, $sp, 384                   # 8-byte Folded Reload
	ld.d	$ra, $sp, 392                   # 8-byte Folded Reload
	addi.d	$sp, $sp, 400
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
