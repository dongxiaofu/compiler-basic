.data



.globl	num

num:	.long	45


.text

.globl	max

max:
	pushl %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	movl %esp, %ebp
	subl $12, %esp
.BB0:
	movl $100, -4(%ebp)
	movl $200, -8(%ebp)
	movl -4(%ebp), %eax
	addl -8(%ebp), %eax
	movl %eax, num
	movl num, %eax
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
	subl $28, %esp
.BB1:
	movl num, %eax
	movl %eax, -4(%ebp)
	movl $5, -8(%ebp)
	pushl -8(%ebp)
	pushl -4(%ebp)
	call max
	addl $8, %esp
	movl %eax, -12(%ebp)
	movl num, %eax
	movl %eax, -16(%ebp)
	movl -12(%ebp), %eax
	subl -16(%ebp), %eax
	movl %eax, -20(%ebp)
	movl -20(%ebp), %ecx
	addl $2, %ecx
	movl %ecx, -20(%ebp)
	movl -20(%ebp), %eax
	movl %ebp, %esp
	popl %edi
	popl %esi
	popl %ebx
	popl %ebp
	ret

