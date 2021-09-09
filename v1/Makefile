C_SRC	=	lex.c
H_SRC			= tokens.txt
CFLAGS	= -I ./ -g -m32
#OBJS        = $(C_SRC:.c=.o)
# CC          = ucc
CC          = gcc


all:  $(C_SRC) $(H_SRC)
	$(CC) ${CFLAGS} -o lex $(C_SRC)

clean:
	rm -f *.o lex

