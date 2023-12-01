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


#endif
