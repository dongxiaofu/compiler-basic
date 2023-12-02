#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct mblock
{
    struct mblock *next;
    char *begin;
    char *avail;
    char *end;
};

typedef struct heap
{
    struct mblock *last;
    struct mblock head;
} *Heap;

#define HEAP(hp)    static struct heap  hp = { &hp.head }
//#define HEAP(hp)    hp = { &hp.head }
#define True 1
#define False 0
#define END_OF_FILE -1

// 一个程序最多有MAX_INSTR_NUM条指令。究竟有多少？这是由什么决定的？
#define MAX_INSTR_NUM	4096
#define MAX_SIZE 4096
#define STATE_NO_STRING 0
#define STATE_IN_STRING	1
#define STATE_END_STRING 2

#define STATE_IN_FUNCTION 1
#define STATE_OUT_FUNCTION 0

// token的种类。
#define TYPE_TOKEN_INVALID			-1
#define TYPE_TOKEN_INSTR			0
#define TYPE_TOKEN_INT				1
#define TYPE_TOKEN_FLOAT			2
#define TYPE_TOKEN_STRING			3
#define TYPE_TOKEN_FUNCTION			4
#define TYPE_TOKEN_LABEL			5

#define TYPE_TOKEN_QUOT				6	//"
#define TYPE_TOKEN_COMMA			7	//,
#define TYPE_TOKEN_COLON			8	//:
#define TYPE_TOKEN_OPEN_PARENTHESES		9	//(
#define TYPE_TOKEN_CLOSE_PARENTHESES		10	//)
#define TYPE_TOKEN_PERCENT_SIGN				11	// %

#define TYPE_TOKEN_LONG				12	// .long
#define TYPE_TOKEN_BYTE				13	// .byte
#define TYPE_TOKEN_DATA				14	// .data
#define TYPE_TOKEN_GLOBL			15	// .globl
#define TYPE_TOKEN_TEXT				16	// .text
#define TYPE_TOKEN_KEYWORD_STRING			17	// .string

#define TYPE_TOKEN_NEWLINE					18	// \n
#define TYPE_TOKEN_INDENT					19	// 例如，str0

#define TYPE_TOKEN_REGISTER					20	// 寄存器，例如，%ebp
#define TYPE_TOKEN_IMMEDIATE				21	// 立即数，例如，$5

#define TYPE_TOKEN_STAR						22	// *

#define TYPE_TOKEN_COMM						23	// .comm
#define TYPE_TOKEN_ALIGN					24	// .align
#define TYPE_TOKEN_OBJECT					25	// @object
#define TYPE_TOKEN_SECTION					26	// .section
#define TYPE_TOKEN_RODATA					27	// .rodata
#define TYPE_TOKEN_TYPE						28	// .type
#define TYPE_TOKEN_SIZE						29	// .size
#define TYPE_TOKEN_DOT_INDENT			    30	// 例如，.LC0
#define TYPE_TOKEN_LOCAL					31	// .local
#define TYPE_TOKEN_CALL						32	// call

#define DATA_TYPE_INVALID					-1
#define DATA_TYPE_INT						0
#define DATA_TYPE_ADDRESS					1

typedef struct _lexer
{
	int string_state;
	int function_state;
	int token_type;
	int currentLine;
	int currentFuncIndex;
	int lineNum;
	int index0, index1;
	char lexeme[MAX_SIZE];
} *Lexer;

typedef struct _Label
{
	char name[MAX_SIZE];
	int index;
	// 行标签所在函数的索引。
	int functionIndex;
	// 行标签指向的指令的索引。
	int targetIndex;
} *Label;

typedef enum{
	#define INSTR_ELEMENT(code, name)   code,
	#include "all_instruction_set.txt"
	#undef INSTR_ELEMENT
}InstructionSet;

#define INSTRUCTION_SETS_SIZE 77

static char instructionSets[INSTRUCTION_SETS_SIZE][8] = {
	#define INSTR_ELEMENT(code, name)   name,
	#include "all_instruction_set.txt"
	#undef INSTR_ELEMENT
};

typedef struct modRM{
	int rm:3;
	int regOrOpcode:3;
	int mod:2;
} *ModRM; 

typedef struct sib{
	int base:3;
	int index:3;
	int scale:2;	
} *SIB;

typedef struct {
	unsigned char primaryOpcode;
	char secondaryOpcode;
}Opcode;

typedef struct instruction{
	// 前缀。
	char prefix;
	// 操作码。
//	unsigned char opcode;
	Opcode opcode;
	// ModR/M。
	ModRM modRM;
	// SIB。
	SIB sib;
	// 偏移。
	int offset; 
	// 立即数。
	int immediate;

	struct instruction *next;
} *Instruction;


//static Heap CurrentHeap;
//static struct heap ProgramHeap;
HEAP(ProgramHeap);
// CurrentHeap = &ProgramHeap;
static Heap CurrentHeap = &ProgramHeap;

// static char **sourceCode;
// static Lexer lexer;
// static unsigned int instrStreamIndex;
// static Label currentLabel;
// static FILE *fp;

// todo 不知道为什么，在这些变量的前面加了static后，在instr.c中，这些变量的值变为0。
char **sourceCode;
Lexer lexer;
unsigned int instrStreamIndex;
Label currentLabel;
FILE *fp;

void *MALLOC(int size);
int HeapAllocate(Heap heap, int size);

int IsWhitespace(char ch);
int IsCharDelimeter(char ch);
int IsCharIdent(char ch);
int IsCharNumeric(char ch);
int IsStringInt(char *str);
int IsStringFloat(char *str);
int IsStringIdent(char *str);
int IsRegister(char *str);
int IsImmediateOperand(char *str);
void TrimWhitespace(char *str);
int SkipToNewline();
char GetLookAheadChar();
int GetLookAheadToken();
int GetNextToken();
int GetNextTokenExceptNewLine();
char *GetCurrentTokenLexeme();


#endif
