	.file	"return-value.c"
# GNU C17 (GCC) version 8.3.1 20191121 (Red Hat 8.3.1-5) (x86_64-redhat-linux)
#	compiled by GNU C version 8.3.1 20191121 (Red Hat 8.3.1-5), GMP version 6.1.2, MPFR version 3.1.6-p2, MPC version 1.0.2, isl version none
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed:  -imultilib 32 return-value.c -m32 -mtune=generic
# -march=x86-64 -fverbose-asm
# options enabled:  -faggressive-loop-optimizations
# -fasynchronous-unwind-tables -fauto-inc-dec -fchkp-check-incomplete-type
# -fchkp-check-read -fchkp-check-write -fchkp-instrument-calls
# -fchkp-narrow-bounds -fchkp-optimize -fchkp-store-bounds
# -fchkp-use-static-bounds -fchkp-use-static-const-bounds
# -fchkp-use-wrappers -fcommon -fdelete-null-pointer-checks
# -fdwarf2-cfi-asm -fearly-inlining -feliminate-unused-debug-types
# -ffp-int-builtin-inexact -ffunction-cse -fgcse-lm -fgnu-runtime
# -fgnu-unique -fident -finline-atomics -fira-hoist-pressure
# -fira-share-save-slots -fira-share-spill-slots -fivopts
# -fkeep-static-consts -fleading-underscore -flifetime-dse
# -flto-odr-type-merging -fmath-errno -fmerge-debug-strings
# -fpcc-struct-return -fpeephole -fplt -fprefetch-loop-arrays
# -fsched-critical-path-heuristic -fsched-dep-count-heuristic
# -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
# -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
# -fsched-stalled-insns-dep -fschedule-fusion -fsemantic-interposition
# -fshow-column -fshrink-wrap-separate -fsigned-zeros
# -fsplit-ivs-in-unroller -fssa-backprop -fstdarg-opt
# -fstrict-volatile-bitfields -fsync-libcalls -ftrapping-math -ftree-cselim
# -ftree-forwprop -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
# -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop
# -ftree-reassoc -ftree-scev-cprop -funit-at-a-time -funwind-tables
# -fverbose-asm -fzero-initialized-in-bss -m32 -m80387 -m96bit-long-double
# -malign-stringops -mavx256-split-unaligned-load
# -mavx256-split-unaligned-store -mfancy-math-387 -mfp-ret-in-387 -mfxsr
# -mglibc -mieee-fp -mlong-double-80 -mmmx -mno-red-zone -mno-sse4
# -mpush-args -msahf -msse -msse2 -mstv -mtls-direct-seg-refs -mvzeroupper

	.text
	.section	.rodata
.LC0:
	.string	"b = %d\n"
.LC1:
	.string	"node.a = %d\n"
.LC2:
	.string	"str = %s\n"
.LC3:
	.string	"*c = %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	leal	4(%esp), %ecx	#,
	.cfi_def_cfa 1, 0
	andl	$-16, %esp	#,
	pushl	-4(%ecx)	#
	pushl	%ebp	#
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp	#,
	pushl	%ecx	#
	.cfi_escape 0xf,0x3,0x75,0x7c,0x6
	subl	$20, %esp	#,
# return-value.c:21: 	int b = test();
	call	test	#
	movl	%eax, -12(%ebp)	# tmp91, b
# return-value.c:22: 	printf("b = %d\n", b);
	subl	$8, %esp	#,
	pushl	-12(%ebp)	# b
	pushl	$.LC0	#
	call	printf	#
	addl	$16, %esp	#,
# return-value.c:24: 	Node node2 = test2();
	call	test2	#
	movl	%eax, -16(%ebp)	# tmp92, node2
# return-value.c:25: 	printf("node.a = %d\n", node2->a);
	movl	-16(%ebp), %eax	# node2, tmp93
	movl	(%eax), %eax	# node2_8->a, _1
	subl	$8, %esp	#,
	pushl	%eax	# _1
	pushl	$.LC1	#
	call	printf	#
	addl	$16, %esp	#,
# return-value.c:27: 	char *str = test3();
	call	test3	#
	movl	%eax, -20(%ebp)	# tmp94, str
# return-value.c:28: 	printf("str = %s\n", str);
	subl	$8, %esp	#,
	pushl	-20(%ebp)	# str
	pushl	$.LC2	#
	call	printf	#
	addl	$16, %esp	#,
# return-value.c:30: 	int *c = test4();
	call	test4	#
	movl	%eax, -24(%ebp)	# tmp95, c
# return-value.c:31: 	printf("*c = %d\n", *c);
	movl	-24(%ebp), %eax	# c, tmp96
	movl	(%eax), %eax	# *c_14, _2
	subl	$8, %esp	#,
	pushl	%eax	# _2
	pushl	$.LC3	#
	call	printf	#
	addl	$16, %esp	#,
# return-value.c:33: 	return 0;
	movl	$0, %eax	#, _16
# return-value.c:34: }
	movl	-4(%ebp), %ecx	#,
	.cfi_def_cfa 1, 0
	leave	
	.cfi_restore 5
	leal	-4(%ecx), %esp	#,
	.cfi_def_cfa 4, 4
	ret	
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	test
	.type	test, @function
test:
.LFB1:
	.cfi_startproc
	pushl	%ebp	#
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp	#,
	.cfi_def_cfa_register 5
	subl	$16, %esp	#,
# return-value.c:37: 	int a = 5;
	movl	$5, -4(%ebp)	#, a
# return-value.c:38: 	return a;
	movl	-4(%ebp), %eax	# a, _2
# return-value.c:39: }
	leave	
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret	
	.cfi_endproc
.LFE1:
	.size	test, .-test
	.globl	test2
	.type	test2, @function
test2:
.LFB2:
	.cfi_startproc
	pushl	%ebp	#
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp	#,
	.cfi_def_cfa_register 5
	subl	$16, %esp	#,
# return-value.c:43: 	struct node node2 = {3,NULL};
	movl	$3, -12(%ebp)	#, node2.a
	movl	$0, -8(%ebp)	#, node2.next
# return-value.c:45: 	node1 = &node2;
	leal	-12(%ebp), %eax	#, tmp89
	movl	%eax, -4(%ebp)	# tmp89, node1
# return-value.c:46: 	return node1;
	movl	-4(%ebp), %eax	# node1, _5
# return-value.c:47: }
	leave	
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret	
	.cfi_endproc
.LFE2:
	.size	test2, .-test2
	.globl	test5
	.type	test5, @function
test5:
.LFB3:
	.cfi_startproc
	pushl	%ebp	#
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp	#,
	.cfi_def_cfa_register 5
	subl	$16, %esp	#,
# return-value.c:51: 	struct person p2 = {83};
	movl	$0, -12(%ebp)	#, p2
	movl	$0, -8(%ebp)	#, p2
	movl	$83, -12(%ebp)	#, p2.age
# return-value.c:53: 	p1 = &p2;
	leal	-12(%ebp), %eax	#, tmp89
	movl	%eax, -4(%ebp)	# tmp89, p1
# return-value.c:54: 	return p1;
	movl	-4(%ebp), %eax	# p1, _5
# return-value.c:55: }
	leave	
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret	
	.cfi_endproc
.LFE3:
	.size	test5, .-test5
	.globl	test3
	.type	test3, @function
test3:
.LFB4:
	.cfi_startproc
	pushl	%ebp	#
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp	#,
	.cfi_def_cfa_register 5
	subl	$32, %esp	#,
# return-value.c:58: 	char str[20] = "hello,world";
	movl	$1819043176, -20(%ebp)	#, str
	movl	$1870081135, -16(%ebp)	#, str
	movl	$6581362, -12(%ebp)	#, str
	movl	$0, -8(%ebp)	#, str
	movl	$0, -4(%ebp)	#, str
# return-value.c:60: 	return str;
	movl	$0, %eax	#, _3
# return-value.c:61: }
	leave	
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret	
	.cfi_endproc
.LFE4:
	.size	test3, .-test3
	.globl	test4
	.type	test4, @function
test4:
.LFB5:
	.cfi_startproc
	pushl	%ebp	#
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp	#,
	.cfi_def_cfa_register 5
	subl	$16, %esp	#,
# return-value.c:64: 	int a = 78;
	movl	$78, -8(%ebp)	#, a
# return-value.c:65: 	int *b = &a;
	leal	-8(%ebp), %eax	#, tmp89
	movl	%eax, -4(%ebp)	# tmp89, b
# return-value.c:66: 	return b;
	movl	-4(%ebp), %eax	# b, _4
# return-value.c:67: }
	leave	
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret	
	.cfi_endproc
.LFE5:
	.size	test4, .-test4
	.ident	"GCC: (GNU) 8.3.1 20191121 (Red Hat 8.3.1-5)"
	.section	.note.GNU-stack,"",@progbits
