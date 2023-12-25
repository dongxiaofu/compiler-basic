#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "elf.h"
//#include "linker.h"
//#include "linklist.h"

#define MAX_SIZE 200

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

typedef struct _lexer
{
	int string_state;
	int function_state;
	int token_type;
	int currentLine;
	int currentFuncIndex;
	int lineNum;
	int index0, index1;
	char lexeme[200];
} *Lexer;

typedef struct _Label
{
	char name[200];
	int index;
	// 行标签所在函数的索引。
	int functionIndex;
	// 行标签指向的指令的索引。
	int targetIndex;
} *Label;

typedef enum{
	NUM, STRING, SYMBOL_LINK
} NODE_VALUE;

typedef struct symbolLink{
	char *name;
	Elf32_Sym *sym;
} *SymbolLink;

typedef union value{
	int num;
	char *str;
	SymbolLink symLink;
} Value;


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

int GetSubStrIndex(char *subStr, char *str, unsigned int strLength);
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

#endif
