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

typedef struct instruction{
	// 前缀。
	char prefix;
	// 操作码。
	unsigned char opcode;
	// ModR/M。
	ModRM modRM;
	// SIB。
	SIB sib;
	// 偏移。
	int offset; 
	// 立即数。
	int immediate;
} *Instruction;


//static Heap CurrentHeap;
//static struct heap ProgramHeap;
HEAP(ProgramHeap);
// CurrentHeap = &ProgramHeap;
static Heap CurrentHeap = &ProgramHeap;

void *MALLOC(int size);
int HeapAllocate(Heap heap, int size);

#endif
