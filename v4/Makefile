#PYTHON:everything
# 指定默认动作是all。在终端执行make，默认会执行make all。
PYTHON:all

#C_SRC	=	lex.c stmt.c expr.c
H_SRC			= lex.h stmt.h
T_SRC			= tokens.txt
CFLAGS	= -I ./include -g -m32 -O0
#OBJS        = $(C_SRC:.c=.o)
# CC          = ucc
CC          = gcc
LD	=	ld
LDFLAGS		= -m elf_i386

SCANNER = cgc
# 包含main函数的.o文件必须放在前面。
#OBJS	= cgc.o decl.o lex.o expr.o
#OBJS	= cgc.o decl.o lex.o expr.o input.o
OBJS	= cgc.o decl.o lex.o expr.o input.o type2.o stmt.o packages.o declchk.o \
			symbol.o stmtchk.o exprchk.o \
			tranexpr.o transtmt.o gen.o  \
			dumpir.o	\
			emit.o
#OBJS	= stmt.o lex.o expr.o decl.o
#OBJS	= lex.o

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
	rm -f *.o lex cgc

cgc.o:cgc.c
	#${CC} ${CFLAGS} -c -o $@ $<
	${CC} ${CFLAGS} -c cgc.c

lex.o:lex.c
	#${CC} ${CFLAGS} -c -o $@ $<
	${CC} ${CFLAGS} -c lex.c

emit.o:emit.c
	${CC} ${CFLAGS} -c -o $@ $<
dumpir.o:dumpir.c
	${CC} ${CFLAGS} -c -o $@ $<
gen.o:gen.c
	${CC} ${CFLAGS} -c -o $@ $<
transtmt.o:transtmt.c
	${CC} ${CFLAGS} -c -o $@ $<
tranexpr.o:tranexpr.c
	${CC} ${CFLAGS} -c -o $@ $<
exprchk.o:exprchk.c
	${CC} ${CFLAGS} -c -o $@ $<
symbol.o:symbol.c
	${CC} ${CFLAGS} -c -o $@ $<
declchk.o:declchk.c
	${CC} ${CFLAGS} -c -o $@ $<
stmtchk.o:stmtchk.c
	${CC} ${CFLAGS} -c -o $@ $<
input.o:input.c
	${CC} ${CFLAGS} -c -o $@ $<
expr.o:expr.c
	${CC} ${CFLAGS} -c -o $@ $<
decl.o:decl.c
	${CC} ${CFLAGS} -c -o $@ $<
type2.o:type.c
	${CC} ${CFLAGS} -c -o $@ $<
stmt.o:stmt.c
	${CC} ${CFLAGS} -c -o $@ $<
packages.o:packages.c
	${CC} ${CFLAGS} -c -o $@ $<
