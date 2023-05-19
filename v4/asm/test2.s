.data

.str0:	.string	"Attention!"
.str1:	.string	"Attention2!"


.align 4
.globl	num

num:	.long	5

.globl	num2

num2:	.long	50

.globl	notice

notice:	.long	.str0

.globl	notice2

notice2:	.long	.str1

.globl	ch1

ch1:	.byte	65

.globl	ch2

ch2:	.byte	38

.align 4
.globl	ptr

ptr:	.long	num

.globl	num3

num3:	.long	1091462758



.text

.globl	max

max:
	pushl %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	movl %esp, %ebp
.BB0:
	movl 20(%ebp), %eax
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
	subl $12, %esp
.BB1:
	movl $89, -4(%ebp)
	movl $5, -8(%ebp)
	pushl -8(%ebp)
	pushl -4(%ebp)
	call max
	addl $8, %esp
	movl %eax, -12(%ebp)
	movl $0, %eax
	movl %ebp, %esp
	popl %edi
	popl %esi
	popl %ebx
	popl %ebp
	ret

