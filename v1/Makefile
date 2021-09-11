#PYTHON:everything
# 指定默认动作是all。在终端执行make，默认会执行make all。
PYTHON:all

#C_SRC	=	lex.c stmt.c expr.c
H_SRC			= lex.h stmt.h
T_SRC			= tokens.txt
CFLAGS	= -I ./include -g -m32
#OBJS        = $(C_SRC:.c=.o)
# CC          = ucc
CC          = gcc
LD	=	ld
LDFLAGS		= -m elf_i386

SCANNER = lex
# 包含main函数的.o文件必须放在前面。
#OBJS	= lex.o	stmt.o
OBJS	= stmt.o lex.o expr.o decl.o

all:clean everything

everything:${SCANNER} ${OBJS}

${SCANNER}:${OBJS}
#${SCANNER}:lex.o stmt.o
	#$(CC) ${CFLAGS} -o $@ $^
	# $<能消除下列错误：
	# include/lex.h:22: multiple definition of `token_names'
	$(CC) ${CFLAGS}  -o $@ $^
	#$(CC) ${CFLAGS}  -o ${SCANNER} ${OBJS}

clean:
	rm -f *.o lex

lex.o:lex.c
	#${CC} ${CFLAGS} -c -o $@ $<
	${CC} ${CFLAGS} -c lex.c

stmt.o:stmt.c
	#${CC} ${CFLAGS} -c -o $@ $<
	${CC} ${CFLAGS} -c stmt.c

expr.o:expr.c
	${CC} ${CFLAGS} -c -o $@ $<
decl.o:decl.c
	${CC} ${CFLAGS} -c -o $@ $<
