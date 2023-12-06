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
	unsigned int rm:3;
	unsigned int regOrOpcode:3;
	unsigned int mod:2;
} *ModRM; 

typedef struct sib{
//	int base:3;
//	int index:3;
//	int scale:2;	
	unsigned int base:3;
	unsigned int index:3;
	unsigned int scale:2;	
	// TODO 新增一个offset成员，没有问题吧？
	int offset;
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

typedef enum {
	FPU, NOT_FPU
} INSTR_TYPE_FPU;

typedef enum {
	Zero, One, Two
} INSTR_TYPE_OPRAND_COUNT;

typedef struct instructionType{
	INSTR_TYPE_FPU fpuType;
	INSTR_TYPE_OPRAND_COUNT	oprandCount;
} *InstructionType;


// 加上后缀Info纯属无奈，避免重名。
typedef struct regInfo{
	unsigned char index;
	unsigned char size;
	char *name;
//	unsigned char [4];
} *RegInfo;

// todo 想不到更好的名字了。
// 内存地址有几种形式？0x1234，-4(%ebp)，num11。
struct memoryAddress{
	// 寄存器基址。
	char reg;
	// -4(%ebp)中的-4。
	int offset;
};

typedef union{
	// TODO 有更好的变量名吗？
	struct sib sib;	
	struct memoryAddress regBaseMem;
	int immediate;
	// TODO mem用来存储0x1234这种内存地址。我认为，(0x1234)等价于0x1234。
	// num11不会出现在指令中。链接时会不会出现？没法思考那个时候的问题，不熟悉。
	int immBaseMem;
	RegInfo reg;
} OprandValue;

// TODO 不知道关于内存的猜想是否正确。
// 分别是立即数、SIB、内存地址(寄存器基址、32位基址)、寄存器。
typedef enum{
	IMM, T_SIB, REG_BASE_MEM, IMM_BASE_MEM, REG
} OprandType;

// 操作数。
typedef struct oprand{
	OprandValue value;
	OprandType type;
} *Oprand;

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
// lexer的快照。
Lexer snapshotLexer;
unsigned int instrStreamIndex;
Label currentLabel;
FILE *fp;

typedef struct {
  	char reg8[3];
  	char reg16[3];
  	char reg32[4];
}Register;

typedef enum{
	EIGHT, SIXTEEN, THIRTY_TWO
}OFFSET_TYPE;

/**
 *instr.o:/home/cg/compiler-basic/v4/asm/common.h:226: multiple definition of `registers'
assembler.o:/home/cg/compiler-basic/v4/asm/common.h:226: first defined here
common.o:/home/cg/compiler-basic/v4/asm/common.h:226: multiple definition of `registers'
assembler.o:/home/cg/compiler-basic/v4/asm/common.h:226: first defined here
collect2: error: ld returned 1 exit status
加上static后能消除上面的错误。我只是碰运气试试，结果修复了这个错误。我并不知道这样做的原理。
我并不知道这种错误出现的原因。其他变量例如前面的fp为什么没有出现这种问题？
 */
static Register registers[8] = {
  {"al", "ax", "eax"},
  {"cl", "cx", "ecx"},
  {"dl",  "dx",	"edx"},
  {"bl",  "bx",	"ebx"},
  {"ah",  "sp",	"esp"},
  {"ch",   "bp",	"ebp"},
  {"dh",   "si",	"esi"},
  {"bh",   "di",	"edi"},
};

void StrToUpper(char *str, char *upperStr);
void StrToLower(char *str, char *lowerStr);
void UcFirst(char *str);

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
void StartPeekToken();
void EndPeekToken();
// todo 放在这里并不合适。
int StrToNumber(char *str);
InstructionSet FindInstrCode(char *instr);
char HaveParenthesis(char *str);
char IsMemoryAddress(int token, char *str);
char IsSibOrOtherMemory(int token, char *str);
InstructionType GetInstructionType(InstructionSet instrCode);
RegInfo FindRegIndex(char *regName);

#endif
