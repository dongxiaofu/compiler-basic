#ifndef COMMON_H
#define COMMON_H

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
} ModRM; 

typedef struct sib{
	int base:3;
	int index:3;
	int scale:2;	
} *SIB;

typedef struct instruction{
	// 前缀。
	char prefix;
	// 操作码。
	char opcode;
	// ModR/M。
	ModRM modRM;
	// SIB。
	SIB sib;
	// 偏移。
	int offset; 
	// 立即数。
	int immediate;
} *Instruction;


#endif
