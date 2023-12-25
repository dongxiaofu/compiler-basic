#PYTHON:everything
# 指定默认动作是all。在终端执行make，默认会执行make all。
PYTHON:all

# H_SRC			= assembler.h instr.h common.h
# T_SRC			= tokens.txt
CFLAGS	= -I ./ -g -m32
CC          = gcc
LD	=	ld
LDFLAGS		= -m elf_i386

SCANNER = link
# 包含main函数的.o文件必须放在前面。
OBJS	= linker.o common.o main.o linklist.o

all:clean everything

everything:${SCANNER} ${OBJS}

${SCANNER}:${OBJS}
	$(CC) ${CFLAGS}  -o $@ $^

clean:
	rm -f *.o ${SCANNER}

main.o:main.c
	${CC} ${CFLAGS} -c -o $@ $<

common.o:common.c
	${CC} ${CFLAGS} -c -o $@ $<

linker.o:linker.c
	${CC} ${CFLAGS} -c -o $@ $<

linklist.o:linklist.c
	${CC} ${CFLAGS} -c -o $@ $<
