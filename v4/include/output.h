#ifndef OUTPUT_H
#define OUTPUT_H

FILE *ASMFile;

#define DST irinst->opds[0]
#define SRC1 irinst->opds[1]
#define SRC2 irinst->opds[2]
#define OP irinst->opcode

// 汇编函数中预留的栈空间的数量，单位是4个字节。
#define PRESERVE_REGS	4
// 栈空间的单位。
#define STACK_SIZE	4

#endif
