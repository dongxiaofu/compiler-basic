max:
	pushl %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	movl %esp, %ebp
	subl $12, %esp
.BB0:
	leal .str2, %eax
	movl %eax, -4(%ebp)
	movl 20(%ebp), %ecx
	cmpl 24(%ebp), %ecx
	jle .BB2
.BB1:
	movl 20(%ebp), %eax
	movl %eax, -12(%ebp)
	jmp .BB3
.BB2:
	movl 24(%ebp), %eax
	movl %eax, -12(%ebp)
.BB3:
	movl -12(%ebp), %eax
	movl %ebp, %esp
	popl %edi
	popl %esi
	popl %ebx
	popl %ebp
	ret

.globl	main

main:
	pushl %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	movl %esp, %ebp
	subl $52, %esp
.BB4:
	leal .str3, %eax
	movl %eax, -4(%ebp)
	leal .str4, %ecx
	movl %ecx, -8(%ebp)
	movl notice, %edx
	movl %edx, -12(%ebp)
	movl num, %edx
	movl %edx, -16(%ebp)
	leal max, %edx
	movl %edx, -20(%ebp)
	pushl -28(%ebp)
	pushl -24(%ebp)
	call *-20(%ebp)
	addl $8, %esp
	pushl -32(%ebp)
	pushl %eax
	call *-20(%ebp)
	addl $8, %esp
	movl %eax, -36(%ebp)
	movl $0, %eax
	movl %ebp, %esp
	popl %edi
	popl %esi
	popl %ebx
	popl %ebp
	ret

